#ifndef APPLICATIONLAUNCHERSERVICE_H
#define APPLICATIONLAUNCHERSERVICE_H

#include <QObject>
#include "../data/IndexedItem.h"

class ApplicationLauncherService : public QObject {
    Q_OBJECT

public:
    explicit ApplicationLauncherService(QObject* parent = nullptr);

    void launch(const QString& path, IndexedItemType type);

private:
    void launchApplication(const QString& path);
    void openFileOrFolder(const QString& path);
};

#endif