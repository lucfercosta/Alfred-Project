#include "filesystemindexer.h"

#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QRegularExpression>
#include <QStandardPaths>
#include <QMap>
#include <QFileInfo>


namespace {

/*
 * ignoredDirectoryNames
 *
 * PURPOSE:
 * - Defines folders that are ignored during filesystem scanning
 * - Prevents indexing of system/noise directories
 */
const QStringList ignoredDirectoryNames = {
    ".git", ".cache", "node_modules", "__pycache__"
};

/*
 * supportedFileExtensions
 *
 * PURPOSE:
 * - Defines which file types are valid for indexing
 * - Filters out irrelevant file formats
 */
const QStringList supportedFileExtensions = {
    "txt", "pdf", "png", "jpg", "jpeg", "mp3", "mp4", "desktop"
};

/*
 * maximumDirectoryTraversalDepth
 *
 * PURPOSE:
 * - Limits recursion depth in filesystem scanning
 * - Prevents infinite traversal and performance issues
 */
const int maximumDirectoryTraversalDepth = 6;

}


    /*
     * buildSearchIndex
     *
     * MAIN ENTRY POINT
     *
     * PURPOSE:
     * - Starts the full indexing process
     * - Calls all scanning functions:
     *   → applications
     *   → directories
     *   → files
     *
     * This is the orchestrator of indexing.
     */
void FileSystemIndexer::buildSearchIndex() {

    indexedItems.clear();
    visitedPaths.clear();

    QStringList searchIndexRootPaths = {
        QStandardPaths::writableLocation(QStandardPaths::HomeLocation),
        QStandardPaths::writableLocation(QStandardPaths::DesktopLocation),
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
        QStandardPaths::writableLocation(QStandardPaths::DownloadLocation),
        QStandardPaths::writableLocation(QStandardPaths::PicturesLocation),
        QStandardPaths::writableLocation(QStandardPaths::MusicLocation),
        QStandardPaths::writableLocation(QStandardPaths::MoviesLocation),
        "/usr/bin",
        "/usr/local/bin",
        "/opt"
    };

    // Index Directories
    for (const QString& searchIndexRootPath : searchIndexRootPaths) {
        indexDirectoryTree(searchIndexRootPath, 0);
    }

    // Reset visited paths so files aren't skipped
    visitedPaths.clear();

    // Index Files
    for (const QString& searchIndexRootPath : searchIndexRootPaths) {
        indexDirectoryFiles(searchIndexRootPath, 0);
    }

    // Index Aplications
    indexLinuxApplications();
}


/*
 * indexDirectoryTree
 *
 * PURPOSE:
 * - Recursively scans directory structure
 * - Adds folders as searchable items
 *
 * PARAMETERS:
 * - rootPath: directory to scan
 * - currentDepth: recursion depth control
 *
 * OUTPUT:
 * - Adds Directory items to indexedItems
 */
void FileSystemIndexer::indexDirectoryTree(const QString& rootPath, int currentDepth) {

    // Stop recursion if too deep
    if (currentDepth > maximumDirectoryTraversalDepth) return;

    QDir directory(rootPath);

    // Skip non existing directories
    if (!directory.exists()) return;

    // Get only subdirectories (not files)
    QFileInfoList directoryEntries = directory.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);

    for (const QFileInfo& directoryEntry : directoryEntries) {

        // Extracts directory name
        QString directoryName = directoryEntry.fileName();

        // Skips hidden folders
        if (directoryName.startsWith(".")) continue;

        // Skips junk/system folders
        if (ignoredDirectoryNames.contains(directoryName)) continue;

        // Extracts absolute folder path
        QString absolutePath = directoryEntry.absoluteFilePath();

        // Skips visited paths
        if (visitedPaths.contains(absolutePath)) continue;

        // Marks path as visited
        visitedPaths.insert(absolutePath);

        // Add folder to the indexed items list
        indexedItems.append(IndexedItem(IndexedItemType::Directory, directoryName, absolutePath));

        // Recursively scan inside this folder
        indexDirectoryTree(absolutePath, currentDepth + 1);
    }
}



/*
 * indexDirectoryFiles
 *
 * PURPOSE:
 * - Scans files inside directories
 * - Applies filtering rules (valid file types only)
 *
 * PARAMETERS:
 * - rootPath: directory to scan
 * - currentDepth: recursion depth control
 *
 * OUTPUT:
 * - File entries added to index
 */
