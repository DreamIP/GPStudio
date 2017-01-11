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

#include "propertygroupwidget.h"

#include <QBoxLayout>
#include <QGroupBox>
#include <QFormLayout>
#include <QLabel>
#include <QDebug>

PropertyGroupWidget::PropertyGroupWidget(bool framed)
    : _framed(framed)
{
}

PropertyGroupWidget::~PropertyGroupWidget()
{
}

PropertyWidget::Type PropertyGroupWidget::type() const
{
    return Group;
}

void PropertyGroupWidget::createWidget()
{
    QFormLayout *layoutPanel = new QFormLayout();
    layoutPanel->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
    layoutPanel->setSpacing(6);

    _subPropertyWidgets.clear();
    foreach (Property *property, _linkedProperty->subProperties())
    {
        PropertyWidget *propertyWidget = PropertyWidget::getWidgetFromProperty(property);
        if(propertyWidget)
        {
            if(propertyWidget->type()==Field)
            {
                QLabel *label = new QLabel(property->caption());
                if(property->isConst())
                {
                    QFont font = label->font();
                    font.setBold(true);
                    label->setFont(font);
                }
                layoutPanel->addRow(label, propertyWidget);
            }
            else
            {
                layoutPanel->setWidget(layoutPanel->count(), QFormLayout::SpanningRole, propertyWidget);
            }
            _subPropertyWidgets.append(propertyWidget);
        }
    }

    if(_framed)
    {
        QLayout *layout = new QVBoxLayout();
        layout->setContentsMargins(0,10,0,0);
        QGroupBox *groupBox = new QGroupBox(_linkedProperty->caption());
        groupBox->setLayout(layoutPanel);
        layout->addWidget(groupBox);
        setLayout(layout);
    }
    else
        setLayout(layoutPanel);
}

void PropertyGroupWidget::destroyWidget()
{
}

void PropertyGroupWidget::setValue(QVariant value)
{
    Q_UNUSED(value)
}
