/******************************************************************************
* Copyright 2012  BogDan Vatra <bogdan@kde.org>                               *
*                                                                             *
* This library is free software; you can redistribute it and/or               *
* modify it under the terms of the GNU Lesser General Public                  *
* License as published by the Free Software Foundation; either                *
* version 2.1 of the License, or (at your option) version 3, or any           *
* later version accepted by the membership of KDE e.V. (or its                *
* successor approved by the membership of KDE e.V.), which shall              *
* act as a proxy defined in Section 6 of version 3 of the license.            *
*                                                                             *
* This library is distributed in the hope that it will be useful,             *
* but WITHOUT ANY WARRANTY; without even the implied warranty of              *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU           *
* Lesser General Public License for more details.                             *
*                                                                             *
* You should have received a copy of the GNU Lesser General Public            *
* License along with this library. If not, see <http://www.gnu.org/licenses/>.*
******************************************************************************/

#ifndef QATOMIC_ANDROID_H
#define QATOMIC_ANDROID_H

#if defined(QT_USE_FAST_ATOMICS) || defined(QT_BUILDING_QT)
# if defined(Q_PROCESSOR_ARM_V7)
#  include "QtCore/qatomic_armv7.h"
# elif defined(Q_PROCESSOR_ARM_V6)
#  include "QtCore/qatomic_armv6.h"
# elif defined(Q_PROCESSOR_ARM_V5)
#  include "QtCore/qatomic_armv5.h"
# endif
#else
# if /*defined(Q_COMPILER_ATOMICS) && defined(Q_COMPILER_CONSTEXPR)
#  include <QtCore/qatomic_cxx11.h>
# elif */defined(Q_CC_GNU)
#  include <QtCore/qatomic_gcc.h>
# else
#  error "Unknown atomics"
# endif
#endif

#endif // QATOMIC_ANDROID_H
