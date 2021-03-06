/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef QINPUTMETHOD_P_H
#define QINPUTMETHOD_P_H

#include <qinputmethod.h>
#include <private/qobject_p.h>
#include <QtCore/QWeakPointer>
#include <QTransform>
#include <qpa/qplatforminputcontext.h>
#include <qpa/qplatformintegration.h>
#include <private/qguiapplication_p.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QInputMethodPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QInputMethod)

public:
    inline QInputMethodPrivate() : testContext(0)
    {}
    QPlatformInputContext *platformInputContext() const
    {
        return testContext ? testContext : QGuiApplicationPrivate::platformIntegration()->inputContext();
    }
    static inline QInputMethodPrivate *get(QInputMethod *inputMethod)
    {
        return inputMethod->d_func();
    }
    inline void setInputItem(QObject *item)
    {
        Q_Q(QInputMethod);

        if (inputItem.data() == item)
            return;

        inputItem = item;
        emit q->inputItemChanged();
    }

    void _q_connectFocusObject();
    void _q_checkFocusObject(QObject *object);
    bool objectAcceptsInputMethod(QObject *object);

    QTransform inputItemTransform;
    QPointer<QObject> inputItem;
    QPlatformInputContext *testContext;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif
