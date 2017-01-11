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

#include "propertyenumwidget.h"

#include <QVBoxLayout>
#include <QComboBox>
#include <QDebug>

PropertyEnumWidget::PropertyEnumWidget()
{
}

PropertyEnumWidget::~PropertyEnumWidget()
{
}

PropertyWidget::Type PropertyEnumWidget::type() const
{
    return Field;
}

void PropertyEnumWidget::createWidget()
{
    QVBoxLayout *layout = new QVBoxLayout();
    layout->setContentsMargins(0,0,0,0);

    _comboBox = new QComboBox();
    if(_linkedProperty->isFixed() && _linkedProperty->mode()==Property::Run)
        _comboBox->setEnabled(false);

    foreach (PropertyEnum *propertyEnum, _linkedProperty->enums())
    {
        _comboBox->addItem(propertyEnum->caption(), propertyEnum->name());
    }

    connect(_comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(wrapValue(int)));
    connect(this, SIGNAL(valueChanged(QVariant)), _linkedProperty, SLOT(setValue(QVariant)));
    connect(_linkedProperty, SIGNAL(valueChanged(QVariant)), this, SLOT(setValue(QVariant)));

    layout->addWidget(_comboBox);

    setLayout(layout);

    setValue(_linkedProperty->value());
}

void PropertyEnumWidget::destroyWidget()
{
}

void PropertyEnumWidget::setValue(QVariant value)
{
    int index = _comboBox->findData(value.toString());
    //qDebug()<<"combo"<<value<<index;
    if(index!=-1)
    {
        _comboBox->blockSignals(true);
        _comboBox->setCurrentIndex(index);
        _comboBox->blockSignals(false);
    }
}

void PropertyEnumWidget::wrapValue(int value)
{
    Q_UNUSED(value);
    //qDebug()<<"wrapValue"<<value<<_comboBox->currentText();
    emit(valueChanged(_comboBox->itemData(_comboBox->currentIndex())));
}
