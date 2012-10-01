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

#include "qandroidplatformservices.h"
#include <QUrl>
#include <QDir>
#include <QDebug>

QAndroidPlatformServices::QAndroidPlatformServices()
{
    JNIEnv* env;
    if (QtAndroid::javaVM()->AttachCurrentThread(&env, NULL)<0)
    {
        qCritical()<<"AttachCurrentThread failed";
        return;
    }
    m_openURIMethodID=env->GetStaticMethodID(QtAndroid::applicationClass(), "openURL", "(Ljava/lang/String;)V");
}

bool QAndroidPlatformServices::openUrl ( const QUrl & url )
{
    JNIEnv* env;
    if (QtAndroid::javaVM()->AttachCurrentThread(&env, NULL)<0)
    {
        qCritical()<<"AttachCurrentThread failed";
        return false;
    }
    jstring string = env->NewString((const jchar*)url.toString().constData(), url.toString().length());
    env->CallStaticVoidMethod(QtAndroid::applicationClass(), m_openURIMethodID,string);
    env->DeleteLocalRef(string);
    return true;
}

bool QAndroidPlatformServices::openDocument(const QUrl &url)
{
    return openUrl(url);
}

QByteArray QAndroidPlatformServices::desktopEnvironment() const
{
    return QByteArray("Android");
}
