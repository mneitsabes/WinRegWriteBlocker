#include <QApplication>

#include "writeblockerengine.h"
#include "systrayview.h"
#include "engineexception.h"

/**
 * Main entry point.
 *
 * @param argc nb of arguments
 * @param argv arguments
 * @return error code
 */
int main(int argc, char *argv[])
{
    // Qt application
    QApplication app(argc, argv);

    // See http://doc.qt.io/qt-4.8/custom-types.html
    qRegisterMetaType<EngineException>();

    // Initialize the engine
    WriteBlockerEngine *wbe = new WriteBlockerEngine(&app);

    // Used to enable the write blocking juste before quitting
    QObject::connect(&app, SIGNAL(aboutToQuit()), wbe, SLOT(enable()));

    // Create the view
    new SystrayView(wbe);

    // Start the engine
    wbe->start();

    return app.exec();
}
