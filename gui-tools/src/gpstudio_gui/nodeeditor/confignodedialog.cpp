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

#include "confignodedialog.h"

#include "lib_parser/lib.h"
#include "model/model_board.h"

#include <QDebug>

#include <QCheckBox>
#include <QRadioButton>
#include <QGroupBox>
#include <QLayout>
#include <QDialogButtonBox>
#include <QSpacerItem>

ConfigNodeDialog::ConfigNodeDialog(QWidget *parent) :
    QDialog(parent)
{
    setupWidgets();
    setWindowTitle(tr("Platform configuration"));
}

ConfigNodeDialog::~ConfigNodeDialog()
{
}

GPNodeProject *ConfigNodeDialog::project() const
{
    return _project;
}

void ConfigNodeDialog::setProject(GPNodeProject *project)
{
    _project = project;

    foreach (BoardLib *board, Lib::getLib().boards())
    {
        _boardComboBox->addItem(board->name());
    }

    if(project->node()->board())
    {
        if(project->node()->board()->name().isEmpty())
            _boardComboBox->setCurrentIndex(0);
        else
            _boardComboBox->setCurrentIndex(_boardComboBox->findText(project->node()->board()->name()));
    }
}

QString ConfigNodeDialog::boardName()
{
    return _boardComboBox->currentText();
}

void ConfigNodeDialog::selectBoard(const QString &boardName)
{
    BoardLib *board = Lib::getLib().board(boardName);
    if(!board)
        return;

    bool rec = false;
    if(_project->node()->board())
        if(_project->node()->board()->name() == boardName)
            rec = true;

    QWidget *widget = new QWidget();
    widget->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

    _iosLayout = new QVBoxLayout();
    _iosLayout->setContentsMargins(10, 10, 10, 10);

    QMapIterator<QString, IOBoardLibGroup> i(board->iosGroups());
    while (i.hasNext())
    {
        i.next();

        QGroupBox *group = new QGroupBox(i.key());

        QLayout *groupLayout = new QVBoxLayout();
        groupLayout->setContentsMargins(10, 10, 10, 10);

        foreach(QString ioName, i.value().ios())
        {
            IOBoardLib *io = board->io(ioName);
            if(io)
            {
                QAbstractButton *checkBox;
                if(io->isOptional())
                {
                    checkBox = new QCheckBox(io->name());
                    groupLayout->addWidget(checkBox);
                }
                else
                {
                    checkBox = new QRadioButton(io->name());
                    groupLayout->addWidget(checkBox);
                }
                if(rec)
                {
                    if(_project->node()->getBlock(ioName) != NULL)
                        checkBox->setChecked(true);
                }
            }
        }

        group->setLayout(groupLayout);
        _iosLayout->addWidget(group);
    }

    widget->setLayout(_iosLayout);
    _iosWidget->setWidget(widget);
}

void ConfigNodeDialog::setupWidgets()
{
    QVBoxLayout *layout = new QVBoxLayout();

    _boardComboBox = new QComboBox();
    connect(_boardComboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(selectBoard(QString)));
    layout->addWidget(_boardComboBox);

    _iosWidget = new QScrollArea();
    layout->addWidget(_iosWidget);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    layout->addWidget(buttonBox);

    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    setLayout(layout);

    setGeometry(100, 100, 300, 400);
}

QStringList ConfigNodeDialog::iosName()
{
    QStringList iosName;
    int i=0;
    QLayoutItem *layoutItem;
    while((layoutItem = _iosLayout->itemAt(i)) != NULL)
    {
        if(layoutItem->widget())
        {
            QGroupBox *groupBox = static_cast<QGroupBox*>(layoutItem->widget());
            if(groupBox)
            {
                int j=0;
                QLayoutItem *layoutItem2;
                while((layoutItem2 = groupBox->layout()->itemAt(j)) != NULL)
                {
                    if(layoutItem2->widget())
                    {
                        QCheckBox *checkBox = dynamic_cast<QCheckBox*>(layoutItem2->widget());
                        if(checkBox)
                            if(checkBox->isChecked())
                                iosName.append(checkBox->text());

                        QRadioButton *radioButton = dynamic_cast<QRadioButton*>(layoutItem2->widget());
                        if(radioButton)
                            if(radioButton->isChecked())
                                iosName.append(radioButton->text());
                    }
                    j++;
                }
            }
        }
        i++;
    }
    return iosName;
}
