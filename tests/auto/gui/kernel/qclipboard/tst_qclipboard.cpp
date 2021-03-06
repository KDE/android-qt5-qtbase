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
#include <QtCore/QDebug>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtGui/QGuiApplication>
#include <QtGui/QClipboard>
#ifdef Q_OS_MAC
#include <Carbon/Carbon.h>
#endif

class tst_QClipboard : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void copy_exit_paste();
    void capabilityFunctions();
    void modes();
    void testSignals();
    void setMimeData();
    void clearBeforeSetText();

private:
    bool nativeClipboardWorking();
};

void tst_QClipboard::init()
{
    const QString testdataDir = QFileInfo(QFINDTESTDATA("copier")).absolutePath();
    QVERIFY2(QDir::setCurrent(testdataDir), qPrintable("Could not chdir to " + testdataDir));
}

bool tst_QClipboard::nativeClipboardWorking()
{
#ifdef Q_OS_MAC
    PasteboardRef pasteboard;
    OSStatus status = PasteboardCreate(0, &pasteboard);
    if (status == noErr)
        CFRelease(pasteboard);
    return status == noErr;
#endif
    return true;
}

Q_DECLARE_METATYPE(QClipboard::Mode)

/*
    Tests that the capability functions are implemented on all
    platforms.
*/
void tst_QClipboard::capabilityFunctions()
{
    QClipboard * const clipboard =  QGuiApplication::clipboard();

    clipboard->supportsSelection();
    clipboard->supportsFindBuffer();
    clipboard->ownsSelection();
    clipboard->ownsClipboard();
    clipboard->ownsFindBuffer();
}

/*
    Test that text inserted into the clipboard in different modes is
    kept separate.
*/
void tst_QClipboard::modes()
{
    QClipboard * const clipboard =  QGuiApplication::clipboard();

    if (!nativeClipboardWorking())
        QSKIP("Native clipboard not working in this setup");

    const QString defaultMode = "default mode text;";
    clipboard->setText(defaultMode);
    QCOMPARE(clipboard->text(), defaultMode);

    if (clipboard->supportsSelection()) {
        const QString selectionMode = "selection mode text";
        clipboard->setText(selectionMode, QClipboard::Selection);
        QCOMPARE(clipboard->text(QClipboard::Selection), selectionMode);
        QCOMPARE(clipboard->text(), defaultMode);
    }

    if (clipboard->supportsFindBuffer()) {
        const QString searchMode = "find mode text";
        clipboard->setText(searchMode, QClipboard::FindBuffer);
        QCOMPARE(clipboard->text(QClipboard::FindBuffer), searchMode);
        QCOMPARE(clipboard->text(), defaultMode);
    }
}

/*
    Test that the appropriate signals are emitted when the clipboard
    contents is changed by calling the qt functions.
*/
void tst_QClipboard::testSignals()
{
    qRegisterMetaType<QClipboard::Mode>("QClipboard::Mode");

    if (!nativeClipboardWorking())
        QSKIP("Native clipboard not working in this setup");

    QClipboard * const clipboard =  QGuiApplication::clipboard();

    QSignalSpy changedSpy(clipboard, SIGNAL(changed(QClipboard::Mode)));
    QSignalSpy dataChangedSpy(clipboard, SIGNAL(dataChanged()));
    QSignalSpy searchChangedSpy(clipboard, SIGNAL(findBufferChanged()));
    QSignalSpy selectionChangedSpy(clipboard, SIGNAL(selectionChanged()));

    const QString text = "clipboard text;";

    // Test the default mode signal.
    clipboard->setText(text);
    QCOMPARE(dataChangedSpy.count(), 1);
    QCOMPARE(searchChangedSpy.count(), 0);
    QCOMPARE(selectionChangedSpy.count(), 0);
    QCOMPARE(changedSpy.count(), 1);
    QCOMPARE(changedSpy.at(0).count(), 1);
    QCOMPARE(qvariant_cast<QClipboard::Mode>(changedSpy.at(0).at(0)), QClipboard::Clipboard);

    changedSpy.clear();

    // Test the selection mode signal.
    if (clipboard->supportsSelection()) {
        clipboard->setText(text, QClipboard::Selection);
        QCOMPARE(selectionChangedSpy.count(), 1);
        QCOMPARE(changedSpy.count(), 1);
        QCOMPARE(changedSpy.at(0).count(), 1);
        QCOMPARE(qvariant_cast<QClipboard::Mode>(changedSpy.at(0).at(0)), QClipboard::Selection);
    } else {
        QCOMPARE(selectionChangedSpy.count(), 0);
    }
    QCOMPARE(dataChangedSpy.count(), 1);
    QCOMPARE(searchChangedSpy.count(), 0);

    changedSpy.clear();

    // Test the search mode signal.
    if (clipboard->supportsFindBuffer()) {
        clipboard->setText(text, QClipboard::FindBuffer);
        QCOMPARE(searchChangedSpy.count(), 1);
        QCOMPARE(changedSpy.count(), 1);
        QCOMPARE(changedSpy.at(0).count(), 1);
        QCOMPARE(qvariant_cast<QClipboard::Mode>(changedSpy.at(0).at(0)), QClipboard::FindBuffer);
    } else {
        QCOMPARE(searchChangedSpy.count(), 0);
    }
    QCOMPARE(dataChangedSpy.count(), 1);
}

