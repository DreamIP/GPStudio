#include "propertycompleter.h"

#include <QDebug>
#include <QStringList>

PropertyCompleter::PropertyCompleter(QObject *parent, const Property *rootProperty)
    : QCompleter(new PropertyItemModel(rootProperty), parent)
{
}


QStringList PropertyCompleter::splitPath(const QString &path) const
{
    return path.split(".");
}

QString PropertyCompleter::pathFromIndex(const QModelIndex &index) const
{
    QString path;
    path.prepend(model()->data(index).toString());

    QModelIndex parent = index.parent();
    while (parent.isValid())
    {
        path.prepend(model()->data(parent).toString() + ".");
        parent = parent.parent();
    }
    return path;
}
