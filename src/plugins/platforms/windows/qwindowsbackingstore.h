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

#ifndef QWINDOWSBACKINGSTORE_H
#define QWINDOWSBACKINGSTORE_H

#include "qtwindows_additional.h"

#include <qpa/qplatformbackingstore.h>
#include <QtCore/QScopedPointer>

QT_BEGIN_NAMESPACE

class QWindowsWindow;
class QWindowsNativeImage;

class QWindowsBackingStore : public QPlatformBackingStore
{
    Q_DISABLE_COPY(QWindowsBackingStore)
public:
    QWindowsBackingStore(QWindow *window);
    ~QWindowsBackingStore();

    virtual QPaintDevice *paintDevice();
    virtual void flush(QWindow *window, const QRegion &region, const QPoint &offset);
    virtual void resize(const QSize &size, const QRegion &r);
    virtual bool scroll(const QRegion &area, int dx, int dy);
    virtual void beginPaint(const QRegion &);

    HDC getDC() const;

private:
    QWindowsWindow *rasterWindow() const;

    QScopedPointer<QWindowsNativeImage> m_image;
};

QT_END_NAMESPACE

#endif // QWINDOWSBACKINGSTORE_H
