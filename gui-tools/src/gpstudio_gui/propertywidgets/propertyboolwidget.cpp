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

#include "propertyboolwidget.h"

#include <QBoxLayout>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QDebug>

PropertyBoolWidget::PropertyBoolWidget()
{
}

PropertyBoolWidget::~PropertyBoolWidget()
{
}

PropertyWidget::Type PropertyBoolWidget::type() const
{
    return SimpleField;
}

void PropertyBoolWidget::createWidget()
{
    QBoxLayout *layout = new QVBoxLayout();
    layout->setContentsMargins(0,0,0,0);

    _checkBox = new QCheckBox(_linkedProperty->caption());
    if(_linkedProperty->isFixed() && _linkedProperty->mode()==Property::Run)
        _checkBox->setEnabled(false);
    if(_linkedProperty->isConst())
    {
        QFont font = _checkBox->font();
        font.setBold(true);
        _checkBox->setFont(font);
    }
    connect(_checkBox, SIGNAL(toggled(bool)), _linkedProperty, SLOT(setValue(bool)));
    connect(_checkBox, SIGNAL(toggled(bool)), this, SLOT(wrapValue(bool)));
    connect(_linkedProperty, SIGNAL(valueChanged(QVariant)), this, SLOT(setValue(QVariant)));
    layout->addWidget(_checkBox);

    setLayout(layout);

    setValue(_linkedProperty->value());
}

void PropertyBoolWidget::destroyWidget()
{
}

void PropertyBoolWidget::setValue(QVariant value)
{
    if(value.canConvert(QVariant::Bool))
    {
        _checkBox->blockSignals(true);
        _checkBox->setChecked(value.toBool());
        _checkBox->blockSignals(false);
    }
}

void PropertyBoolWidget::wrapValue(bool value)
{
    emit(valueChanged(value));
}
