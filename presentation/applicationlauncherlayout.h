#ifndef LAUNCHERLAYOUT_H
#define LAUNCHERLAYOUT_H

#include <QWidget>

/*
 * ApplicationLauncherLayout
 *
 * PURPOSE:
 * - Handles UI positioning logic for the launcher
 * - Responsible for centering and aligning widgets on screen
 *
 * DESIGN ROLE:
 * - Utility class (no state)
 * - Only provides layout helper functions
 */
class ApplicationLauncherLayout {
public:
    /*
     * centerWidget
     *
     * PURPOSE:
     * - Centers the search box and result list on the screen
     * - Applies vertical spacing between UI components
     * - Ensures consistent launcher positioning across resolutions
     *
     * PARAMETERS:
     * - searchBox: input field widget
     * - resultList: list widget showing search results
     * - searchBoxWidth: width of search input
     * - searchBoxHeight: height of search input
     * - resultListHeight: height of results list
     * - spacing: vertical space between search box and results
     */
    static void centerWidget(QWidget* searchBox, QWidget* resultList, int searchBoxWidth, int searchBoxHeight, int resultListHeight, int spacing);
};

#endif