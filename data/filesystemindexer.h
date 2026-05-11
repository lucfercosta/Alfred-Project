#ifndef FILESYSTEMINDEXER_H                   
#define FILESYSTEMINDEXER_H

#include "IndexedItem.h"

#include <QVector>
#include <QString>
#include <QSet>



/*
 * FileSystemIndexer
 *
 * RESPONSIBILITY (Data Layer):
 * - Scans the Linux filesystem
 * - Finds applications, files, and directories
 * - Converts them into IndexedItem objects
 *
 * This class is part of the DATA LAYER:
 * → It ONLY collects data
 * → It does NOT handle UI or search logic
 */
class FileSystemIndexer {
private:
    /*
     * Container for all discovered items
     *
     * This is the final dataset used by the search engine
     */
    QVector<IndexedItem> indexedItems;

public:
    /*
     * Tracks visited filesystem paths
     *
     * PURPOSE:
     * - Prevents infinite loops during recursion
     * - Prevents duplicate entries
     *
     * Example:
     * /home/user/docs → only indexed once
     */
    QSet<QString> visitedPaths;

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
    void buildSearchIndex();



    /*
     * indexLinuxApplications
     *
     * PURPOSE:
     * - Scans installed Linux applications
     * - Reads .desktop files from system directories
     *
     * OUTPUT:
     * - Adds applications into indexedItems
     *
     * Example:
     * Firefox, VS Code, Terminal, etc.
     */
    void indexLinuxApplications();



    /*
     * indexDirectoryTree
     *
     * PURPOSE:
     * - Recursively scans folders
     * - Builds folder structure as IndexedItems
     *
     * PARAMETERS:
     * - rootPath: starting directory
     * - currentDepth: recursion depth (prevents infinite scanning)
     *
     * OUTPUT:
     * - Directory entries added to index
     */
    void indexDirectoryTree(const QString& rootPath, int currentDepth = 0);



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
    void indexDirectoryFiles(const QString& rootPath, int currentDepth = 0);



    /*
     * getIndexedItems
     *
     * PURPOSE:
     * - Returns final list of all indexed items
     *
     * USED BY:
     * - SearchEngine (for ranking)
     * - UI layer (for displaying results)
     */
    QVector<IndexedItem> getIndexedItems() const;




};

#endif
