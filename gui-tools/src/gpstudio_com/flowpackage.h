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

#ifndef FLOWPACKAGE_H
#define FLOWPACKAGE_H

#include <QByteArray>
#include <QList>
#include <QImage>
#include <QMetaType>

#include "gpstudio_com_common.h"

class GPSTUDIO_COM_EXPORT FlowPackage
{
public:
    FlowPackage(const QByteArray &data=QByteArray());
    FlowPackage(const FlowPackage &other);
    const FlowPackage &operator= (const FlowPackage &other);

    enum ImageMode {ImageModeGray, ImageModeColor};
    FlowPackage(const QImage &image, const int bitCount=8, const ImageMode imageMode=ImageModeGray);

    const QByteArray &data() const;
    QByteArray getPart(const int size);

    bool empty() const;
    void clear();

    void appendData(const QByteArray &data);

    QImage toImage(const int width, const int height, const int dataSize) const;
    QImage toImage(const QSize size, const int dataSize) const;

    bool isNewPack() const;

private:
    QByteArray _data;
    bool _newPack;
};

Q_DECLARE_METATYPE(FlowPackage)

#endif // FLOWPACKAGE_H
