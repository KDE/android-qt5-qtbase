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

#ifndef QFBBACKINGSTORE_P_H
#define QFBBACKINGSTORE_P_H

#include <qpa/qplatformbackingstore.h>

QT_BEGIN_NAMESPACE

class QFbScreen;
class QFbWindow;
class QWindow;

class QFbBackingStore : public QPlatformBackingStore
{
public:
    QFbBackingStore(QWindow *window);
    ~QFbBackingStore();

    virtual QPaintDevice *paintDevice() { return &mImage; }
    virtual void flush(QWindow *window, const QRegion &region, const QPoint &offset);

    virtual void beginPaint(const QRegion &region);
    virtual void endPaint();

    virtual void resize(const QSize &size, const QRegion &region);

    const QImage image() { return mImage; }

protected:
    friend class QFbWindow;

    QImage mImage;
};

QT_END_NAMESPACE

#endif // QFBBACKINGSTORE_P_H

