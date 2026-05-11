#include "applicationlauncherlayout.h"
#include <QGuiApplication>
#include <QScreen>

/*
 * centerWidget
 *
 * PURPOSE:
 * - Centers the launcher UI elements on the primary screen
 * - Positions search box and result list vertically aligned
 *
 * HOW IT WORKS:
 * - Gets screen resolution
 * - Calculates centerWidget position based on widget sizes
 * - Moves widgets to computed coordinates
 */
void ApplicationLauncherLayout::centerWidget(QWidget* searchBox, QWidget* resultList, int searchBoxWidth, int searchBoxHeight, int resultListHeight, int spacing)
{
    /*
     * Get primary screen
     * PURPOSE:
     * - Retrieve main display geometry for positioning UI
     */
    QScreen* screen = QGuiApplication::primaryScreen();


    /*
     * Screen geometry
     * PURPOSE:
     * - Contains width and height of available screen area
     */
    QRect geom = screen->geometry();


    /*
     * centerX calculation
     * PURPOSE:
     * - Horizontally centers the widgets on screen
     */
    int centerX = (geom.width() - searchBoxWidth) / 2;


    /*
     * centerY calculation
     * PURPOSE:
     * - Vertically centers both widgets as a group
     * - Accounts for search box + spacing + result list height
     */
    int centerY = (geom.height() - (searchBoxHeight + spacing + resultListHeight)) / 2;


    /*
     * Position search box
     * PURPOSE:
     * - Place input field at computed center position
     */
    searchBox->move(centerX, centerY);


    /*
     * Position result list
     * PURPOSE:
     * - Place results directly below search box with spacing
     */
    resultList->move(centerX, centerY + searchBoxHeight + spacing);
}