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

#include "propertymatrixwidget.h"

#include <QGridLayout>
#include <QSpinBox>
#include <QDebug>
#include <QGroupBox>

PropertyMatrixWidget::PropertyMatrixWidget()
{
}

PropertyMatrixWidget::~PropertyMatrixWidget()
{

}

PropertyWidget::Type PropertyMatrixWidget::type() const
{
    return Group;
}

void PropertyMatrixWidget::createWidget()
{
    QLayout *layout = new QVBoxLayout();
    layout->setContentsMargins(0,0,0,0);
    QGroupBox *groupBox = new QGroupBox(_linkedProperty->caption());

    QGridLayout *layoutMatrix = new QGridLayout();
    layoutMatrix->setContentsMargins(0,10,0,0);

    _subPropertyWidgets.clear();
    for(int x=0; x<9; x++)
    {
        int find=0;
        for(int y=0; y<_linkedProperty->value().toInt(); y++)
        {
            Property *property = _linkedProperty->subPropertiesMap()[QString("m%1%2").arg(x).arg(y)];
            if(property)
            {
                PropertyWidget *propertyWidget = PropertyWidget::getWidgetFromProperty(property);
                if(propertyWidget)
                {
                    layoutMatrix->addWidget(propertyWidget,x,y);
                    find++;
                    _subPropertyWidgets.append(propertyWidget);
                }
            }
        }
        if(find==0) break;
    }

    groupBox->setLayout(layoutMatrix);
    layout->addWidget(groupBox);
    setLayout(layout);
}

void PropertyMatrixWidget::destroyWidget()
{

}

void PropertyMatrixWidget::setValue(QVariant value)
{
    Q_UNUSED(value)
}
