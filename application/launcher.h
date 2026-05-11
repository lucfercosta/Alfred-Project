#ifndef LAUNCHER_H                  // Include guard: prevents this header from being included multiple times during compilation
#define LAUNCHER_H                  // Defines the macro so the header content is only processed once

#include "../data/IndexedItem.h"           // Custom Item data model (defines structure for apps/files/folders used in the launcher)


// -------------------------------
// ItemLauncher class - responsible for executing/opening selected items
// -------------------------------
class ItemLauncher {
public:


    // -----------------------------------------
    // Open Path - Opens file/folder externally
    // -----------------------------------------
    static void openFileOrFolder(const QString& path);



    // -------------------------------
    // Launch - launchApplication the selected item (application, file, or folder)
    // -------------------------------
    void launchApplication(const IndexedItem& item);
};

#endif
