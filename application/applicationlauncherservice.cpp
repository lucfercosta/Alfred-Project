#include "applicationlauncherservice.h"
#include <QProcess>


/*
 * Constructor
 */
ApplicationLauncherService::ApplicationLauncherService(QObject* parent) : QObject(parent) {}


/*
 * Main launch function
 *
 * This is the central decision point of the service.
 * It receives:
 * - itemPath: what to execute (application command or file itemPath)
 * - itemType: determines HOW to execute it
 *
 * Based on itemType, it routes the request to the correct handler.
 */
void ApplicationLauncherService::launch(const QString& itemPath, IndexedItemType itemType)
{
    switch (itemType)
    {
    /*
     * If the item is an application:
     * → execute it as a system process
     */
    case IndexedItemType::Application:
        launchApplication(itemPath);
        break;

    /*
     * If the item is a file or directory:
     * → open it using the system default application (xdg-open)
     */
    case IndexedItemType::Directory:
    case IndexedItemType::File:
        openFileOrFolder(itemPath);
        break;
    }
}


/*
 * Launches a Linux application
 *
 * Example input:
 * - "firefox"
 * - "/usr/bin/code"
 * - "flatpak run org.example.App"
 *
 * Problem solved here:
 * - Some commands contain arguments (e.g. "firefox --private-window")
 * - We need to split command from arguments
 */
void ApplicationLauncherService::launchApplication(const QString& itemPath)
{
    /*
     * Find first space:
     * - Everything before = executable
     * - Everything after = arguments
     */
    int firstSpace = itemPath.indexOf(' ');


    /*
     * Extract executable name and arguments
     */

    // If there are no spaces, whole string is the application
    QString applicationPath = (firstSpace == -1) ? itemPath : itemPath.left(firstSpace);

    // If there are spaces, tokenize arguments
    QStringList applicationArguments = (firstSpace == -1) ? QStringList() : itemPath.mid(firstSpace + 1).split(' ', Qt::SkipEmptyParts);

    /*
     * Start process in detached mode:
     * - detached = runs independently of this app
     * - launcher does NOT wait for it to finish
     */
    QProcess::startDetached(applicationPath, applicationArguments);
}
/*
 * Opens a file or folder using the system default application
 *
 * Uses Linux tool:
 * → xdg-open
 *
 * Behavior:
 * - Folder → opens file manager
 * - File → opens default associated app
 */
void ApplicationLauncherService::openFileOrFolder(const QString& fileOrFolderPath)
{
    QProcess::startDetached("xdg-open", { fileOrFolderPath });
}
