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

#include "qandroidplatformintegration.h"
#include "qandroidinputcontext.h"
#include "qabstracteventdispatcher.h"
#include "androidjnimain.h"
#include <QtGui/private/qpixmap_raster_p.h>
#include <qpa/qwindowsysteminterface.h>
#include <QThread>
#include <qpa/qplatformwindow.h>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCommonStyle>
#include "qandroidplatformservices.h"
#include "qandroidplatformfontdatabase.h"
#include "qandroidplatformclipboard.h"
#include <QtPlatformSupport/private/qgenericunixeventdispatcher_p.h>

#ifdef ANDROID_PLUGIN_OPENGL
#include "qandroideglfsscreen.h"
#include "qandroideglfswindowsurface.h"
#include <private/qpixmapdata_gl_p.h>
#else
#include "qandroidplatformscreen.h"
#include "qandroidplatformwindow.h"
#include <QtPlatformSupport/private/qfbbackingstore_p.h>
#endif

QT_BEGIN_NAMESPACE

int QAndroidPlatformIntegration::m_defaultGeometryWidth=320;
int QAndroidPlatformIntegration::m_defaultGeometryHeight=455;
int QAndroidPlatformIntegration::m_defaultPhysicalSizeWidth=50;
int QAndroidPlatformIntegration::m_defaultPhysicalSizeHeight=71;

void *QAndroidPlatformNativeInterface::nativeResourceForIntegration(const QByteArray &resource)
{
    if (resource=="JavaVM")
        return QtAndroid::javaVM();
    return 0;
}

QAndroidPlatformIntegration::QAndroidPlatformIntegration(const QStringList &paramList)
{
    Q_UNUSED(paramList);
    m_eventDispatcher=createUnixEventDispatcher();
    m_androidPlatformNativeInterface =  new QAndroidPlatformNativeInterface();
    if (qgetenv("QT_USE_ANDROID_NATIVE_STYLE").toInt()
            && qgetenv("NECESSITAS_API_LEVEL").toInt()>1
            && !qgetenv("MINISTRO_ANDROID_STYLE_PATH").isEmpty())
    {
        QApplication::setStyle(new QCommonStyle); // don't remove, it's used to set the default things (fonts, palette, etc)
        QApplication::setStyle("android");
    }
    else
        QApplication::setStyle("plastique");
#ifdef ANDROID_PLUGIN_OPENGL
    qDebug() << "QAndroidPlatformIntegration::QAndroidPlatformIntegration():  creating QAndroidEglFSScreen => Using OpenGL painting";
    m_primaryScreen = new QAndroidEglFSScreen(EGL_DEFAULT_DISPLAY);
#else
    qDebug() << "QAndroidPlatformIntegration::QAndroidPlatformIntegration():  creating QAndroidPlatformScreen => Using Raster (Software) for painting";
    m_primaryScreen = new QAndroidPlatformScreen();
#endif
    screenAdded(m_primaryScreen);

    m_mainThread=QThread::currentThread();
    QtAndroid::setAndroidPlatformIntegration(this);
// FIXME
//    qApp->setInputContext( new QAndroidInputContext() );
    m_androidFDB = new QAndroidPlatformFontDatabase();
    m_androidPlatformServices = new QAndroidPlatformServices();
    m_androidPlatformClipboard = new QAndroidPlatformClipboard();
}

bool QAndroidPlatformIntegration::hasCapability(Capability cap) const
{
    switch (cap) {
        case ThreadedPixmaps: return true;
#ifdef ANDROID_PLUGIN_OPENGL
        case OpenGL: return true;
        case ThreadedOpenGL: return true;
#endif
        default: return QPlatformIntegration::hasCapability(cap);
    }
}

QPlatformBackingStore *QAndroidPlatformIntegration::createPlatformBackingStore(QWindow *window) const
{
#ifdef ANDROID_PLUGIN_OPENGL
        return 0;
//        Q_ASSERT(dynamic_cast<QAndroidEglFSScreen*>(m_primaryScreen) != 0);
//        return new QAndroidEglFSWindowSurface(dynamic_cast<QAndroidEglFSScreen*>(m_primaryScreen), widget);
#else
    return new QFbBackingStore(window);
#endif

}

