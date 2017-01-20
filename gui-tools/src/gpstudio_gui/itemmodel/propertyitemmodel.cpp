/****************************************************************************
** Copyright (C) 2014-2017 Dream IP
** 
** This file is part of GPStudio.
**
** GPStudio is a free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#include "propertyitemmodel.h"

#include <camera/property.h>

#include <QDebug>
#include <QIcon>
#include <QStringList>

PropertyItemModelNoSorted::PropertyItemModelNoSorted(QObject *parent)
    : QAbstractItemModel(parent)
{
    _rootProperty = NULL;
}

PropertyItemModelNoSorted::PropertyItemModelNoSorted(const Property *property, QObject *parent)
    : QAbstractItemModel(parent)
{
    _rootProperty = NULL;
    setRootProperty(property);
}

QVariant PropertyItemModelNoSorted::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation==Qt::Vertical) return QVariant();
    switch (role)
    {
    case Qt::DisplayRole:
        switch (section)
        {
        case Name:
            return QVariant("Name");
        case Value:
            return QVariant("Value");
        default:
            return QVariant();
        }
        break;
    }
    return QVariant();
}

QModelIndex PropertyItemModelNoSorted::index(int row, int column, const QModelIndex &parent) const
{
    if (!_rootProperty)
        return QModelIndex();

    if (!hasIndex(row, column, parent))
            return QModelIndex();

    const Property *parentItem;

    if (!parent.isValid())
        parentItem = _rootProperty;
    else
        parentItem = static_cast<Property*>(parent.internalPointer());

    if(row < parentItem->subProperties().count())
    {
        const Property *childItem = parentItem->subProperties()[row];
        return createIndex(row, column, (void*)childItem);
    }
    return QModelIndex();
}

QModelIndex PropertyItemModelNoSorted::parent(const QModelIndex &child) const
{
    if (!_rootProperty)
        return QModelIndex();

    if (!child.isValid())
        return QModelIndex();

    Property *childItem = static_cast<Property*>(child.internalPointer());
    Property *parentItem = childItem->parent();

    if (parentItem == _rootProperty)
        return QModelIndex();

    if (!parentItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int PropertyItemModelNoSorted::rowCount(const QModelIndex &parent) const
{
    if (!_rootProperty)
        return 0;

    if (!parent.isValid())
        return _rootProperty->subProperties().count();

    Property *parentItem = static_cast<Property*>(parent.internalPointer());
    return parentItem->subProperties().count();
}

int PropertyItemModelNoSorted::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return ColumnCount;
}

QVariant PropertyItemModelNoSorted::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();

    if (!_rootProperty)
        return QVariant();

    if(_rootProperty->subProperties().count()==0)
        return QVariant();

    Property *property = static_cast<Property*>(index.internalPointer());

    QStringList enumsList;

    switch (role)
    {
    case Qt::DisplayRole:
        switch (index.column())
        {
        case Name:
            return QVariant(property->name());
        case Value:
            return property->value();
        default:
            return QVariant();
        }
    case Qt::EditRole:
        switch (index.column())
        {
        case Name:
            return QVariant(property->name());
        case Value:
            switch (property->type())
            {
            case Property::Group:
            case Property::FlowType:
            case Property::FlowDataType:
            case Property::BlockType:
                return QVariant();
            case Property::Int:
            case Property::SInt:
            case Property::Bool:
            case Property::String:
            case Property::StringType:
            case Property::Clock:
                return property->value();
            case Property::Enum:
                foreach (PropertyEnum *penum, property->enums()) enumsList << penum->name();
                return QVariant(enumsList);
            case Property::Matrix:
                return QVariant();
            }
        default:
            return QVariant();
        }
    case Qt::DecorationRole:
        if(index.column() != Name)
            return QVariant();
        switch (property->type())
        {
        case Property::Group:
        case Property::Matrix:
            return QIcon(":/icons/img/group-type.png");
        case Property::FlowType:
            if(property->propertymap()=="in")
                return QIcon(":/icons/img/flow-in.png");
            else
                return QIcon(":/icons/img/flow-out.png");
        case Property::FlowDataType:
            return QIcon(":/icons/img/type-type.png");
        case Property::BlockType:
            return QIcon(":/icons/img/process-type.png");
        case Property::Int:
        case Property::SInt:
            return QIcon(":/icons/img/int-type.png");
        case Property::Bool:
            return QIcon(":/icons/img/bool-type.png");
        case Property::String:
        case Property::StringType:
            return QIcon(":/icons/img/string-type.png");
        case Property::Clock:
            return QIcon(":/icons/img/clock-type.png");
        case Property::Enum:
            return QIcon(":/icons/img/enum-type.png");
        }
    }
    return QVariant();
}

bool PropertyItemModelNoSorted::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (data(index, role) != value)
    {
        Property *property = static_cast<Property*>(index.internalPointer());

        property->setValue(value);

        emit dataChanged(index, index);
        return true;
    }
    return false;
}

Qt::ItemFlags PropertyItemModelNoSorted::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    if (index.column() == Value)
        return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

void PropertyItemModelNoSorted::updateRoot()
{
    emit layoutAboutToBeChanged();
    emit layoutChanged();
}

const Property *PropertyItemModelNoSorted::rootProperty() const
{
    return _rootProperty;
}

void PropertyItemModelNoSorted::setRootProperty(const Property *rootProperty)
{
    beginResetModel();
    _rootProperty = rootProperty;
    if(_rootProperty)
        connect(_rootProperty, SIGNAL(subPropertyChange()), this, SLOT(updateRoot()));
    endResetModel();
}


// =============== sorted model ====================

PropertyItemModel::PropertyItemModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    _modelProperty = new PropertyItemModelNoSorted(this);
    setSourceModel(_modelProperty);
}

PropertyItemModel::PropertyItemModel(const Property *property, QObject *parent)
    : QSortFilterProxyModel(parent)
{
    _modelProperty = new PropertyItemModelNoSorted(property, this);
    setSourceModel(_modelProperty);
}

const Property *PropertyItemModel::rootProperty() const
{
    return _modelProperty->rootProperty();
}

void PropertyItemModel::setRootProperty(const Property *rootProperty)
{
    _modelProperty->setRootProperty(rootProperty);
}
