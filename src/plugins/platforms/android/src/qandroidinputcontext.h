/****************************************************************************
**
** Copyright (C) 2012 BogDan Vatra <bogdan@kde.org>
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef ANDROIDINPUTCONTEXT_H
#define ANDROIDINPUTCONTEXT_H

#include <qpa/qplatforminputcontext.h>
#include <jni.h>

class QAndroidInputContext : public QPlatformInputContext
{
    Q_OBJECT
    enum CapsMode
    {
        CAP_MODE_CHARACTERS = 0x00001000,
        CAP_MODE_SENTENCES  = 0x00004000,
        CAP_MODE_WORDS      = 0x00002000
    };

public:
    struct ExtractedText
    {
        ExtractedText() {clear();}
        void clear()
        {
            partialEndOffset=partialStartOffset=selectionEnd=selectionStart=startOffset = -1;
            text.clear();
        }
        int partialEndOffset;
        int partialStartOffset;
        int selectionEnd;
        int selectionStart;
        int startOffset;
        QString text;
    };

public:
    explicit QAndroidInputContext(QObject *parent = 0);
    ~QAndroidInputContext();
    virtual bool isValid() const;

    virtual void reset();
    virtual void commit();
    virtual void update(Qt::InputMethodQueries);
    virtual void invokeAction(QInputMethod::Action, int cursorPosition);
    virtual void showInputPanel();
    virtual void hideInputPanel();
    virtual bool isInputPanelVisible() const;
    virtual QLocale locale() const;
    virtual Qt::LayoutDirection inputDirection() const;

    virtual void setFocusObject(QObject *object);
//    virtual QString identifierName ();
//    virtual bool isComposing () const;
//    virtual QString language ();
//    void clear();

//    virtual void setFocusWidget( QWidget *w );
//    virtual bool filterEvent ( const QEvent * event );

//    //---------------//
//    jboolean commitText(const QString & text, jint newCursorPosition);
//    jboolean deleteSurroundingText(jint leftLength, jint rightLength);
//    jboolean finishComposingText();
//    jint getCursorCapsMode(jint reqModes);
//    const ExtractedText & getExtractedText(jint hintMaxChars, jint hintMaxLines, jint flags);
//    QString getSelectedText(jint flags);
//    QString getTextAfterCursor(jint length, jint flags);
//    QString getTextBeforeCursor(jint length, jint flags);
//    jboolean setComposingText(const QString & text, jint newCursorPosition);
//    jboolean setSelection(jint start, jint end);
//    jboolean selectAll();
//    jboolean cut();
//    jboolean copy();
//    jboolean copyURL();
//    jboolean paste();

//signals:

//private slots:
//    virtual void sendEvent(const QInputMethodEvent &event);

//private:
//    ExtractedText m_extractedText;
//    QString m_composingText;
};

#endif // ANDROIDINPUTCONTEXT_H
