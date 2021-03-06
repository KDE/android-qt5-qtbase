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

#include "qcocoabackingstore.h"
#include "qcocoaautoreleasepool.h"

#include <QtCore/qdebug.h>
#include <QtGui/QPainter>

QT_BEGIN_NAMESPACE

QCocoaBackingStore::QCocoaBackingStore(QWindow *window)
    : QPlatformBackingStore(window)
{
    m_image = new QImage(window->geometry().size(),QImage::Format_ARGB32_Premultiplied);
}

QCocoaBackingStore::~QCocoaBackingStore()
{
    delete m_image;
}

QPaintDevice *QCocoaBackingStore::paintDevice()
{
    return m_image;
}

void QCocoaBackingStore::flush(QWindow *widget, const QRegion &region, const QPoint &offset)
{
    Q_UNUSED(widget);
    Q_UNUSED(offset);
    QCocoaAutoReleasePool pool;

    QRect geo = region.boundingRect();
    NSRect rect = NSMakeRect(geo.x(), geo.y(), geo.width(), geo.height());
    QCocoaWindow *cocoaWindow = static_cast<QCocoaWindow *>(window()->handle());
    if (cocoaWindow) {

#if MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_8
    if (QSysInfo::QSysInfo::MacintoshVersion >= QSysInfo::MV_10_8) {
        // Workaround for malfunctioning displayRect on 10.8 where
        // calling it seems to have no effect. Call setImage like
        // resize() does.
        [cocoaWindow->m_contentView setImage:m_image];
    }
#endif
        [cocoaWindow->m_contentView displayRect:rect];
   }
}

void QCocoaBackingStore::resize(const QSize &size, const QRegion &)
{
    delete m_image;
    m_image = new QImage(size, QImage::Format_ARGB32_Premultiplied);

    QCocoaWindow *cocoaWindow = static_cast<QCocoaWindow *>(window()->handle());
    if (cocoaWindow)
        [static_cast<QNSView *>(cocoaWindow->m_contentView) setImage:m_image];
}

bool QCocoaBackingStore::scroll(const QRegion &area, int dx, int dy)
{
    extern void qt_scrollRectInImage(QImage &img, const QRect &rect, const QPoint &offset);
    QPoint qpoint(dx, dy);
    const QVector<QRect> qrects = area.rects();
    for (int i = 0; i < qrects.count(); ++i) {
        const QRect &qrect = qrects.at(i);
        qt_scrollRectInImage(*m_image, qrect, qpoint);
    }
    return true;
}

QT_END_NAMESPACE
