/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Copyright (C) 2012 Klaralvdalens Datakonsult AB, a KDAB Group company, info@kdab.com, author Christoph Schleifenbaum <christoph.schleifenbaum@kdab.com>
** Contact: http://www.qt-project.org/
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#ifndef QPLATFORMSYSTEMTRAYICON_H
#define QPLATFORMSYSTEMTRAYICON_H

#ifndef QT_NO_SYSTEMTRAYICON

#include "QtCore/qobject.h"

QT_BEGIN_NAMESPACE

class QPlatformMenu;
class QIcon;
class QString;
class QRect;

class Q_GUI_EXPORT QPlatformSystemTrayIcon : public QObject
{
    Q_OBJECT
public:
    enum ActivationReason {
        Unknown,
        Context,
        DoubleClick,
        Trigger,
        MiddleClick
    };

    enum MessageIcon { NoIcon, Information, Warning, Critical };

    QPlatformSystemTrayIcon();
    ~QPlatformSystemTrayIcon();

    virtual void init() = 0;
    virtual void cleanup() = 0;
    virtual void updateIcon(const QIcon &icon) = 0;
    virtual void updateToolTip(const QString &tooltip) = 0;
    virtual void updateMenu(QPlatformMenu *menu) = 0;
    virtual QRect geometry() const = 0;
    virtual void showMessage(const QString &msg, const QString &title,
                             const QIcon &icon, MessageIcon iconType, int secs) = 0;

    virtual bool isSystemTrayAvailable() const = 0;
    virtual bool supportsMessages() const = 0;

Q_SIGNALS:
    void activated(QPlatformSystemTrayIcon::ActivationReason reason);
    void messageClicked();
};

QT_END_NAMESPACE

#endif // QT_NO_SYSTEMTRAYICON

#endif // QSYSTEMTRAYICON_P_H
