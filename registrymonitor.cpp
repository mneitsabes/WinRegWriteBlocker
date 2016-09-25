#include "registrymonitor.h"

RegistryMonitor::RegistryMonitor(QString hiveAndSubkey, QString entryName) {
    // The entry name is mandatory
    if(entryName.length() == 0)
        throw EngineException("The entryname is incorrect");

    this->entryName = entryName;

    this->hiveAndSubkey = hiveAndSubkey;

    // We split up the full key path in two part : the hive and the subkey path
    int firstSeparatorPosition = hiveAndSubkey.indexOf("\\");

    // We match the string with the HIVE value (WinAPI)
    QString hiveStr = hiveAndSubkey.left(firstSeparatorPosition);
    if(hiveStr == "HKEY_LOCAL_MACHINE")
        this->hive = HKEY_LOCAL_MACHINE;
    else if(hiveStr == "HKEY_USERS")
        this->hive = HKEY_USERS;
    else if(hiveStr == "HKEY_CURRENT_USER")
        this->hive = HKEY_CURRENT_USER;
    else if(hiveStr == "HKEY_CLASSES_ROOT")
        this->hive = HKEY_CLASSES_ROOT;
    else if(hiveStr == "HKEY_CURRENT_CONFIG")
        this->hive = HKEY_CURRENT_CONFIG;
    else
        throw EngineException("The hive is unknown");

    // The subkey part must be in LPCTSTR format (WinAPI)
    QString subKeyStr = hiveAndSubkey.right(hiveAndSubkey.length() - firstSeparatorPosition - 1);
    if(subKeyStr.length() == 0)
        throw EngineException("The subkey is unknown");

    // Convert QString to LPCTSTR
    QByteArray data = subKeyStr.toLocal8Bit();
    const size_t cSize = data.size() + 1;
    wchar_t* wc = new wchar_t[data.size() + 1];
    mbstowcs (wc, data.data(), cSize);
    this->subKey = (LPCTSTR)wc;
}

void RegistryMonitor::run() {
    // The event (https://msdn.microsoft.com/en-us/library/windows/desktop/ms682396(v=vs.85).aspx)
    HANDLE hEvent = CreateEvent(NULL,  // No security attributes
                                false, // Automatic reset
                                false, // No initial event
                                NULL); // No name
    if(hEvent == NULL) {
        emit exceptionRaised(EngineException("CreateEvent() failed"));
        return ;
    }

    // The key handle and the error code
    HKEY hKey;
    long errorCode;

    // Opening the key https://msdn.microsoft.com/en-us/library/windows/desktop/ms724897(v=vs.85).aspx
    errorCode = RegOpenKeyEx(this->hive,   // The hive
                             this->subKey, // The subkey path
                             0,            // No option
                             KEY_NOTIFY,   // Desired access
                             &hKey);       // The key handle
    if(errorCode != ERROR_SUCCESS) {
        emit exceptionRaised(EngineException("RegOpenKeyEx() failed"));
        return ;
    }

    // We handle the request sended to interrupt the thread
    while(!QThread::currentThread()->isInterruptionRequested()) {
        // Ask to be notified if the  value changes (https://msdn.microsoft.com/en-us/library/windows/desktop/ms724892(v=vs.85).aspx)
        errorCode = RegNotifyChangeKeyValue(hKey,     // The key handle
                                            false,    // We want to handle be notify for the specified key, not subkeys
                                            REG_NOTIFY_CHANGE_NAME | REG_NOTIFY_CHANGE_LAST_SET, // Be notified if the value or name changes
                                            hEvent,   // The event handle
                                            true);    // Asynchronous mode
        if(errorCode != ERROR_SUCCESS) {
            emit exceptionRaised(EngineException("RegNotifyChangeKeyValue() failed"));
            return ;
        }

        // This function is blocking, waiting the event
        if(WaitForSingleObject(hEvent, INFINITE) == WAIT_FAILED) {
            emit exceptionRaised(EngineException("WaitForSingleObject() failed"));
            return ;
        } else
            qDebug() << "Value Changed";
            emit valueChanged(this->readCurrentValue()); // Notified
    }

    // Close the key
    errorCode = RegCloseKey(hKey);
    if(errorCode != ERROR_SUCCESS) {
        emit exceptionRaised(EngineException("RegCloseKey() failed"));
        return ;
    }

    // Close the event handle
    if(!CloseHandle(hEvent)){
        emit exceptionRaised(EngineException("CloseHandle() failed"));
        return ;
    }

    return ;
}

QVariant RegistryMonitor::readCurrentValue() {
    QSettings settings(this->hiveAndSubkey, QSettings::NativeFormat);
    return settings.value(this->entryName);
}
