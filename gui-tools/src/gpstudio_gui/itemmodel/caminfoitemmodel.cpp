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

#include "caminfoitemmodel.h"

#include "cameracom.h"

CamInfoItemModel::CamInfoItemModel(QObject *parent) :
    QAbstractItemModel(parent)
{
}

int CamInfoItemModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return ColumnCount;
}

QVariant CamInfoItemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation==Qt::Vertical) return QVariant();
    switch (role)
    {
    case Qt::DisplayRole:
        switch (section)
        {
        case Name:
            return QVariant("Name");
        case Driver:
            return QVariant("Driver");
        case Adress:
            return QVariant("Adress");
        default:
            return QVariant();
        }
    break;
    }
    return QVariant();
}

QVariant CamInfoItemModel::data(const QModelIndex &index, int role) const
{
    if(index.row()>=_usbList.count()) return QVariant();

    const CameraInfo &cameraInfo = _usbList[index.row()];

    switch (role)
    {
    case Qt::DisplayRole:
        switch (index.column())
        {
        case Name:
            return QVariant(cameraInfo.name());
        case Driver:
            return QVariant(cameraInfo.driverType());
        case Adress:
            return QVariant(cameraInfo.addr());
        default:
            return QVariant();
        }
    case Qt::UserRole:
        return index.row();
    }
    return QVariant();
}

QModelIndex CamInfoItemModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return createIndex(row, column);
}

QModelIndex CamInfoItemModel::parent(const QModelIndex &child) const
{
    Q_UNUSED(child);
    return QModelIndex();
}

int CamInfoItemModel::rowCount(const QModelIndex &parent) const
{
    if(!parent.isValid()) return _usbList.count();
    return 0;
}

void CamInfoItemModel::refreshCams()
{
    emit layoutAboutToBeChanged();

    _usbList.clear();
    foreach(CameraInfo info, CameraCom::avaibleCams())
    {
        _usbList.append(info);
    }

    emit layoutChanged();
}

const QList<CameraInfo> &CamInfoItemModel::usbList() const
{
    return _usbList;
}
