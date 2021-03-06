/***************************************************************************
**
** Copyright (C) 2011 - 2012 Research In Motion
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

#ifndef QQNXROOTWINDOW_H
#define QQNXROOTWINDOW_H

#include <QtCore/QByteArray>
#include <QtCore/QRect>

#include <screen/screen.h>

QT_BEGIN_NAMESPACE

class QQnxScreen;

class QQnxRootWindow
{
public:
    QQnxRootWindow(QQnxScreen *screen);
    ~QQnxRootWindow();

    screen_window_t nativeHandle() const { return m_window; }

    void post() const;
    void flush() const;

    void setRotation(int rotation);

    void resize(const QSize &size);

    QByteArray groupName() const { return m_windowGroupName; }

private:
    void createWindowGroup();

    QQnxScreen *m_screen;
    screen_window_t m_window;
    QByteArray m_windowGroupName;
};

QT_END_NAMESPACE

#endif // QQNXROOTWINDOW_H
