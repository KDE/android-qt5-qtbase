/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
**
** This file is part of the plugins of the Qt Toolkit.
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

#include "qcocoaintegration.h"

#include "qcocoawindow.h"
#include "qcocoabackingstore.h"
#include "qcocoanativeinterface.h"
#include "qcocoamenuloader.h"
#include "qcocoaeventdispatcher.h"
#include "qcocoahelpers.h"
#include "qcocoaapplication.h"
#include "qcocoaapplicationdelegate.h"
#include "qcocoafiledialoghelper.h"
#include "qcocoatheme.h"
#include "qcocoainputcontext.h"
#include "qmacmime.h"

#include <qpa/qplatformaccessibility.h>
#include <QtCore/qcoreapplication.h>

#include <QtPlatformSupport/private/qcoretextfontdatabase_p.h>

QT_BEGIN_NAMESPACE

QCocoaScreen::QCocoaScreen(int screenIndex)
    :QPlatformScreen()
{
    m_screen = [[NSScreen screens] objectAtIndex:screenIndex];
    NSRect frameRect = [m_screen frame];
    m_geometry = QRect(frameRect.origin.x, frameRect.origin.y, frameRect.size.width, frameRect.size.height);
    NSRect visibleRect = [m_screen visibleFrame];
    m_availableGeometry = QRect(visibleRect.origin.x,
                                frameRect.size.height - (visibleRect.origin.y + visibleRect.size.height), // invert y
                                visibleRect.size.width, visibleRect.size.height);

    m_format = QImage::Format_RGB32;

    m_depth = NSBitsPerPixelFromDepth([m_screen depth]);

    const int dpi = 72;
    const qreal inch = 25.4;
    m_physicalSize = QSizeF(m_geometry.size()) * inch / dpi;

    m_cursor = new QCocoaCursor;
};

QCocoaScreen::~QCocoaScreen()
{
    delete m_cursor;
}

extern CGContextRef qt_mac_cg_context(const QPaintDevice *pdev);

QPixmap QCocoaScreen::grabWindow(WId window, int x, int y, int width, int height) const
{
    // TODO window should be handled
    Q_UNUSED(window)

    const int maxDisplays = 128; // 128 displays should be enough for everyone.
    CGDirectDisplayID displays[maxDisplays];
    CGDisplayCount displayCount;
    CGRect cgRect;

    if (width < 0 || height < 0) {
        // get all displays
        cgRect = CGRectInfinite;
    } else {
        cgRect = CGRectMake(x, y, width, height);
    }
    const CGDisplayErr err = CGGetDisplaysWithRect(cgRect, maxDisplays, displays, &displayCount);

    if (err && displayCount == 0)
        return QPixmap();

    // calculate pixmap size
    QSize windowSize(width, height);
    if (width < 0 || height < 0) {
        QRect windowRect;
        for (uint i = 0; i < displayCount; ++i) {
            const CGRect cgRect = CGDisplayBounds(displays[i]);
            QRect qRect(cgRect.origin.x, cgRect.origin.y, cgRect.size.width, cgRect.size.height);
            windowRect = windowRect.united(qRect);
        }
        if (width < 0)
            windowSize.setWidth(windowRect.width());
        if (height < 0)
            windowSize.setHeight(windowRect.height());
    }

    QPixmap windowPixmap(windowSize);
    windowPixmap.fill(Qt::transparent);

    for (uint i = 0; i < displayCount; ++i) {
        const CGRect bounds = CGDisplayBounds(displays[i]);
        int w = (width < 0 ? bounds.size.width : width);
        int h = (height < 0 ? bounds.size.height : height);
        QRect displayRect = QRect(x, y, w, h);
        QCFType<CGImageRef> image = CGDisplayCreateImageForRect(displays[i],
            CGRectMake(displayRect.x(), displayRect.y(), displayRect.width(), displayRect.height()));
        QPixmap pix(w, h);
        pix.fill(Qt::transparent);
        CGRect rect = CGRectMake(0, 0, w, h);
        CGContextRef ctx = qt_mac_cg_context(&pix);
        qt_mac_drawCGImage(ctx, &rect, image);
        CGContextRelease(ctx);

        QPainter painter(&windowPixmap);
        painter.drawPixmap(bounds.origin.x, bounds.origin.y, pix);
    }
    return windowPixmap;
}

