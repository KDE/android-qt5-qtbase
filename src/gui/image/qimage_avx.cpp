/****************************************************************************
**
** Copyright (C) 2012 Intel Corporation
** Contact: http://www.qt-project.org/
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <private/qsimd_p.h>

#ifdef QT_COMPILER_SUPPORTS_AVX

#ifndef __AVX__
#error "AVX not enabled in this file, cannot proceed"
#endif

#define convert_ARGB_to_ARGB_PM_inplace_sse2 convert_ARGB_to_ARGB_PM_inplace_avx
#include "qimage_sse2.cpp"

#define qt_convert_rgb888_to_rgb32_ssse3 qt_convert_rgb888_to_rgb32_avx
#define convert_RGB888_to_RGB32_ssse3 convert_RGB888_to_RGB32_avx
#include "qimage_ssse3.cpp"

#endif
