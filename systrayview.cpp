#include "systrayview.h"

SystrayView::SystrayView(WriteBlockerEngine *wbEngine) {
    // Initialize icons and the systray
    this->trayIcon = new QSystemTrayIcon(QIcon(":/ressources/icon.png"));
    this->iconWriteBlockingOn = new QIcon(":/ressources/protect-on.png");
    this->iconWriteBlockingOff = new QIcon(":/ressources/protect-off.png");

    // Create the menu
    this->menu = new QMenu();
    this->actionActivate = this->menu->addAction(QIcon(":/ressources/ok.png"), "Enable");
    this->actionDisable1Min = this->menu->addAction(QIcon(":/ressources/clock.png"), "Disable 1 minute");
    this->actionDisable = this->menu->addAction(QIcon(":/ressources/warning.png"), "Disable");
    this->trayIcon->setContextMenu(this->menu);

    // Bind the signals to slots
    connect(this->actionActivate, SIGNAL(triggered(bool)), this, SLOT(enableClicked()));
    connect(this->actionDisable1Min, SIGNAL(triggered(bool)), this, SLOT(disable1MinClicked()));
    connect(this->actionDisable, SIGNAL(triggered(bool)), this, SLOT(disableClicked()));

    // Now, we're ready to listen
    this->wbEngine = wbEngine;
    this->wbEngine->addListener(this);
}

SystrayView::~SystrayView() {
    // Hide the icon
    this->trayIcon->setVisible(false);

    // Stop listenning and delete the enggine
    this->wbEngine->removeListener(this);
    delete this->wbEngine;
    this->wbEngine = NULL;

    // Delete ui parts
    delete this->menu;
    this->menu = NULL;

    delete this->trayIcon;
    this->trayIcon = NULL;

    delete this->iconWriteBlockingOn;
    this->iconWriteBlockingOn = NULL;

    delete this->iconWriteBlockingOff;
    this->iconWriteBlockingOff = NULL;

    delete this->actionActivate;
    this->actionActivate = NULL;

    delete this->actionDisable1Min;
    this->actionDisable1Min = NULL;

    delete this->actionDisable;
    this->actionDisable = NULL;
}

void SystrayView::writeBlockerStateChanged(bool isWriteBlocking, WriteBlockerEventSource source, int duration) {
    // If the source is external, we pop up a notification
    if(source == EXTERNAL)
        this->trayIcon->showMessage("WriteBlocker", "A third-party application has changed the write protection status", QSystemTrayIcon::Warning, 30000);

    // Refreshing the menu
    refreshMenu();
}

void SystrayView::writeBlockerError(QString msg) {
    // Show the error message
    QMessageBox::critical(NULL, "WriteBlocker error", msg + "\nThe application will now stop.");

    // The application quits
    QApplication::quit();
}

void SystrayView::disable1MinClicked() {
    this->wbEngine->disable(60);
}

void SystrayView::disableClicked() {
    this->wbEngine->disable(0);
}

void SystrayView::enableClicked() {
    this->wbEngine->enable();
}

void SystrayView::refreshMenu() {
    bool isWriteBlocking = this->wbEngine->isWriteBlockingEnabled();

    this->actionActivate->setVisible(!isWriteBlocking);
    this->actionDisable1Min->setVisible(isWriteBlocking);
    this->actionDisable->setVisible(isWriteBlocking);

    if(isWriteBlocking)
        this->trayIcon->setIcon(*this->iconWriteBlockingOn);
    else
        this->trayIcon->setIcon(*this->iconWriteBlockingOff);

    // Only used for the first time
    this->trayIcon->setVisible(true);
}
