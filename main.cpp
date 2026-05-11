#include <QApplication>
#include <QLocalSocket>
#include <QLocalServer>
#include "presentation/applicationlauncherwidget.h"


/*
 * SERVER_NAME
 *
 * PURPOSE:
 * - Defines the unique identifier for the local IPC server
 * - Ensures only one instance of the application runs
 * - Used for communication between multiple launched instances
 */
const QString SERVER_NAME = "alfred_single_instance";


int main(int argc, char *argv[]) {
    /*
     * APPLICATION INITIALIZATION
     *
     * PURPOSE:
     * - Creates Qt application instance
     * - Initializes event loop and GUI system
     */

    QApplication a(argc, argv);

    /*
     * SINGLE INSTANCE CHECK (CLIENT SIDE)
     *
     * PURPOSE:
     * - Prevent multiple instances of the application
     * - If another instance is running, send a message and exit
     */
    QLocalSocket socket;

    // Attempt to connect to existing running instance
    socket.connectToServer(SERVER_NAME);

    // Wait briefly for connection success
    if (socket.waitForConnected(100)) {

        // Another instance is already running

        // Send command to existing instance

        socket.write("toggle");
        socket.flush();

        // Exit this instance immediately (prevents multiple instances running)
        return 0;
    }


    /*
     * SINGLE INSTANCE SERVER SETUP
     *
     * PURPOSE:
     * - Acts as the main instance listener
     * - Receives messages from future application launches
     */
    QLocalServer server;

    // Remove stale server from previous crash (if exists)
    server.removeServer(SERVER_NAME);

    // Start listening for incoming connections
    if (!server.listen(SERVER_NAME)) {

        qWarning() << "Failed to start single instance server";

        // Exit if single-instance system cannot be enforced
        return 1;
    }


    /*
     * MAIN APPLICATION WINDOW
     *
     * PURPOSE:
     * - Creates the launcher UI
     */
    ApplicationLauncherWidget w;


    /*
     * IPC MESSAGE HANDLER
     *
     * PURPOSE:
     * - Handles communication from second instance
     * - Toggles visibility of main window
     */
    QObject::connect(&server, &QLocalServer::newConnection, [&]() {

        // Accept incoming connection
        QLocalSocket *clientConnection = server.nextPendingConnection();

        // Wait for message data
        if (clientConnection->waitForReadyRead(100)) {

            QByteArray msg = clientConnection->readAll(); // Read incoming message

            /*
             * MESSAGE: "toggle"
             *
             * PURPOSE:
             * - If window is visible → hide it
             * - If hidden → show and focus search box
             */
            if (msg == "toggle") {

                if (w.isVisible())
                    w.hide();

                else {
                    w.show();

                    // Focus search input for immediate typing
                    w.searchBox->setFocus();
                }
            }
        }

        // Close connection after handling message
        clientConnection->disconnectFromServer();
    });


    /*
     * SHOW MAIN WINDOW
     *
     * PURPOSE:
     * - Displays launcher on startup
     */
    w.show();


    /*
     * EVENT LOOP
     *
     * PURPOSE:
     * - Starts Qt event loop
     * - Keeps application running and responsive
     */
    return a.exec();
}
