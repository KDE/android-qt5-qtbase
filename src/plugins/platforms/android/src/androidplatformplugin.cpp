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

#include <qpa/qplatformintegrationplugin.h>
#include <QDebug>
#include "qandroidplatformintegration.h"

QT_BEGIN_NAMESPACE

class QAndroidPlatformIntegrationPlugin : public QPlatformIntegrationPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QPA.QPlatformIntegrationFactoryInterface.5.1" FILE "android.json")
public:
    QPlatformIntegration *create(const QString &key, const QStringList &paramList);
};


QPlatformIntegration* QAndroidPlatformIntegrationPlugin::create(const QString &key, const QStringList &paramList)
{
    Q_UNUSED(paramList);
    qDebug()<<"QAndroidPlatformIntegrationPlugin::create"<<key;
    if (key.toLower() == "android")
        return new QAndroidPlatformIntegration(paramList);
    return 0;
}

QT_END_NAMESPACE
#include "androidplatformplugin.moc"