static bool runHelper(const QString &program, const QStringList &arguments, QByteArray *errorMessage)
{
    QProcess process;
    process.setReadChannelMode(QProcess::ForwardedChannels);
    process.start(program, arguments);
    if (!process.waitForStarted()) {
        *errorMessage = "Unable to start '" + program.toLocal8Bit() + " ': "
                        + process.errorString().toLocal8Bit();
        return false;
    }

    // Windows: Due to implementation changes, the event loop needs
    // to be spun since we ourselves also need to answer the
    // WM_DRAWCLIPBOARD message as we are in the chain of clipboard
    // viewers.
    bool running = true;
    for (int i = 0; i < 60 && running; ++i) {
        QGuiApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
        if (process.waitForFinished(500))
            running = false;
    }
    if (running) {
        process.kill();
        *errorMessage = "Timeout running '" + program.toLocal8Bit() + '\'';
        return false;
    }
    if (process.exitStatus() != QProcess::NormalExit) {
        *errorMessage = "Process '" + program.toLocal8Bit() + "' crashed.";
        return false;
    }
    if (process.exitCode()) {
        *errorMessage = "Process '" + program.toLocal8Bit() + "' returns "
                        + QByteArray::number(process.exitCode());
        return false;
    }
    return true;
}

// Test that pasted text remains on the clipboard after a Qt application exits.
void tst_QClipboard::copy_exit_paste()
{
#ifndef QT_NO_PROCESS
#if !defined(Q_OS_WIN) && !defined(Q_OS_MAC)
    QSKIP("This test does not make sense on X11 and embedded, copied data disappears from the clipboard when the application exits ");
    // ### It's still possible to test copy/paste - just keep the apps running
#endif
    if (!nativeClipboardWorking())
        QSKIP("Native clipboard not working in this setup");
    const QStringList stringArgument(QStringLiteral("Test string."));
    QByteArray errorMessage;
    QVERIFY2(runHelper(QStringLiteral("copier/copier"), stringArgument, &errorMessage),
             errorMessage.constData());
#ifdef Q_OS_MAC
    // The Pasteboard needs a moment to breathe (at least on older Macs).
    QTest::qWait(100);
#endif
    QVERIFY2(runHelper(QStringLiteral("paster/paster"), stringArgument, &errorMessage),
             errorMessage.constData());
#endif
}

