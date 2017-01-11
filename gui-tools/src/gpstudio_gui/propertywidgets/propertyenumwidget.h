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

#ifndef PROPERTYENUMWIDGET_H
#define PROPERTYENUMWIDGET_H

#include "gpstudio_gui_common.h"

#include "propertywidget.h"

class QComboBox;

class GPSTUDIO_GUI_EXPORT PropertyEnumWidget : public PropertyWidget
{
    Q_OBJECT
public:
    PropertyEnumWidget();
    virtual ~PropertyEnumWidget();

    Type type() const;

protected:
    virtual void createWidget();
    virtual void destroyWidget();

public slots:
    virtual void setValue(QVariant value);

protected slots:
    virtual void wrapValue(int value);

private:
    QComboBox *_comboBox;
};

#endif // PROPERTYENUMWIDGET_H
