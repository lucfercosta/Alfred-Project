#include "SearchBoxResultListRenderer.h"
#include "applicationlauncherwidget.h"
#include "applicationlauncherlayout.h"
#include "../data/IndexedItem.h"

#include <QKeyEvent>
#include <QIcon>
#include <QGraphicsDropShadowEffect>




/*
 * ApplicationLauncherWidget
 *
 * PURPOSE:
 * - Initialize UI components (search box + result list)
 * - Configure visual styling and layout
 * - Connect signals and initialize indexing system
 *
 *   RESPONSIBILITIES:
 * - Capture user input (search queries)
 * - Display ranked search results
 * - Handle keyboard navigation and selection
 * - Trigger execution of selected items
 */
ApplicationLauncherWidget::ApplicationLauncherWidget(QWidget* parent)
    : QWidget(parent)
{
    // Create frameless transparent window (launcher overlay)
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);


    // UI size configuration
    const int searchBoxWidth = 600;
    const int searchBoxHeight = 40;
    const int resultListHeight = 400;
    const int spacing = 10;

    /*
     * searchBox
     *
     * PURPOSE:
     * - Accept user search input
     */
    searchBox = new QLineEdit(this);
    searchBox->setFixedSize(searchBoxWidth, searchBoxHeight);


    // Search box style
    searchBox->setStyleSheet(
        "background-color: rgba(30,30,30,235);"
        "color: white;"
        "font-size: 18px;"
        "border-radius: 6px;"
        "padding: 8px;"
        );

    // Search box shadow
    auto shadow = new QGraphicsDropShadowEffect(searchBox);
    shadow->setBlurRadius(12);
    shadow->setColor(QColor(255, 255, 255, 60));
    shadow->setOffset(0, 0);
    searchBox->setGraphicsEffect(shadow);

    /*
     * resultList
     *
     * PURPOSE:
     * - Displays search results
     */
    resultList = new QListWidget(this);
    resultList->setFixedSize(searchBoxWidth, resultListHeight);
    resultList->setVisible(false);


    // Result list style
    resultList->setStyleSheet(
        "background-color: rgba(40,40,40,235);"
        "color: white;"
        "font-size: 16px;"
        "border-radius: 6px;"
        "selection-background-color: rgba(0,0,0,0);"
        );

    resultList->setItemDelegate(new SearchBoxResultListRenderer(resultList));
    resultList->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    resultList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);


    // Result list shadow
    auto shadowList = new QGraphicsDropShadowEffect(resultList);
    shadowList->setBlurRadius(12);
    shadowList->setColor(QColor(255, 255, 255, 60));
    shadowList->setOffset(0, 0);
    resultList->setGraphicsEffect(shadowList);


    /*
     * centerWidget
     *
     * PURPOSE:
     * - Centers UI elements on screen
     */
    ApplicationLauncherLayout::centerWidget(
        searchBox,
        resultList,
        searchBoxWidth,
        searchBoxHeight,
        resultListHeight,
        spacing
        );


    /*
     * SIGNALS
     *
     * PURPOSE:
     * - Connect UI events to logic functions
     */

    // Connects event for live search
    connect(searchBox, &QLineEdit::textChanged, this, &ApplicationLauncherWidget::handleQueryChanged);

    // Connects event for item execution
    connect(resultList, &QListWidget::itemActivated, this, &ApplicationLauncherWidget::handleItemActivated);

    // Connects event for focus loss
    connect(qApp, &QGuiApplication::applicationStateChanged, this, [this](Qt::ApplicationState state) { if (state != Qt::ApplicationActive) resetAndHide();});

    /*
     * INDEXING SYSTEM INITIALIZATION
     *
     * PURPOSE:
     * - Load filesystem index at startup
     * - Provide searchable dataset for search engine
     */
    fileSystemIndexingWorker = new FileSystemIndexingWorker(this);

    // Connects event for indexing complete signal
    connect(fileSystemIndexingWorker, &FileSystemIndexingWorker::indexingCompleted, this, [this](QVector<IndexedItem> items) {indexedItems = items;});

    // Starts indexing
    fileSystemIndexingWorker->startIndexing();
}

/*
 * ~ApplicationLauncherWidget
 *
 * PURPOSE:
 * - Clean up UI resources
 */
