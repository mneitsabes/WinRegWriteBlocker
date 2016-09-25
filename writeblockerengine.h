#ifndef WRITEBLOCKERENGINE_H
#define WRITEBLOCKERENGINE_H

#include <windows.h>
#include <winreg.h>
#include <tchar.h>
#include <stdio.h>

#include <QObject>
#include <QSettings>
#include <QVector>
#include <QTimer>
#include <QString>

#include "registrymonitor.h"
#include "writeblockereventsource.h"
#include "writeblockerenginelistener.h"

/**
 * The full path containing the hive and the subkey
 */
#define __HIVE_AND_SUBKEY__ "HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\StorageDevicePolicies"

/**
 * The entry name
 */
#define __ENTRYNAME__ "WriteProtect"

class WriteBlockerEngine : public QObject
{
    Q_OBJECT

    private:
        /**
         * Listenners
         */
        QVector<WriteBlockerEngineListener *> *listeners;

        /**
         * The registry monitor process
         */
        RegistryMonitor *wbRegMonitor;

        /**
         * QTimer to auto enable the protection after a specific duration
         */
        QTimer *durationBeforeAutoEnabling;

        /**
         * Current state of the protection
         */
        bool writeBlockingEnabled;

        /**
         * Mutex used to link a user change and a registry changing event
         */
        bool mutexStateChange;

    public:
        /**
         * Constructor.
         *
         * Initialize the engine.
         *
         * @param parent the parent
         */
        WriteBlockerEngine(QObject *parent = 0);

        /**
         * Destructor.
         */
        ~WriteBlockerEngine();

        /**
         * Add a listener to the engine.
         *
         * A listener can be only added once.
         *
         * @param listener the listener
         */
        void addListener(WriteBlockerEngineListener *listener);

        /**
         * Remove the listener.
         *
         * @param listener the listener
         */
        void removeListener(WriteBlockerEngineListener *listener);

        /**
         * Start the engine.
         *
         * A valueChanged event is directly notified.
         */
        void start();

        /**
         * Disable the protection.
         *
         * If the duration > 0, a countdown is enabled which will auto enable the protection when reaching 0.
         *
         * @param duration the duration
         */
        void disable(int duration);

        /**
         * Enable or disable the protection.
         *
         * If we can't change the value, a EngineException is raised
         *
         * @param enable true to enable, false to disable
         */
        void toggleWriteBlocking(bool enable);

        /**
         * Return the current state.
         *
         * @return the current state
         */
        bool isWriteBlockingEnabled();

    public slots:
        /**
         * Enable the protection.
         */
        void enable();

        /**
         * Slot linked with the registry monitoring process to be notified when the value changed.
         *
         * The new state is sended to listeners.
         *
         * @param newValue the new value
         */
        void registryMonitorValueChanged(QVariant newValue);

        /**
         * Slot linked with the registry monitoring process to be notified if a error occurs.
         *
         * The error is sended to listeners.
         *
         * @param e the exception raised
         */
        void registryMonitorExceptionRaised(EngineException e);

    private:
        /**
         * Notify all listeners of the new state
         *
         * @param source the source (see WriteBlockEventSource)
         * @param duration the duration (0 if no duration)
         */
        void notifyListenersValueChanged(WriteBlockerEventSource source, int duration = 0);

        /**
         * Notify all listeners of the error.
         *
         * @param msg the error message
         */
        void notifyListenersError(QString msg);

        /**
         * Check if the current process has sufficient privileges to change the value in the registry.
         *
         * If the key doesn't exist, the key is created. The write protection is also enabled.
         *
         * Must always returns true because the UAC prompt will ask elevation.
         *
         * @return true if sufficient privileges, false if not
         */
        bool hasSufficientPrivilegesAndEnabling();
};

#endif // WRITEBLOCKERENGINE_H