QPlatformWindow *QAndroidPlatformIntegration::createPlatformWindow(QWindow *window) const
{
#ifdef ANDROID_PLUGIN_OPENGL
    return 0;
//    Q_ASSERT(dynamic_cast<QAndroidEglFSScreen*>(m_primaryScreen) != 0);
//    return new QAndroidEglFSWindow(widget, dynamic_cast<QAndroidEglFSScreen*>(m_primaryScreen));
#else
    return new QAndroidPlatformWindow(window);
#endif
}

QAbstractEventDispatcher *QAndroidPlatformIntegration::guiThreadEventDispatcher() const
{
    return m_eventDispatcher;
}

QAndroidPlatformIntegration::~QAndroidPlatformIntegration()
{
    delete m_androidPlatformNativeInterface;
    delete m_androidFDB;
    QtAndroid::setAndroidPlatformIntegration(NULL);
}

QPlatformFontDatabase *QAndroidPlatformIntegration::fontDatabase() const
{
    return m_androidFDB;
}

#ifndef QT_NO_CLIPBOARD
QPlatformClipboard *QAndroidPlatformIntegration::clipboard() const
{
static QAndroidPlatformClipboard * clipboard = 0;
    if (!clipboard)
        clipboard = new QAndroidPlatformClipboard;
    return clipboard;
}
#endif

QPlatformNativeInterface *QAndroidPlatformIntegration::nativeInterface() const
{
    return m_androidPlatformNativeInterface;
}

QPlatformServices *QAndroidPlatformIntegration::services() const
{
    return m_androidPlatformServices;
}

QPlatformMenu * QAndroidPlatformIntegration::platformMenu()
{
    return 0;
}

void QAndroidPlatformIntegration::setDefaultDisplayMetrics(int gw, int gh, int sw, int sh)
{
    m_defaultGeometryWidth=gw;
    m_defaultGeometryHeight=gh;
    m_defaultPhysicalSizeWidth=sw;
    m_defaultPhysicalSizeHeight=sh;
}

void QAndroidPlatformIntegration::setDefaultDesktopSize(int gw, int gh)
{
    m_defaultGeometryWidth=gw;
    m_defaultGeometryHeight=gh;
}

void QAndroidPlatformIntegration::setDesktopSize(int width, int height)
{
    qDebug()<<"setDesktopSize";
    if (m_primaryScreen)
        QMetaObject::invokeMethod(m_primaryScreen, "setGeometry", Qt::AutoConnection, Q_ARG(QRect, QRect(0,0,width, height)));
    qDebug()<<"setDesktopSize done";
}

void QAndroidPlatformIntegration::setDisplayMetrics(int width, int height)
{
    qDebug()<<"setDisplayMetrics";
    if (m_primaryScreen)
        QMetaObject::invokeMethod(m_primaryScreen, "setPhysicalSize", Qt::AutoConnection, Q_ARG(QSize, QSize(width, height)));
    qDebug()<<"setDisplayMetrics done";
}

void QAndroidPlatformIntegration::pauseApp()
{
    if (QAbstractEventDispatcher::instance(m_mainThread))
        QAbstractEventDispatcher::instance(m_mainThread)->interrupt();
}

void QAndroidPlatformIntegration::resumeApp()
{
    if (QAbstractEventDispatcher::instance(m_mainThread))
        QAbstractEventDispatcher::instance(m_mainThread)->wakeUp();
}

#ifdef ANDROID_PLUGIN_OPENGL
void QAndroidPlatformIntegration::surfaceChanged()
{
    if (m_primaryScreen)
        QMetaObject::invokeMethod(m_primaryScreen, "surfaceChanged", Qt::AutoConnection);
}
#endif

QT_END_NAMESPACE
