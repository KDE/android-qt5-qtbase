/****************************************************************************
**
** Copyright (C) 2012 BogDan Vatra <bogdan@kde.org>
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qandroidplatformintegration.h"
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
    m_primaryScreen->setPhysicalSize(QSize(m_defaultPhysicalSizeWidth, m_defaultPhysicalSizeHeight));
    m_primaryScreen->setGeometry(QRect(0,0, m_defaultGeometryWidth, m_defaultGeometryHeight));

    m_mainThread=QThread::currentThread();
    QtAndroid::setAndroidPlatformIntegration(this);
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

QPlatformInputContext *QAndroidPlatformIntegration::inputContext() const
{
    return &m_platformInputContext;
}

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
