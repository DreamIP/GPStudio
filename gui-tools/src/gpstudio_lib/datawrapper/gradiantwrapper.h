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

#ifndef GRADIANTWRAPPER_H
#define GRADIANTWRAPPER_H

#include "gpstudio_lib_common.h"

#include "datawrapper.h"

class GPSTUDIO_LIB_EXPORT GradiantWrapper : public DataWrapper
{
public:
    GradiantWrapper();
    ~GradiantWrapper();

    virtual QImage *transform(const FlowPackage &data);

    int nbBins() const;
    void setNbBins(int nbBins);

    int cellSize() const;
    void setCellSize(int cellSize);

    int wimg() const;
    void setWimg(int wimg);

    int himg() const;
    void setHimg(int himg);

private:
    int _nbBins;
    int _cellSize;
    int _wimg;
    int _himg;
};

#endif // GRADIANTWRAPPER_H
