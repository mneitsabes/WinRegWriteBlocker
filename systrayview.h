#ifndef SYSTRAYVIEW_H
#define SYSTRAYVIEW_H

#include <QObject>
#include <QMenu>
#include <QAction>
#include <QSystemTrayIcon>
#include <QMessageBox>
#include <QApplication>

#include "writeblockerengine.h"
#include "writeblockerenginelistener.h"
#include "writeblockereventsource.h"

/**
 * The "view" of the application. It's a systray icon and a menu.
 *
 * This view is listenning the engine for event : value changes and error messages.
 */
class SystrayView : public QObject, public WriteBlockerEngineListener
{
    Q_OBJECT

    private:
        /**
         * The engine
         */
        WriteBlockerEngine *wbEngine;

        /**
         * The systray icon
         */
        QSystemTrayIcon *trayIcon;

        /**
         * The menu
         */
        QMenu *menu;

        /**
         * The "Enable" option in the menu
         */
        QAction *actionActivate;

        /**
         * The "Disable 1 min" option in the menu
         */
        QAction *actionDisable1Min;

        /**
         * The "Disable" option in the menu
         */
        QAction *actionDisable;

        /**
         * The systray icon when the write protection if on
         */
        QIcon *iconWriteBlockingOn;

        /**
         * The systray icon when the write protection if off
         */
        QIcon *iconWriteBlockingOff;

    public:
        /**
         * Constructor.
         *
         * Add itself to the engine listeners.
         *
         * @param wbEngine the engine
         */
        SystrayView(WriteBlockerEngine *wbEngine);

        /**
         * Destructor.
         */
        ~SystrayView();

        /**
         * Used by the engine to notify the view that the current state has changed.
         *
         * The new state, the source and the duration is provided. The duration is in secondes (if the write protection is off for 1 minute for example).
         *
         * @param isWriteBlocking the current state
         * @param source the source (see WriteBlockEventSource)
         * @param duration the duration (0 if no duration)
         */
        void writeBlockerStateChanged(bool isWriteBlocking, WriteBlockerEventSource source, int duration);

        /**
         * Used by the engine to notify the view if a error occurs.
         *
         * After showing the message, the application quits.
         *
         * @param msg the error message
         */
        void writeBlockerError(QString msg);

    private:
        /**
         * Refresh the menu depending of the current state.
         *
         * If the state is "write protected on", the "Disable 1 minute" and "Disable" options are available.
         * If the state is "write protected off", the "Enable" option is available.
         *
         * The systray icon changes also : green when the protection is active, red if not.
         */
        void refreshMenu();

    public slots:
        /**
         * Asks the engine to disable the protection for 1 minute.
         *
         * Used when the user clicks on the "Disable 1 min" option
         */
        void disable1MinClicked();

        /**
         * Asks the engine to disable the protection.
         *
         * Used when the user clicks on the "Disable" option
         */
        void disableClicked();

        /**
         * Asks the engine to enable the protection.
         *
         * Used when the user clicks on the "enable" option
         */
        void enableClicked();
};

#endif // SYSTRAYVIEW_H
