#ifndef INDEXEDITEM_H
#define INDEXEDITEM_H

#include<QString>

/*
 * IndexedItemType
 *
 * PURPOSE:
 * - Defines the type of item that can be indexed by the system
 * - Used to differentiate behavior inside the launcher
 *
 * VALUES:
 * - Directory   → system folders
 * - File        → regular files
 * - Application → applications (.desktop / executables)
 */
enum IndexedItemType {
    Directory, File, Application
};
/*
 * IndexedItem
 *
 * PURPOSE:
 * - Represents a searchable entity in the system
 * - Can be an application, file, or directory
 * - Used by the SearchEngine and UI for display and execution
 *
 * ROLE IN SYSTEM:
 * - Data Layer (core data structure of the launcher)
 */
class IndexedItem {

public:
    /*
     * itemType
     *
     * PURPOSE:
     * - Defines the type of the item (Directory, File, Application)
     * - Determines how the item is handled during execution
     */
    IndexedItemType itemType;
    /*
     * displayName
     *
     * PURPOSE:
     * - Name shown in the graphical user interface
     * - Example: "Firefox", "Downloads", "report.pdf"
     */
    QString displayName;
    /*
     * itemLocation
     *
     * PURPOSE:
     * - Full filesystem path
     * - Used to open files, folders, or execute applications
     */
    QString itemLocation;
    /*
     * iconPath
     *
     * PURPOSE:
     * - Path of the icon associated with the item
     * - Used for UI rendering in QListWidget
     */
    QString iconPath;


    IndexedItem() = default;

    /*
     * PARAMETERIZED CONSTRUCTOR
     *
     * PURPOSE:
     * - Creates a fully initialized IndexedItem instance
     *
     * PARAMETERS:
     * - type: item type (app, file, directory)
     * - name: display name
     * - location: system path
     * - icon: optional icon path
     */
    IndexedItem(IndexedItemType type, const QString& name, const QString& location, const QString& icon = "")
        : itemType(type), displayName(name), itemLocation(location), iconPath(icon) {}

};

#endif // INDEXEDITEM_H