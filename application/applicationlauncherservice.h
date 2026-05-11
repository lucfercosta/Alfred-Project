#ifndef APPLICATIONLAUNCHERSERVICE_H
#define APPLICATIONLAUNCHERSERVICE_H

#include <QObject>
#include "../data/IndexedItem.h"


/*
 * ApplicationLauncherService
 *
 * This class belongs to the APPLICATION LAYER (logic layer).
 *
 * Responsibility:
 * - Take a selected IndexedItem (from the UI)
 * - Decide how to execute it
 * - Launch applications or open files/folders using the OS
 *
 * It acts as a bridge between:
 * UI (user selection) → System execution (OS commands)
 */
class ApplicationLauncherService : public QObject {
    Q_OBJECT // Enables Qt meta-object features (signals, slots, and runtime type system) for this class

public:
    /*
     * Constructor
     * parent: Qt ownership system (memory management)
     */
    explicit ApplicationLauncherService(QObject* parent = nullptr);



    /*
     * Main entry point for execution
     *
     * Called when user selects an item in the launcher UI.
     *
     * Parameters:
     * - itemPath: path or command to execute
     * - itemType: defines what kind of item it is (Application, File, Directory)
     *
     * Behavior:
     * - Routes the request to the correct handler
     */
    void launch(const QString& itemPath, IndexedItemType itemType);

private:
    /*
     * Launches a Linux application
     *
     * Used when itemType == Application
     *
     * Example:
     * - firefox
     * - /usr/bin/code
     * - flatpak run org.example.App
     */
    void launchApplication(const QString& applicationPath);



    /*
     * Opens a file or directory using the system default handler
     *
     * Used when itemType == File or Directory
     *
     * Example:
     * - opens folder in file manager
     * - opens file in default application
     */
    void openFileOrFolder(const QString& fileOrFolderPath);
};

#endif
