#ifndef SEARCHBOXRESULTLISTRENDERER_H
#define SEARCHBOXRESULTLISTRENDERER_H

#include <QStyledItemDelegate>
#include <QPainter>
#include <IndexedItem.h>

/*
 * SearchBoxResultListRenderer
 *
 * PURPOSE:
 * - Responsible for custom rendering of each search result item
 * - Defines the visual style of the launcher (Spotlight/Alfred-like UI)
 *
 * ROLE IN SYSTEM:
 * - Presentation Layer (UI rendering component)
 */
class SearchBoxResultListRenderer : public QStyledItemDelegate {
public:
    /*
     * INHERITED CONSTRUCTOR
     *
     * PURPOSE:
     * - Uses base class constructors from QStyledItemDelegate
     * - Required for proper Qt item delegate integration
     */
    using QStyledItemDelegate::QStyledItemDelegate;


    /*
     * paint
     *
     * PURPOSE:
     * - Main function responsible for drawing each item in the list
     * - Controls full visual representation (background, icon, text)
     *
     * PARAMETERS:
     * - painter: object used for all drawing operations
     * - option: provides visual state (selected, hover, etc.)
     * - index: contains the data for the item being rendered
     */
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override
    {
        // Save painter state before custom drawing
        painter->save();

        // Rectangle representing the item area
        QRect r = option.rect;

        // Internal spacing between UI elements
        int padding = 6;


        /*
         * SELECTION BACKGROUND
         *
         * PURPOSE:
         * - Draws a highlight background when the item is selected
         */
        if (option.state & QStyle::State_Selected)
        {
            // smooth edges
            painter->setRenderHint(QPainter::Antialiasing);
            // blue selection color
            painter->setBrush(QColor(100, 100, 255, 180));
            // no border
            painter->setPen(Qt::NoPen);
            // rounded background
            painter->drawRoundedRect(r, 6, 6);
        }



        /*
         * ITEM DATA EXTRACTION
         */

        // Main displayed text (e.g., Firefox, Downloads)
        QString itemName = index.data(Qt::DisplayRole).toString();

        // Item type stored in custom role
        int type = index.data(Qt::UserRole + 1).toInt();

        QString itemType;

        // Convert enum type into readable string
        switch ((IndexedItemType)type) {
        case IndexedItemType::Application:
            itemType = "Application";
            break;
        case IndexedItemType::Directory:
            itemType = "Directory";
            break;
        default:
            itemType = "File";
            break;
        }
        // icon
        QIcon itemIcon = qvariant_cast<QIcon>(index.data(Qt::DecorationRole));



        /*
         * ICON RENDERING
         *
         * PURPOSE:
         * - Draws the item icon on the left side
         */
        QRect iconRect(r.left() + padding, r.top() + padding, 24, 24);
        // draw icon
        itemIcon.paint(painter, iconRect);



        /*
         * TEXT AREA CALCULATION
         *
         * PURPOSE:
         * - Defines where text should be rendered
         */
        int textLeft = iconRect.right() + padding;
        int textWidth = r.right() - textLeft - padding;



        /*
         * MAIN TEXT (ITEM NAME)
         *
         * PURPOSE:
         * - Displays primary label of the item
         */
        painter->setPen(option.palette.text().color()); // Default text color
        painter->setFont(option.font); // Default font
        painter->drawText(QRect(textLeft, r.top() + padding, textWidth, 20), Qt::AlignLeft | Qt::AlignVCenter, itemName);



        /*
         * SECONDARY TEXT (ITEM TYPE)
         *
         * PURPOSE:
         * - Displays item category (Application / File / Directory)
         */

        QFont pathFont = option.font; // Current font size

        qreal size = option.font.pointSizeF(); // Current font size

        if (size <= 0) size = 12; // Fallback size

        pathFont.setPointSizeF(size * 0.8); // Smaller font for secondary text
        painter->setFont(pathFont);

        painter->setPen(Qt::lightGray); // Secondary text color

        painter->drawText(QRect(textLeft, r.top() + padding + 20, textWidth, 16), Qt::AlignLeft | Qt::AlignVCenter, itemType); // Draw ItemType text

        painter->restore(); // Restore painter state after custom drawing
    }

    /*
     * sizeHint
     *
     * PURPOSE:
     * - Defines fixed height for each list item
     * - Controls visual density of the launcher UI
     */
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override {
        return QSize(QStyledItemDelegate::sizeHint(option, index).width(), 50);
    }
};

#endif // SEARCHBOXRESULTLISTRENDERER_H
