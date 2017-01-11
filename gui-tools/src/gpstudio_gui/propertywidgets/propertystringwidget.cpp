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

#include "propertystringwidget.h"

#include <QVBoxLayout>
#include <QLineEdit>

PropertyStringWidget::PropertyStringWidget()
{
}

PropertyStringWidget::~PropertyStringWidget()
{
}

PropertyWidget::Type PropertyStringWidget::type() const
{
    return Field;
}

void PropertyStringWidget::createWidget()
{
    QVBoxLayout *layout = new QVBoxLayout();
    layout->setContentsMargins(0,0,0,0);

    _lineEdit = new QLineEdit();
    if(_linkedProperty->isFixed() && _linkedProperty->mode()==Property::Run)
        _lineEdit->setEnabled(false);

    connect(_lineEdit, SIGNAL(editingFinished()), this, SLOT(wrapValue()));
    connect(this, SIGNAL(valueChanged(QVariant)), _linkedProperty, SLOT(setValue(QVariant)));
    connect(_linkedProperty, SIGNAL(valueChanged(QVariant)), this, SLOT(setValue(QVariant)));

    layout->addWidget(_lineEdit);

    setLayout(layout);

    setValue(_linkedProperty->value());
}

void PropertyStringWidget::destroyWidget()
{
}

void PropertyStringWidget::setValue(QVariant value)
{
    if(value.canConvert(QVariant::String))
    {
        _lineEdit->blockSignals(true);
        _lineEdit->setText(value.toString());
        _lineEdit->blockSignals(false);
    }
}

void PropertyStringWidget::wrapValue()
{
    emit valueChanged(_lineEdit->text());
}
