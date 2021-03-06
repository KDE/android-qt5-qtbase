/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
**
** This file is part of the test suite of the Qt Toolkit.
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

#include <QtTest/QtTest>

#include <qsignalmapper.h>

class tst_QSignalMapper : public QObject
{
    Q_OBJECT
private slots:
    void mapped();
};

class QtTestObject : public QObject
{
    Q_OBJECT
public slots:
    void myslot(int id);
    void myslot(const QString &str);

signals:
    void mysignal(int);

public:
    void emit_mysignal(int);

    int id;
    QString str;
};

void QtTestObject::myslot(int id)
{
    this->id = id;
}

void QtTestObject::myslot(const QString &str)
{
    this->str = str;
}

void QtTestObject::emit_mysignal(int value)
{
    emit mysignal(value);
}

void tst_QSignalMapper::mapped()
{
    QSignalMapper mapper(0);

    QtTestObject target;
    QtTestObject src1;
    QtTestObject src2;
    QtTestObject src3;

    connect(&src1, SIGNAL(mysignal(int)), &mapper, SLOT(map()));
    connect(&src2, SIGNAL(mysignal(int)), &mapper, SLOT(map()));
    connect(&src3, SIGNAL(mysignal(int)), &mapper, SLOT(map()));

    mapper.setMapping(&src1, 7);
    mapper.setMapping(&src1, 1);
    mapper.setMapping(&src2, 2);
    mapper.setMapping(&src2, "two");
    mapper.setMapping(&src3, "three");

    connect(&mapper, SIGNAL(mapped(int)), &target, SLOT(myslot(int)));
    connect(&mapper, SIGNAL(mapped(const QString &)), &target, SLOT(myslot(const QString &)));

    src1.emit_mysignal(20);
    QVERIFY(target.id == 1);
    QVERIFY(target.str.isEmpty());

    src2.emit_mysignal(20);
    QVERIFY(target.id == 2);
    QVERIFY(target.str == "two");

    src3.emit_mysignal(20);
    QVERIFY(target.id == 2);
    QVERIFY(target.str == "three");
}

QTEST_MAIN(tst_QSignalMapper)
#include "tst_qsignalmapper.moc"
