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

#ifndef PROPERTYINTWIDGET_H
#define PROPERTYINTWIDGET_H

#include "gpstudio_gui_common.h"

#include "propertywidget.h"

class QSpinBox;

class GPSTUDIO_GUI_EXPORT PropertyIntWidget : public PropertyWidget
{
    Q_OBJECT
public:
    PropertyIntWidget();
    virtual ~PropertyIntWidget();

    Type type() const;

protected:
    virtual void createWidget();
    virtual void destroyWidget();

public slots:
    virtual void setValue(QVariant value);

protected slots:
    virtual void wrapValue();

private:
    QSpinBox *_spinBox;
};

#endif // PROPERTYINTWIDGET_H
