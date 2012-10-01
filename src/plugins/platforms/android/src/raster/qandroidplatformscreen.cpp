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

#include "qandroidplatformscreen.h"
#include "qandroidplatformintegration.h"
#include "androidjnimain.h"

#include <QDebug>

QAndroidPlatformScreen::QAndroidPlatformScreen():QFbScreen()
{
    mGeometry = QRect(0, 0, QAndroidPlatformIntegration::m_defaultGeometryWidth, QAndroidPlatformIntegration::m_defaultGeometryHeight);
    mFormat = QImage::Format_RGB16;
    mDepth = 16;
    mPhysicalSize.setHeight(QAndroidPlatformIntegration::m_defaultPhysicalSizeHeight);
    mPhysicalSize.setWidth(QAndroidPlatformIntegration::m_defaultPhysicalSizeWidth);
    mGeometry.setTop(0);
    mGeometry.setLeft(0);
    mGeometry.setHeight(QAndroidPlatformIntegration::m_defaultGeometryHeight);
    mGeometry.setWidth(QAndroidPlatformIntegration::m_defaultGeometryWidth);
    initializeCompositor();
    qDebug()<<"QAndroidPlatformScreen::QAndroidPlatformScreen():QFbScreen()";
}

QRegion QAndroidPlatformScreen::doRedraw()
{
    QRegion touched;
    touched = QFbScreen::doRedraw();
    if (touched.isEmpty())
        return touched;
//    QVector<QRect> rects = touched.rects();
//    for (int i = 0; i < rects.size(); i++)
    QtAndroid::flushImage(mGeometry.topLeft(), *mScreenImage, touched.boundingRect());
    return touched;
}
