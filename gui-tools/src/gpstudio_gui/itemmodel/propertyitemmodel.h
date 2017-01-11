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

#ifndef PROPERTYITEMMODEL_H
#define PROPERTYITEMMODEL_H

#include "gpstudio_gui_common.h"

#include <QAbstractItemModel>
#include <QSortFilterProxyModel>

class Property;

class GPSTUDIO_GUI_EXPORT PropertyItemModelNoSorted : public QAbstractItemModel
{
    Q_OBJECT

public:
    PropertyItemModelNoSorted(QObject *parent = 0);
    explicit PropertyItemModelNoSorted(const Property *property, QObject *parent = 0);

    enum Column {
        Name,
        Value,
        ColumnCount
    };

    const Property *rootProperty() const;
    void setRootProperty(const Property *rootProperty);

    // QAbstractItemModel interface
public:
    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    // Basic functionality:
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &child) const;

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    // Editable:
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    Qt::ItemFlags flags(const QModelIndex& index) const;

private:
    const Property *_rootProperty;

protected slots:
    void updateRoot();
};

class GPSTUDIO_GUI_EXPORT PropertyItemModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    PropertyItemModel(QObject *parent = 0);
    explicit PropertyItemModel(const Property *property, QObject *parent = 0);

    const Property *rootProperty() const;
    void setRootProperty(const Property *rootProperty);

private:
    PropertyItemModelNoSorted *_modelProperty;
};

#endif // PROPERTYITEMMODEL_H
