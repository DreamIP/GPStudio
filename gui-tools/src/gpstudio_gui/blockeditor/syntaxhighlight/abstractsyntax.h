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

#ifndef ABSTRACTSYNTAX_H
#define ABSTRACTSYNTAX_H

#include "gpstudio_gui_common.h"

#include <QSyntaxHighlighter>

class GPSTUDIO_GUI_EXPORT AbstractSyntax : public QSyntaxHighlighter
{
    Q_OBJECT
public:
    AbstractSyntax(QTextDocument *parent = 0);

    static QStringList extensions() {return QStringList()<<"vhd"<<"vhdl";}

protected:
    void highlightBlock(const QString &text) Q_DECL_OVERRIDE;

protected:
    struct HighlightingRule
    {
        QRegExp pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> highlightingRules;

    struct PartToHighlight
    {
        AbstractSyntax::HighlightingRule rule;
        int index;
        int length;
    };
    friend bool operator<(const AbstractSyntax::PartToHighlight &t1, const AbstractSyntax::PartToHighlight &t2);

    QTextCharFormat keywordFormat;
    QTextCharFormat singleLineCommentFormat;
    QTextCharFormat multiLineCommentFormat;
    QTextCharFormat quotationFormat;
    QTextCharFormat numberFormat;
    QTextCharFormat typesFormat;

    QRegExp commentStartExpression;
    QRegExp commentEndExpression;
};

inline bool operator<(const AbstractSyntax::PartToHighlight &t1, const AbstractSyntax::PartToHighlight &t2)
{
    return t1.index<t2.index;
}

#endif // ABSTRACTSYNTAX_H