void FileSystemIndexer::indexDirectoryFiles(const QString& rootPath, int currentDepth) {

    // Stop recursion if it's too deep
    if (currentDepth > maximumDirectoryTraversalDepth) return;

    QDir directory(rootPath);
    if (!directory.exists()) return;

    // Get files + folders (we handle both)
    QFileInfoList directoryEntries = directory.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);

    for (const QFileInfo& file : directoryEntries) {

        QString absolutePath = file.absoluteFilePath();

        // Skip duplicates
        if (visitedPaths.contains(absolutePath)) continue;
        visitedPaths.insert(absolutePath);

        // If it's a folder → recurse deeper
        if (file.isDir()) {
            indexDirectoryFiles(absolutePath, currentDepth + 1);
        }
        else {
            // Extracts file extension
            QString fileExtension = file.suffix().toLower();

            // Skip unsupported file types
            if (!supportedFileExtensions.contains(fileExtension)) continue;

            // Add file to index
            indexedItems.append(IndexedItem(IndexedItemType::File, file.fileName(), absolutePath));
        }
    }
}



/*
 * indexLinuxApplications
 *
 * PURPOSE:
 * - Scans Linux .desktop application files
 * - Extracts application metadata
 *
 * PROCESS:
 * - Reads system application directories
 * - Parses .desktop file format
 * - Extracts name, exec command and icon
 *
 * RULES:
 * - Skips hidden applications
 * - Removes Exec placeholders
 * - Handles Flatpak separately
 *
 * OUTPUT:
 * - Adds Application items to index
 */
