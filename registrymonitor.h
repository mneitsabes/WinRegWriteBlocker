#ifndef REGISTRYMONITOR_H
#define REGISTRYMONITOR_H

#include <windows.h>
#include <winreg.h>
#include <tchar.h>
#include <stdio.h>

#include <QThread>
#include <QVariant>
#include <QString>
#include <QSettings>
#include <QDebug>

#include "engineexception.h"

/**
 * Monitoring a change in value in the registry.
 *
 * This is designed to run in a separate thread. If a exception is raised, it's sended through a signal.
 * If the value changed, a signal is emit.
 */
class RegistryMonitor : public QThread
{
    Q_OBJECT

    private:
        /**
         * The HIVE (eg. HKEY_LOCAL_MACHINE)
         */
        HKEY hive;

        /**
         * The subkey path (eg.SYSTEM\\CurrentControlSet\\Control\\StorageDevicePolicies)
         */
        LPCTSTR subKey;

        /**
         * A string with the full path containing the hive and the subkey (eg. HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\StorageDevicePolicies)
         */
        QString hiveAndSubkey;

        /**
         * The name of the entry that we want to monitor
         */
        QString entryName;

    public:
        /**
         * Constructor.
         *
         * Set things up and wait to start.
         *
         * @param hiveAndSubkey the full path of the key
         * @param entryName the entry that we want to monitor
         */
        RegistryMonitor(QString hiveAndSubkey, QString entryName);

        /**
         * Start the monitoring
         */
        void run();

    signals:
        /**
         * Signal emitted if the value changed.
         *
         * @param newValue the new value
         */
        void valueChanged(QVariant newValue);

        /**
         * Signal emitted if a exception is raised.
         *
         * The thread is stopped.
         *
         * @param e the exception
         */
        void exceptionRaised(EngineException e);

    private:
        /**
         * Returns the current value of the monitored entry.
         *
         * @return the current value
         */
        QVariant readCurrentValue();
};

#endif // REGISTRYMONITOR_H