void tst_QClipboard::setMimeData()
{
    if (!nativeClipboardWorking())
        QSKIP("Native clipboard not working in this setup");
    QMimeData *mimeData = new QMimeData;
    const QString TestName(QLatin1String("tst_QClipboard::setMimeData() mimeData"));
    mimeData->setObjectName(TestName);
#if defined(Q_OS_WINCE)
    // need to set text on CE
    mimeData->setText(QLatin1String("Qt/CE foo"));
#endif

    QGuiApplication::clipboard()->setMimeData(mimeData);
    QCOMPARE(QGuiApplication::clipboard()->mimeData(), (const QMimeData *)mimeData);
    QCOMPARE(QGuiApplication::clipboard()->mimeData()->objectName(), TestName);

    // set it to the same data again, it shouldn't delete mimeData (and crash as a result)
    QGuiApplication::clipboard()->setMimeData(mimeData);
    QCOMPARE(QGuiApplication::clipboard()->mimeData(), (const QMimeData *)mimeData);
    QCOMPARE(QGuiApplication::clipboard()->mimeData()->objectName(), TestName);
    QGuiApplication::clipboard()->clear();
    const QMimeData *appMimeData = QGuiApplication::clipboard()->mimeData();
    QVERIFY(appMimeData != mimeData || appMimeData->objectName() != TestName);

    // check for crash when using the same mimedata object on several clipboards
    QMimeData *data = new QMimeData;
    data->setText("foo");

    QGuiApplication::clipboard()->setMimeData(data, QClipboard::Clipboard);
    QGuiApplication::clipboard()->setMimeData(data, QClipboard::Selection);
    QGuiApplication::clipboard()->setMimeData(data, QClipboard::FindBuffer);

    QSignalSpy spySelection(QGuiApplication::clipboard(), SIGNAL(selectionChanged()));
    QSignalSpy spyData(QGuiApplication::clipboard(), SIGNAL(dataChanged()));
    QSignalSpy spyFindBuffer(QGuiApplication::clipboard(), SIGNAL(findBufferChanged()));

    QGuiApplication::clipboard()->clear(QClipboard::Clipboard);
    QGuiApplication::clipboard()->clear(QClipboard::Selection); // used to crash on X11
    QGuiApplication::clipboard()->clear(QClipboard::FindBuffer);

    if (QGuiApplication::clipboard()->supportsSelection())
        QCOMPARE(spySelection.count(), 1);
    else
        QCOMPARE(spySelection.count(), 0);

    if (QGuiApplication::clipboard()->supportsFindBuffer())
        QCOMPARE(spyFindBuffer.count(), 1);
    else
        QCOMPARE(spyFindBuffer.count(), 0);

    QCOMPARE(spyData.count(), 1);

    // an other crash test
    data = new QMimeData;
    data->setText("foo");

    QGuiApplication::clipboard()->setMimeData(data, QClipboard::Clipboard);
    QGuiApplication::clipboard()->setMimeData(data, QClipboard::Selection);
    QGuiApplication::clipboard()->setMimeData(data, QClipboard::FindBuffer);

    QMimeData *newData = new QMimeData;
    newData->setText("bar");

    spySelection.clear();
    spyData.clear();
    spyFindBuffer.clear();

    QGuiApplication::clipboard()->setMimeData(newData, QClipboard::Clipboard);
    QGuiApplication::clipboard()->setMimeData(newData, QClipboard::Selection); // used to crash on X11
    QGuiApplication::clipboard()->setMimeData(newData, QClipboard::FindBuffer);

    if (QGuiApplication::clipboard()->supportsSelection())
        QCOMPARE(spySelection.count(), 1);
    else
        QCOMPARE(spySelection.count(), 0);

    if (QGuiApplication::clipboard()->supportsFindBuffer())
        QCOMPARE(spyFindBuffer.count(), 1);
    else
        QCOMPARE(spyFindBuffer.count(), 0);

    QCOMPARE(spyData.count(), 1);
}

void tst_QClipboard::clearBeforeSetText()
{
    QGuiApplication::processEvents();

    if (!nativeClipboardWorking())
        QSKIP("Native clipboard not working in this setup");

    const QString text = "tst_QClipboard::clearBeforeSetText()";

    // setText() should work after processEvents()
    QGuiApplication::clipboard()->setText(text);
    QCOMPARE(QGuiApplication::clipboard()->text(), text);
    QGuiApplication::processEvents();
    QCOMPARE(QGuiApplication::clipboard()->text(), text);

    // same with clear()
    QGuiApplication::clipboard()->clear();
    QVERIFY(QGuiApplication::clipboard()->text().isEmpty());
    QGuiApplication::processEvents();
    QVERIFY(QGuiApplication::clipboard()->text().isEmpty());

    // setText() again
    QGuiApplication::clipboard()->setText(text);
    QCOMPARE(QGuiApplication::clipboard()->text(), text);
    QGuiApplication::processEvents();
    QCOMPARE(QGuiApplication::clipboard()->text(), text);

    // clear() immediately followed by setText() should still return the text
    QGuiApplication::clipboard()->clear();
    QVERIFY(QGuiApplication::clipboard()->text().isEmpty());
    QGuiApplication::clipboard()->setText(text);
    QCOMPARE(QGuiApplication::clipboard()->text(), text);
    QGuiApplication::processEvents();
    QCOMPARE(QGuiApplication::clipboard()->text(), text);
}

QTEST_MAIN(tst_QClipboard)

#include "tst_qclipboard.moc"