void FileSystemIndexer::indexLinuxApplications() {

    /*
     * STEP 1 — DEFINE COMMON APPLICATION DIRECTORIES
     *
     * Linux applications are typically registered via `.desktop` files.
     * These files live in standard directories depending on how the app was installed:
     *
     * - System-wide apps (/usr/share, /usr/local/share)
     * - User-installed apps (~/.local/share)
     * - Sandboxed apps (Flatpak, Snap)
     *
     * We scan all of them to build a complete application index.
     */
    QStringList applicationsDirectories = {
        "/usr/share/applications",
        "/usr/local/share/applications",
        QDir::homePath() + "/.local/share/applications",
        "/var/lib/snapd/desktop/applications",
        "/var/lib/flatpak/exports/share/applications",
        QDir::homePath() + "/.local/share/flatpak/exports/share/applications"
    };

    /*
     * STEP 2 — PREPARE DEDUPLICATION STRUCTURE
     *
     * Multiple `.desktop` files can represent the same application.
     * Example: Flatpak + system install, or multiple launchers.
     *
     * We use a map keyed by execution command to ensure uniqueness.
     */
    QMap<QString, IndexedItem> uniqueApplicationByExecutionKey;

    /*
     * STEP 3 — ITERATE THROUGH EACH APPLICATION DIRECTORY
     */
    for (const QString& applicationDirectory : applicationsDirectories) {

        QDir directory(applicationDirectory);

        // Skip directories that do not exist on the system
        if (!directory.exists()) continue;

        /*
         * STEP 4 — PROCESS EACH .desktop FILE
         *
         * Each `.desktop` file describes an application:
         * name, icon, execution command, visibility, etc.
         */
        for (const QString& file : directory.entryList(QStringList() << "*.desktop")) {

            // Variable that holds the .desktop
            QFile desktopFile(directory.filePath(file));

            // Skip unreadable files
            if (!desktopFile.open(QIODevice::ReadOnly)) continue;

            // Responsabile for itearing through lines within .desktop file
            QTextStream in(&desktopFile);

            /*
             * STEP 5 — PARSE DESKTOP ENTRY FIELDS
             *
             * We only care about the [Desktop Entry] section.
             */
            QString applicationName;
            QString executionCommand;
            QString applicationIcon;

            bool isInDesktopEntrySection = false; // Are we inside [Desktop Entry]?
            bool isApplicationHidden = false;  // Should this app be hidden from launcher?

            // While it's not on the last line
            while (!in.atEnd()) {

                // Each line iteration within the .desktop file
                QString desktopEntryLine = in.readLine();

                // Enter relevant section
                if (desktopEntryLine.startsWith("[Desktop Entry]")) {
                    isInDesktopEntrySection = true;
                    continue;
                }

                // Exit section when another section begins
                if (desktopEntryLine.startsWith('[') &&
                    !desktopEntryLine.startsWith("[Desktop Entry]")) {
                    isInDesktopEntrySection = false;
                    continue;
                }

                // Ignore anything outside the main section
                if (!isInDesktopEntrySection) continue;

                /*
                 * Extract relevant fields
                 */

                // Human-readable application name
                if (desktopEntryLine.startsWith("Name="))
                    applicationName = desktopEntryLine.mid(5);

                /*
                 * Execution command
                 *
                 * Example:
                 * Exec=firefox %u
                 *
                 * We remove placeholders (%u, %f, etc.) because they are
                 * runtime arguments, not part of the executable itself.
                 */
                if (desktopEntryLine.startsWith("Exec=")) {
                    executionCommand = desktopEntryLine.mid(5).trimmed();

                    executionCommand.replace(
                        QRegularExpression("%[fFuUdDnNickvm]"), ""
                        );

                    executionCommand = executionCommand.trimmed();
                }

                // Icon identifier (can be a name or path)
                if (desktopEntryLine.startsWith("Icon="))
                    applicationIcon = desktopEntryLine.mid(5);

                /*
                 * Skip apps that should not appear in launchers
                 *
                 * These flags are commonly used for internal/system entries
                 */
                if (desktopEntryLine.startsWith("NoDisplay=true") ||
                    desktopEntryLine.startsWith("Hidden=true"))
                    isApplicationHidden = true;
            }

            /*
             * STEP 6 — VALIDATION
             *
             * Skip incomplete or hidden entries
             */
            if (applicationName.isEmpty() ||
                executionCommand.isEmpty() ||
                isApplicationHidden)
                continue;

            QString executionKey;

            /*
             * STEP 7 — HANDLE DIFFERENT APP TYPES
             */

            // -------------------------------
            // Flatpak applications
            // -------------------------------
            if (executionCommand.startsWith("flatpak run") ||
                executionCommand.startsWith("/usr/bin/flatpak run")) {

                /*
                 * Flatpak apps use full command as identifier
                 * because the executable is always "flatpak"
                 */
                executionKey = executionCommand;

                QString iconPath;

                /*
                 * Flatpak icons are not always resolved automatically.
                 * We manually search known icon directories.
                 */
                QStringList iconSearchDirectories = {
                    QDir::homePath() + "/.local/share/flatpak/exports/share/icons/hicolor/256x256/apps/",
                    "/var/lib/flatpak/exports/share/icons/hicolor/256x256/apps/"
                };

                for (const QString& iconSearchDirectory : iconSearchDirectories) {
                    QFile test(iconSearchDirectory + applicationIcon + ".png");

                    if (test.exists()) {
                        iconPath = test.fileName();
                        break;
                    }
                }

                // Replace icon name with actual path if found
                if (!iconPath.isEmpty())
                    applicationIcon = iconPath;
            }
            else {
                /*
                 * Standard applications
                 *
                 * Use the first word of the Exec command as the executable
                 */
                executionKey = executionCommand.split(' ').first();

                /*
                 * Ensure the executable actually exists in system PATH
                 *
                 * This avoids indexing broken or uninstalled apps
                 */
                if (QStandardPaths::findExecutable(executionKey).isEmpty())
                    continue;
            }

            /*
             * STEP 8 — DEDUPLICATION
             *
             * Prevent duplicate entries for the same app
             */
            if (!uniqueApplicationByExecutionKey.contains(executionKey)) {

                uniqueApplicationByExecutionKey[executionKey] =
                    IndexedItem(
                        IndexedItemType::Application,
                        applicationName,
                        executionCommand,
                        applicationIcon
                        );
            }
            else {
                /*
                 * Prefer better-quality entries
                 *
                 * Example:
                 * Replace "Install Firefox" with "Firefox"
                 */
                IndexedItem existing = uniqueApplicationByExecutionKey[executionKey];

                if (existing.displayName.contains("Install") &&
                    !applicationName.contains("Install")) {

                    uniqueApplicationByExecutionKey[executionKey] =
                        IndexedItem(
                            IndexedItemType::Application,
                            applicationName,
                            executionCommand,
                            applicationIcon
                            );
                }
            }
        }
    }

    /*
     * STEP 9 — FINALIZE INDEX
     *
     * Move unique applications into the main indexedItems list
     */
    for (const IndexedItem& item : uniqueApplicationByExecutionKey) {
        indexedItems.append(item);
    }
}




/*
 * getIndexedItems
 *
 * PURPOSE:
 * - Returns all indexed items
 */
QVector<IndexedItem> FileSystemIndexer::getIndexedItems() const {
    // full index result
    return indexedItems;

}


