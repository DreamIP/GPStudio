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

#ifndef GPSTUDIO_LIB_COMMON_H
#define GPSTUDIO_LIB_COMMON_H

#include <Qt>

#if defined(GPSTUDIO_LIB_EXPORT_LIB)
#  define GPSTUDIO_LIB_EXPORT Q_DECL_EXPORT
#else
#  define GPSTUDIO_LIB_EXPORT Q_DECL_IMPORT
#endif

#endif // GPSTUDIO_LIB_COMMON_H

