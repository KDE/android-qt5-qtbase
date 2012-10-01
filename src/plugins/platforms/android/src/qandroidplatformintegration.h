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

#ifndef QANDROIDPLATFORMINTERATION_H
#define QANDROIDPLATFORMINTERATION_H

#include <qpa/qplatformintegration.h>
#include <qpa/qplatformmenu.h>
#include <qpa/qplatformnativeinterface.h>
#include <QtWidgets/QAction>

#include <jni.h>
#include <qandroidplatformscreen.h>

QT_BEGIN_NAMESPACE

class QDesktopWidget;
class QAndroidPlatformServices;

class QAndroidPlatformNativeInterface : public QPlatformNativeInterface
{
public:
    virtual void *nativeResourceForIntegration(const QByteArray &resource);
};

class QAndroidPlatformIntegration : public QPlatformIntegration/*, public QPlatformMenu*/
{
    friend class QAndroidPlatformScreen;
    friend class QAndroidEglFSScreen;

public:
    QAndroidPlatformIntegration(const QStringList &paramList);
    ~QAndroidPlatformIntegration();

    bool hasCapability(QPlatformIntegration::Capability cap) const;

    QPlatformBackingStore *createPlatformBackingStore(QWindow *window) const;
    QPlatformWindow *createPlatformWindow(QWindow *window) const;
    QAbstractEventDispatcher *guiThreadEventDispatcher() const;

    QAndroidPlatformScreen * getPrimaryScreen(){return m_primaryScreen;}
    bool isVirtualDesktop() { return true; }
    virtual void setDesktopSize(int width, int height);
    virtual void setDisplayMetrics(int width, int height);
    QPlatformFontDatabase *fontDatabase() const;
#ifndef QT_NO_CLIPBOARD
    virtual QPlatformClipboard *clipboard() const;
#endif
    virtual QPlatformNativeInterface *nativeInterface() const;
    virtual QPlatformServices *services() const;

    virtual QPlatformMenu * platformMenu();

    void pauseApp();
    void resumeApp();
    static void setDefaultDisplayMetrics(int gw, int gh, int sw, int sh);
    static void setDefaultDesktopSize(int gw, int gh);

private:
    QThread * m_mainThread;
    QAndroidPlatformScreen *m_primaryScreen;
    static int m_defaultGeometryWidth,m_defaultGeometryHeight,m_defaultPhysicalSizeWidth,m_defaultPhysicalSizeHeight;
    QAbstractEventDispatcher *m_eventDispatcher;
    QPlatformFontDatabase *m_androidFDB;
    QImage * mFbScreenImage;
    QPainter *compositePainter;
    QAndroidPlatformNativeInterface *m_androidPlatformNativeInterface;
    QAndroidPlatformServices *m_androidPlatformServices;
    QPlatformClipboard *m_androidPlatformClipboard;

#ifdef ANDROID_PLUGIN_OPENGL
public:
    void surfaceChanged();
#endif

};

QT_END_NAMESPACE

#endif
