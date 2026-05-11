#ifndef APPLICATIONLAUNCHERWIDGET_H
#define APPLICATIONLAUNCHERWIDGET_H

#include "../data/IndexedItem.h"
#include "../data/filesystemindexingworker.h"
#include "../application/searchengine.h"
#include "../application/applicationlauncherservice.h"

#include <QWidget>
#include <QLineEdit>
#include <QListWidget>
#include <QMap>
#include <QHotkey>


/*
 * ApplicationLauncherWidget
 *
 * PURPOSE:
 * - Main UI controller of the launcher system
 * - Handles user input, search results, and item selection
 * - Connects UI layer with search engine and execution services
 *
 * ROLE IN ARCHITECTURE:
 * - Presentation Layer (UI)
 * - Central orchestrator of user interaction flow
 */
class ApplicationLauncherWidget : public QWidget {
    Q_OBJECT // Enables Qt meta-object features (signals, slots, and runtime type system) for this class

public:
    /*
     * CONSTRUCTOR
     *
     * PURPOSE:
     * - Initializes the launcher UI and all required components
     * - Sets up search box, result list, indexing, and signals
     */
    explicit ApplicationLauncherWidget(QWidget *parent = nullptr);


    /*
     * DESTRUCTOR
     *
     * PURPOSE:
     * - Cleans up allocated resources (widgets, services, hotkey, etc.)
     */
    ~ApplicationLauncherWidget();


    /*
     * searchBox
     *
     * PURPOSE:
     * - Input field where the user types search queries
     */
    QLineEdit *searchBox;


    /*
     * resultList
     *
     * PURPOSE:
     * - Displays ranked search results dynamically
     */
    QListWidget *resultList;

private:
    /*
     * fileSystemIndexingWorker
     *
     * PURPOSE:
     * - Responsible for scanning the system and building the initial index
     */
    FileSystemIndexingWorker *fileSystemIndexingWorker;


    /*
     * searchEngine
     *
     * PURPOSE:
     * - Handles query processing, scoring, and ranking of results
     */
    SearchEngine searchEngine;

    /*
     * launcherService
     *
     * PURPOSE:
     * - Executes selected items (applications, files, folders)
     */
    ApplicationLauncherService* launcherService;


    /*
     * indexedItems
     *
     * PURPOSE:
     * - Stores all items available for search
     * - Populated by FileSystemIndexingWorker
     */
    QVector<IndexedItem> indexedItems;


    /*
     * itemSelectionFrequency
     *
     * PURPOSE:
     * - Tracks how often each item is selected
     * - Used to boost ranking dynamically (learning system)
     */
    QMap<QString, int> itemSelectionFrequency;
    /*
     * maxResults
     *
     * PURPOSE:
     * - Limits number of results shown in UI
     */
    int maxResults = 6;


    /*
     * hotkey
     *
     * PURPOSE:
     * - Global shortcut used to show/hide launcher window
     */
    QHotkey *hotkey;


    /*
     * resetAndHide
     *
     * PURPOSE:
     * - Resets UI state and hides launcher window
     */
    void resetAndHide();

private slots:
    /*
     * handleQueryChanged
     *
     * PURPOSE:
     * - Triggered whenever user types in search box
     * - Updates search results in real time
     */
    void handleQueryChanged(const QString &text);


    /*
     * handleItemActivated
     *
     * PURPOSE:
     * - Triggered when user selects an item (click or Enter key)
     * - Sends item to launcher service for execution
     */
    void handleItemActivated(QListWidgetItem *item);

protected:
    /*
     * keyPressEvent
     *
     * PURPOSE:
     * - Handles keyboard navigation (up/down/enter/escape)
     * - Provides full keyboard-driven UX like Alfred/Spotlight
     */
    void keyPressEvent(QKeyEvent *event) override;
};

#endif
