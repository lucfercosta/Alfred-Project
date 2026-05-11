#include "filesystemindexingworker.h"

/*
 * CONSTRUCTOR
 *
 * PURPOSE:
 * - Initializes the FileSystemIndexingWorker instance
 * - Passes the parent QObject for automatic memory management (Qt ownership system)
 *
 * FLOW:
 * - QObject(parent) ensures proper cleanup in Qt object tree
 */
FileSystemIndexingWorker::FileSystemIndexingWorker(QObject* parent)
    : QObject(parent)
{
}


/*
 * startIndexing
 *
 * PURPOSE:
 * - Public entry point used by the UI or application startup
 * - Triggers the indexing workflow
 *
 * FLOW:
 * - Calls buildIndex() to execute the full indexing process
 */
void FileSystemIndexingWorker::startIndexing()
{
    // Step 1: Perform full system scan and build internal index
    indexer.buildSearchIndex();


    // Step 2: Emit final indexed results to connected listeners
    emit indexingCompleted(indexer.getIndexedItems());
}