ApplicationLauncherWidget::~ApplicationLauncherWidget(){}


/*
 * handleQueryChanged
 *
 * PURPOSE:
 * - Triggered when user types in search box
 * - Updates result list based on query
 *
 * FLOW:
 * - Clear previous results
 * - Run search engine
 * - Convert results into UI items
 */
void ApplicationLauncherWidget::handleQueryChanged(const QString& text)
{
    resultList->clear();

    // If no user input, keep resultList hidden
    if (text.isEmpty()) {
        resultList->setVisible(false);
        return;
    }

    // Starts searchEngine, normalizing, tokenizing, scoring and returns matching items
    auto matchingItems = searchEngine.findMatchingItems(text, indexedItems, maxResults, itemSelectionFrequency);

    // If no relevant results are found, stores a warning message inside an empty item, adds item into the resultList and set it visible
    if (matchingItems.isEmpty()) {
        auto* item = new QListWidgetItem(" No results found");
        item->setFlags(Qt::NoItemFlags);
        resultList->addItem(item);
        resultList->setVisible(true);
        return;
    }


    for (const IndexedItem& matchingItem : matchingItems) {
        QIcon matchingItemIcon;

        // matchingItemIcon becomes the path from .desktop file
        if (matchingItem.itemType == IndexedItemType::Application)
            matchingItemIcon = QIcon::fromTheme(matchingItem.iconPath);

        // matchingItemIcon becomes default folder icon from OS
        else if (matchingItem.itemType == IndexedItemType::Directory)
            matchingItemIcon = QIcon::fromTheme("folder");

        // matchingItemIcon becomes generic icon from OS
        else
            matchingItemIcon = QIcon::fromTheme("text-x-generic");

        // Creates QList entry with it's Icon and Name for UI
        auto* listItem = new QListWidgetItem(matchingItemIcon, matchingItem.displayName);

        // Stores the item path
        listItem->setData(Qt::UserRole, matchingItem.itemLocation);
        // Stores the item type
        listItem->setData(Qt::UserRole + 1, (int)matchingItem.itemType);
        // Adds item to the resultList UI element
        resultList->addItem(listItem);
    }

    // Shows resultList
    resultList->setVisible(true);
    // Start with first item selected
    resultList->setCurrentRow(0);
}

/*
 * handleItemActivated
 *
 * PURPOSE:
 * - Triggered when user selects an listItem (click or Enter key)
 * - Sends listItem to launcher service for execution
 * - Updates usage frequency for ranking system
 */
void ApplicationLauncherWidget::handleItemActivated(QListWidgetItem* listItem)
{
    if (!listItem) return;

    // Extracts itemPath
    QString itemPath = listItem->data(Qt::UserRole).toString();

    // Extracts itemType
    IndexedItemType itemType = (IndexedItemType)listItem->data(Qt::UserRole + 1).toInt();
    // Increases selection frequency
    itemSelectionFrequency[itemPath]++;
    // Launch the item
    launcherService->launch(itemPath, itemType);
    // Clears searchBox, resultList and completely hide UI
    resetAndHide();
}

/*
 * resetAndHide
 *
 * PURPOSE:
 * - Resets UI state
 * - Clears search and results
 * - Hides launcher window
 */
void ApplicationLauncherWidget::resetAndHide()
{
    searchBox->clear();
    resultList->clear();
    resultList->setVisible(false);
    hide();
}

/*
 * keyPressEvent
 *
 * PURPOSE:
 * - Handles keyboard navigation inside result list
 *
 * CONTROLS:
 * - Down: move selection down
 * - Up: move selection up
 * - Enter: activate item
 * - Escape: close launcher
 */
void ApplicationLauncherWidget::keyPressEvent(QKeyEvent* event)
{
    int currentRow = resultList->currentRow();

    switch (event->key()) {

    case Qt::Key_Down:
        if (currentRow + 1 < resultList->count())
            resultList->setCurrentRow(currentRow + 1);
        break;

    case Qt::Key_Up:
        if (currentRow > 0)
            resultList->setCurrentRow(currentRow - 1);
        break;

    case Qt::Key_Return:
    case Qt::Key_Enter:
        handleItemActivated(resultList->currentItem());
        break;

    case Qt::Key_Escape:
        resetAndHide();
        break;

    default:
        QWidget::keyPressEvent(event);
    }
}
