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

#include "qcocoanativeinterface.h"
#include "qcocoaglcontext.h"
#include "qcocoawindow.h"
#include "qcocoamenubar.h"

#include <qbytearray.h>
#include <qwindow.h>
#include <qpa/qplatformwindow.h>
#include "qsurfaceformat.h"
#include <qpa/qplatformopenglcontext.h>
#include "qopenglcontext.h"
#include "qguiapplication.h"
#include <qdebug.h>

#ifndef QT_NO_WIDGETS
#include "qcocoaprintersupport.h"
#include "qprintengine_mac_p.h"
#include <qpa/qplatformprintersupport.h>
#endif

QT_BEGIN_NAMESPACE

QCocoaNativeInterface::QCocoaNativeInterface()
{
}

void *QCocoaNativeInterface::nativeResourceForWindow(const QByteArray &resourceString, QWindow *window)
{
    if (!window->handle()) {
        qWarning("QCocoaNativeInterface::nativeResourceForWindow: Native window has not been created.");
        return 0;
    }

    if (resourceString == "nsopenglcontext") {
        return static_cast<QCocoaWindow *>(window->handle())->currentContext()->nsOpenGLContext();
    } else if (resourceString == "nsview") {
        return static_cast<QCocoaWindow *>(window->handle())->m_contentView;
    } else if (resourceString == "nswindow") {
        return static_cast<QCocoaWindow *>(window->handle())->m_nsWindow;
    }
    return 0;
}

QPlatformPrinterSupport *QCocoaNativeInterface::createPlatformPrinterSupport()
{
#ifndef QT_NO_WIDGETS
    return new QCocoaPrinterSupport();
#else
    qFatal("Printing is not supported when Qt is configured with -no-widgets");
    return 0;
#endif
}

void *QCocoaNativeInterface::NSPrintInfoForPrintEngine(QPrintEngine *printEngine)
{
#ifndef QT_NO_WIDGETS
    QMacPrintEngine *macPrintEngine = static_cast<QMacPrintEngine *>(printEngine);
    return macPrintEngine->d_func()->printInfo;
#else
    qFatal("Printing is not supported when Qt is configured with -no-widgets");
    return 0;
#endif
}

void QCocoaNativeInterface::onAppFocusWindowChanged(QWindow *window)
{
    Q_UNUSED(window);
    QCocoaMenuBar::updateMenuBarImmediately();
}

QT_END_NAMESPACE
