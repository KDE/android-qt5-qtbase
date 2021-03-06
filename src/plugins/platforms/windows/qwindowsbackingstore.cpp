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

#include "qwindowsbackingstore.h"
#include "qwindowswindow.h"
#include "qwindowsnativeimage.h"
#include "qwindowscontext.h"

#include <QtGui/QWindow>

#include <QtCore/QDebug>

QT_BEGIN_NAMESPACE

/*!
    \class QWindowsBackingStore
    \brief Backing store for windows.
    \internal
    \ingroup qt-lighthouse-win
*/

QWindowsBackingStore::QWindowsBackingStore(QWindow *window) :
    QPlatformBackingStore(window)
{
     if (QWindowsContext::verboseBackingStore)
         qDebug() << __FUNCTION__ << this << window;
}

QWindowsBackingStore::~QWindowsBackingStore()
{
    if (QWindowsContext::verboseBackingStore)
        qDebug() << __FUNCTION__ << this;
}

QPaintDevice *QWindowsBackingStore::paintDevice()
{
    Q_ASSERT(!m_image.isNull());
    return &m_image->image();
}

void QWindowsBackingStore::flush(QWindow *window, const QRegion &region,
                                        const QPoint &offset)
{
    Q_ASSERT(window);

    const QRect br = region.boundingRect();
    if (QWindowsContext::verboseBackingStore > 1)
        qDebug() << __FUNCTION__ << window << offset << br;
    QWindowsWindow *rw = QWindowsWindow::baseWindowOf(window);

#ifndef Q_OS_WINCE
    if (rw->format().hasAlpha() && (window->windowFlags() & Qt::FramelessWindowHint)) {
        const long wl = GetWindowLong(rw->handle(), GWL_EXSTYLE);
        if ((wl & WS_EX_LAYERED) == 0)
            SetWindowLong(rw->handle(), GWL_EXSTYLE, wl | WS_EX_LAYERED);

        QRect r = window->frameGeometry();
        QPoint frameOffset(window->frameMargins().left(), window->frameMargins().top());
        QRect dirtyRect = br.translated(offset + frameOffset);

        SIZE size = {r.width(), r.height()};
        POINT ptDst = {r.x(), r.y()};
        POINT ptSrc = {0, 0};
        BLENDFUNCTION blend = {AC_SRC_OVER, 0, (BYTE)(255.0 * rw->opacity()), AC_SRC_ALPHA};
        RECT dirty = {dirtyRect.x(), dirtyRect.y(),
            dirtyRect.x() + dirtyRect.width(), dirtyRect.y() + dirtyRect.height()};
        UPDATELAYEREDWINDOWINFO info = {sizeof(info), NULL, &ptDst, &size, m_image->hdc(), &ptSrc, 0, &blend, ULW_ALPHA, &dirty};
        QWindowsContext::user32dll.updateLayeredWindowIndirect(rw->handle(), &info);
    } else {
#endif
        const HDC dc = rw->getDC();
        if (!dc) {
            qErrnoWarning("%s: GetDC failed", __FUNCTION__);
            return;
        }

        if (!BitBlt(dc, br.x(), br.y(), br.width(), br.height(),
                    m_image->hdc(), br.x() + offset.x(), br.y() + offset.y(), SRCCOPY))
            qErrnoWarning("%s: BitBlt failed", __FUNCTION__);
        rw->releaseDC();
#ifndef Q_OS_WINCE
    }
#endif

    // Write image for debug purposes.
    if (QWindowsContext::verboseBackingStore > 2) {
        static int n = 0;
        const QString fileName = QString::fromLatin1("win%1_%2.png").
                arg(rw->winId()).arg(n++);
        m_image->image().save(fileName);
        qDebug() << "Wrote " << m_image->image().size() << fileName;
    }
}

void QWindowsBackingStore::resize(const QSize &size, const QRegion &region)
{
    if (m_image.isNull() || m_image->image().size() != size) {
#ifndef QT_NO_DEBUG_OUTPUT
        if (QWindowsContext::verboseBackingStore) {
            QDebug nsp = qDebug().nospace();
            nsp << __FUNCTION__ << ' ' << rasterWindow()->window()
                 << ' ' << size << ' ' << region;
            if (!m_image.isNull())
                nsp << " from: " << m_image->image().size();
        }
#endif
        m_image.reset(new QWindowsNativeImage(size.width(), size.height(),
                                              QWindowsNativeImage::systemFormat()));
    }
}

Q_GUI_EXPORT void qt_scrollRectInImage(QImage &img, const QRect &rect, const QPoint &offset);

bool QWindowsBackingStore::scroll(const QRegion &area, int dx, int dy)
{
    if (m_image.isNull() || m_image->image().isNull())
        return false;

    const QVector<QRect> rects = area.rects();
    for (int i = 0; i < rects.size(); ++i)
        qt_scrollRectInImage(m_image->image(), rects.at(i), QPoint(dx, dy));

    return true;
}

void QWindowsBackingStore::beginPaint(const QRegion &region)
{
    Q_UNUSED(region);
    if (QWindowsContext::verboseBackingStore > 1)
        qDebug() << __FUNCTION__;
}

QWindowsWindow *QWindowsBackingStore::rasterWindow() const
{
    if (const QWindow *w = window())
        if (QPlatformWindow *pw = w->handle())
            return static_cast<QWindowsWindow *>(pw);
    return 0;
}

HDC QWindowsBackingStore::getDC() const
{
    if (!m_image.isNull())
        return m_image->hdc();
    return 0;
}

QT_END_NAMESPACE
