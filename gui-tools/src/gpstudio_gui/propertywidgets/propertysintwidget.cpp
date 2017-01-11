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

#include "propertysintwidget.h"

#include <QVBoxLayout>
#include <QSlider>
#include <QLabel>
#include <QDebug>

PropertySIntWidget::PropertySIntWidget()
{
}

PropertySIntWidget::~PropertySIntWidget()
{
}

PropertyWidget::Type PropertySIntWidget::type() const
{
    return Field;
}

void PropertySIntWidget::createWidget()
{
    QHBoxLayout *layout = new QHBoxLayout();
    layout->setContentsMargins(0,0,0,0);

    _slider = new QSlider();
    if(_linkedProperty->isFixed() && _linkedProperty->mode()==Property::Run)
        _slider->setEnabled(false);
    _slider->setMinimum(_linkedProperty->min().toInt());
    _slider->setMaximum(_linkedProperty->max().toInt());
    _slider->setSingleStep(_linkedProperty->step().toInt());
    _slider->setOrientation(Qt::Horizontal);
    layout->addWidget(_slider);

    connect(_slider, SIGNAL(valueChanged(int)), this, SLOT(wrapValue()));
    connect(_slider, SIGNAL(valueChanged(int)), _linkedProperty, SLOT(setValue(int)));
    connect(_linkedProperty, SIGNAL(valueChanged(QVariant)), this, SLOT(setValue(QVariant)));

    _label = new QLabel();
    _label->setNum(_linkedProperty->value().toInt());
    connect(_slider, SIGNAL(sliderMoved(int)), _label, SLOT(setNum(int)));
    layout->addWidget(_label);

    setLayout(layout);

    setValue(_linkedProperty->value());
}

void PropertySIntWidget::destroyWidget()
{
}

void PropertySIntWidget::setValue(QVariant value)
{
    if(value.canConvert(QVariant::Int))
    {
        _slider->blockSignals(true);
        _slider->setValue(value.toInt());
        _label->setNum(value.toInt());
        _slider->blockSignals(false);
    }
}

void PropertySIntWidget::wrapValue()
{
    emit valueChanged(_slider->value());
}
