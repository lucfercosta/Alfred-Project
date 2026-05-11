#include "applicationlauncherservice.h"
#include <QProcess>

ApplicationLauncherService::ApplicationLauncherService(QObject* parent)
    : QObject(parent)
{
}

void ApplicationLauncherService::launch(const QString& path, IndexedItemType type)
{
    switch (type)
    {
    case IndexedItemType::Application:
        launchApplication(path);
        break;

    case IndexedItemType::Directory:
    case IndexedItemType::File:
        openFileOrFolder(path);
        break;
    }
}

void ApplicationLauncherService::launchApplication(const QString& path)
{
    int space = path.indexOf(' ');

    QString app = (space == -1) ? path : path.left(space);
    QStringList args =
        (space == -1)
            ? QStringList()
            : path.mid(space + 1).split(' ', Qt::SkipEmptyParts);

    QProcess::startDetached(app, args);
}

void ApplicationLauncherService::openFileOrFolder(const QString& path)
{
    QProcess::startDetached("xdg-open", { path });
}