QCocoaIntegration::QCocoaIntegration()
    : mFontDb(new QCoreTextFontDatabase())
    , mEventDispatcher(new QCocoaEventDispatcher())
    , mInputContext(new QCocoaInputContext)
    , mAccessibility(new QPlatformAccessibility)
    , mCocoaClipboard(new QCocoaClipboard)
    , mCocoaDrag(new QCocoaDrag)
    , mNativeInterface(new QCocoaNativeInterface)
    , mServices(new QCocoaServices)
{
    QCocoaAutoReleasePool pool;

    qApp->setAttribute(Qt::AA_DontUseNativeMenuBar, false);

    NSApplication *cocoaApplication = [NSApplication sharedApplication];

    if (qEnvironmentVariableIsEmpty("QT_MAC_DISABLE_FOREGROUND_APPLICATION_TRANSFORM")) {
        // Applications launched from plain executables (without an app
        // bundle) are "background" applications that does not take keybaord
        // focus or have a dock icon or task switcher entry. Qt Gui apps generally
        // wants to be foreground applications so change the process type. (But
        // see the function implementation for exceptions.)
        qt_mac_transformProccessToForegroundApplication();

        // Move the application window to front to avoid launching behind the terminal.
        // Ignoring other apps is neccessary (we must ignore the terminal), but makes
        // Qt apps play slightly less nice with other apps when lanching from Finder
        // (See the activateIgnoringOtherApps docs.)
        [cocoaApplication activateIgnoringOtherApps : YES];
    }

    // ### For AA_MacPluginApplication we don't want to load the menu nib.
    // Qt 4 also does not set the application delegate, so that behavior
    // is matched here.
    if (!QCoreApplication::testAttribute(Qt::AA_MacPluginApplication)) {

        // Set app delegate, link to the current delegate (if any)
        QT_MANGLE_NAMESPACE(QCocoaApplicationDelegate) *newDelegate = [QT_MANGLE_NAMESPACE(QCocoaApplicationDelegate) sharedDelegate];
        [newDelegate setReflectionDelegate:[cocoaApplication delegate]];
        [cocoaApplication setDelegate:newDelegate];

        // Load the application menu. This menu contains Preferences, Hide, Quit.
        QT_MANGLE_NAMESPACE(QCocoaMenuLoader) *qtMenuLoader = [[QT_MANGLE_NAMESPACE(QCocoaMenuLoader) alloc] init];
        qt_mac_loadMenuNib(qtMenuLoader);
        [cocoaApplication setMenu:[qtMenuLoader menu]];
        [newDelegate setMenuLoader:qtMenuLoader];
    }

    NSArray *screens = [NSScreen screens];
    for (uint i = 0; i < [screens count]; i++) {
        QCocoaScreen *screen = new QCocoaScreen(i);
        mScreens.append(screen);
        screenAdded(screen);
    }

    QMacPasteboardMime::initializeMimeTypes();
}

QCocoaIntegration::~QCocoaIntegration()
{
    QCocoaAutoReleasePool pool;
    if (!QCoreApplication::testAttribute(Qt::AA_MacPluginApplication)) {
        // remove the apple event handlers installed by QCocoaApplicationDelegate
        QT_MANGLE_NAMESPACE(QCocoaApplicationDelegate) *delegate = [QT_MANGLE_NAMESPACE(QCocoaApplicationDelegate) sharedDelegate];
        [delegate removeAppleEventHandlers];
        // reset the application delegate
        [[NSApplication sharedApplication] setDelegate: 0];
    }

    // Delete the clipboard integration and destroy mime type converters.
    // Deleting the clipboard integration flushes promised pastes using
    // the mime converters - the ordering here is important.
    delete mCocoaClipboard;
    QMacPasteboardMime::destroyMimeTypes();

    // Delete screens in reverse order to avoid crash in case of multiple screens
    while (!mScreens.isEmpty()) {
        delete mScreens.takeLast();
    }
}

bool QCocoaIntegration::hasCapability(QPlatformIntegration::Capability cap) const
{
    switch (cap) {
    case ThreadedPixmaps: return true;
    case OpenGL : return true;
    case ThreadedOpenGL : return true;
    case BufferQueueingOpenGL: return true;
    default: return QPlatformIntegration::hasCapability(cap);
    }
}



QPlatformWindow *QCocoaIntegration::createPlatformWindow(QWindow *window) const
{
    return new QCocoaWindow(window);
}

QPlatformOpenGLContext *QCocoaIntegration::createPlatformOpenGLContext(QOpenGLContext *context) const
{
    return new QCocoaGLContext(context->format(), context->shareHandle());
}

QPlatformBackingStore *QCocoaIntegration::createPlatformBackingStore(QWindow *window) const
{
    return new QCocoaBackingStore(window);
}

QAbstractEventDispatcher *QCocoaIntegration::guiThreadEventDispatcher() const
{
    return mEventDispatcher;
}

QPlatformFontDatabase *QCocoaIntegration::fontDatabase() const
{
    return mFontDb.data();
}

QPlatformNativeInterface *QCocoaIntegration::nativeInterface() const
{
    return mNativeInterface.data();
}

QPlatformInputContext *QCocoaIntegration::inputContext() const
{
    return mInputContext.data();
}

QPlatformAccessibility *QCocoaIntegration::accessibility() const
{
    return mAccessibility.data();
}

QPlatformClipboard *QCocoaIntegration::clipboard() const
{
    return mCocoaClipboard;
}

QPlatformDrag *QCocoaIntegration::drag() const
{
    return mCocoaDrag.data();
}

QStringList QCocoaIntegration::themeNames() const
{
    return QStringList(QLatin1String(QCocoaTheme::name));
}

QPlatformTheme *QCocoaIntegration::createPlatformTheme(const QString &name) const
{
    if (name == QLatin1String(QCocoaTheme::name))
        return new QCocoaTheme;
    return QPlatformIntegration::createPlatformTheme(name);
}

QPlatformServices *QCocoaIntegration::services() const
{
    return mServices.data();
}

QVariant QCocoaIntegration::styleHint(StyleHint hint) const
{
    if (hint == QPlatformIntegration::FontSmoothingGamma)
        return 2.0;
    if (hint == QPlatformIntegration::SynthesizeMouseFromTouchEvents)
        return false;

    return QPlatformIntegration::styleHint(hint);
}

QT_END_NAMESPACE
