/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
**
** This file is part of the QtOpenVG module of the Qt Toolkit.
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
#ifndef QPLATFORMCURSOR_H
#define QPLATFORMCURSOR_H

//
//  W A R N I N G
//  -------------
//
// This file is part of the QPA API and is not meant to be used
// in applications. Usage of this API may make your code
// source and binary incompatible with future versions of Qt.
//

#include <QtCore/QList>
#include <QtGui/QImage>
#include <QtGui/QMouseEvent>
#include <QtCore/QWeakPointer>
#include <QtCore/QObject>
#include <qpa/qplatformscreen.h>
#include <QtGui/QCursor>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE


// Cursor graphics management
class Q_GUI_EXPORT QPlatformCursorImage {
public:
    QPlatformCursorImage(const uchar *data, const uchar *mask, int width, int height, int hotX, int hotY)
    { set(data, mask, width, height, hotX, hotY); }
    QImage * image() { return &cursorImage; }
    QPoint hotspot() const { return hot; }
    void set(const uchar *data, const uchar *mask, int width, int height, int hotX, int hotY);
    void set(const QImage &image, int hx, int hy);
    void set(Qt::CursorShape);
private:
    static void createSystemCursor(int id);
    QImage cursorImage;
    QPoint hot;
};

class QPlatformCursor;

class Q_GUI_EXPORT QPlatformCursorPrivate {
public:
    static QList<QPlatformCursor *> getInstances();
};

class Q_GUI_EXPORT QPlatformCursor : public QObject {
public:
    QPlatformCursor();

    // input methods
    virtual void pointerEvent(const QMouseEvent & event) { Q_UNUSED(event); }
    virtual void changeCursor(QCursor * windowCursor, QWindow * window) = 0;
    virtual QPoint pos() const;
    virtual void setPos(const QPoint &pos);

private:
    Q_DECLARE_PRIVATE(QPlatformCursor)
    friend void qt_qpa_set_cursor(QWidget * w, bool force);
    friend class QApplicationPrivate;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QPLATFORMCURSOR_H
