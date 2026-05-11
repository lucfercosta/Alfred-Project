#ifndef FILESYSTEMINDEXINGWORKER_H
#define FILESYSTEMINDEXINGWORKER_H

#include <QObject>
#include <QVector>
#include "filesystemindexer.h"

/*
 * FileSystemIndexingWorker
 *
 * PURPOSE:
 * - Acts as a simple wrapper around FileSystemIndexer
 * - Executes indexing synchronously (blocking call)
 * - Emits results when finished
 *
 * NOTE:
 * - No threading is used in this version
 */
class FileSystemIndexingWorker : public QObject {
    Q_OBJECT // Enables Qt meta-object features (signals, slots, and runtime type system) for this class

public:
    explicit FileSystemIndexingWorker(QObject* parent = nullptr);

    /*
     * startIndexing
     *
     * PURPOSE:
     * - Starts full system indexing synchronously
     */
    void startIndexing();

signals:
    /*
     * indexingCompleted
     *
     * PURPOSE:
     * - Emits final indexed results after completion
     */
    void indexingCompleted(QVector<IndexedItem> indexedItemsList);

private:
    FileSystemIndexer indexer;
};

#endif