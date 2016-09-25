#include "writeblockerengine.h"

WriteBlockerEngine::WriteBlockerEngine(QObject *parent) : QObject(parent) {
    this->listeners = new QVector<WriteBlockerEngineListener *>();

    this->wbRegMonitor = NULL;

    this->mutexStateChange = false;

    this->durationBeforeAutoEnabling = new QTimer();
    connect(this->durationBeforeAutoEnabling, SIGNAL(timeout()), this, SLOT(enable()));
}

WriteBlockerEngine::~WriteBlockerEngine() {
    // We stop the process
    this->wbRegMonitor->requestInterruption();

    delete this->listeners;
    this->listeners = NULL;

    this->durationBeforeAutoEnabling->stop();
    delete this->durationBeforeAutoEnabling;
    this->durationBeforeAutoEnabling = NULL;

    delete this->wbRegMonitor;
    this->wbRegMonitor = NULL;
}

void WriteBlockerEngine::addListener(WriteBlockerEngineListener *listener) {
    if(!this->listeners->contains(listener))
        this->listeners->append(listener);
}

void WriteBlockerEngine::removeListener(WriteBlockerEngineListener *listener) {
    int pos = this->listeners->indexOf(listener);
    if(pos != -1)
        this->listeners->remove(pos);
}

void WriteBlockerEngine::notifyListenersValueChanged(WriteBlockerEventSource source, int duration) {
    bool isWriteBlocking = this->isWriteBlockingEnabled();

    for(int i=0; i < this->listeners->size(); i++)
        this->listeners->at(i)->writeBlockerStateChanged(isWriteBlocking, source, duration);
}

void WriteBlockerEngine::notifyListenersError(QString msg) {
    for(int i=0; i < this->listeners->size(); i++)
        this->listeners->at(i)->writeBlockerError(msg);
}

void WriteBlockerEngine::registryMonitorValueChanged(QVariant newValue) {
    // If the mutex is on, this event is the consequence of this application
    if(this->mutexStateChange) {
        this->mutexStateChange = false;
        return ;
    }

    // We save the new state
    this->writeBlockingEnabled = (newValue == 1);

    // We notify with "external" source
    this->notifyListenersValueChanged(EXTERNAL);
}

void WriteBlockerEngine::registryMonitorExceptionRaised(EngineException e) {
    this->notifyListenersError(e.getMessage());
}

void WriteBlockerEngine::start() {
    // Check the privileges and enabling the protection (if the key doesn't exist, the key is created)
    if(!this->hasSufficientPrivilegesAndEnabling())
        this->notifyListenersError("Initialize or doesn't have the right privilege to change the registry value");
    this->writeBlockingEnabled = true;
    this->mutexStateChange = false;

    // We try to open the key (must never failed if we go here because of the previous check)
    try {
        this->wbRegMonitor = new RegistryMonitor(__HIVE_AND_SUBKEY__, __ENTRYNAME__);

        connect(this->wbRegMonitor, SIGNAL(valueChanged(QVariant)), this, SLOT(registryMonitorValueChanged(QVariant)));
        connect(this->wbRegMonitor, SIGNAL(exceptionRaised(EngineException)), this, SLOT(registryMonitorExceptionRaised(EngineException)));

        // We start the process
        this->wbRegMonitor->start();
    } catch(EngineException e) {
        this->notifyListenersError(e.getMessage());
    }

    // We send a first event
    this->notifyListenersValueChanged(APPLICATION);
}

void WriteBlockerEngine::disable(int duration) {
    try {
        this->toggleWriteBlocking(false);

        // If a duration is provided, we start the timer
        if(duration > 0) {
            // The QTimer works with ms duration
            this->durationBeforeAutoEnabling->setInterval(duration * 1000);
            this->durationBeforeAutoEnabling->start();
        }

        // We notify
        this->notifyListenersValueChanged(APPLICATION, duration);
    } catch(EngineException e) {
        this->notifyListenersError(e.getMessage());
    }
}

void WriteBlockerEngine::enable() {
    // If a timer was running, we stop it
    this->durationBeforeAutoEnabling->stop();

    try {
        this->toggleWriteBlocking(true);

        // We notify
        this->notifyListenersValueChanged(APPLICATION);
    } catch(EngineException e) {
        this->notifyListenersError(e.getMessage());
    }
}

bool WriteBlockerEngine::isWriteBlockingEnabled() {
    return this->writeBlockingEnabled;
}

bool WriteBlockerEngine::hasSufficientPrivilegesAndEnabling() {
    bool success = false;

    try {
        this->toggleWriteBlocking(true);
        success = true;
    } catch(EngineException e) { }

    return success;
}

void WriteBlockerEngine::toggleWriteBlocking(bool enable) {
    int value = (enable) ? 1 : 0;

    // We set the mutex
    this->mutexStateChange = true;

    QSettings settings(__HIVE_AND_SUBKEY__, QSettings::NativeFormat);
    settings.setValue(__ENTRYNAME__, value);

    if(settings.status() != 0)
        throw EngineException("Cannot change the value in the registry");

    this->writeBlockingEnabled = enable;
}
