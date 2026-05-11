#include "launcher.h"

#include <QProcess>                 // used to start external programs
#include <QDebug>                   // used for error logging



// -----------------------------
// Open Path - Open file/folder using OS
// -----------------------------
void ItemLauncher::openFileOrFolder(const QString& path) {
    // delegate to system opener
    QProcess::startDetached("xdg-open", {path});
}



// -------------------------------
// ItemLauncher function - decides how to open/execute a given indexedItem
// -------------------------------
void ItemLauncher::launchApplication(const IndexedItem& indexedItem)
{
    // Check what type of item we are dealing with
    switch (indexedItem.getType())
    {



    // -------------------------
    // CASE 1: APPLICATION
    // -------------------------
    case ItemType::Application:
    {

        // Stores executable name (e.g. "firefox")
        QString program;
        // Stores command-line arguments for the program
        QStringList args;

        // Find first space to separate executable from arguments
        int firstSpace = indexedItem.getPath().indexOf(' ');

        // If no space exists, entire string is just the program name
        if (firstSpace == -1)
        {
            program = indexedItem.getPath(); // Full path is the executable
        }

        else
        {
            // Split into program and arguments
            program = indexedItem.getPath().left(firstSpace);

            // Split arguments safely
            args = indexedItem.getPath().mid(firstSpace + 1).split(' ', Qt::SkipEmptyParts);
        }

        // Attempt to start the application without blocking the current process
        bool started = QProcess::startDetached(program, args);

        // Log error if application failed to start
        if (!started)
        {
            qWarning() << "Failed to start app:" << indexedItem.getPath();
        }

        break;
    }



    // -------------------------
    // CASE 2: FILE OR FOLDER
    // -------------------------
    case ItemType::Directory:
    case ItemType::File:
    {
        // Open file or folder using system default application (xdg-open)
        ItemLauncher::openFileOrFolder(indexedItem.getPath());
        break;
    }
    }
}
