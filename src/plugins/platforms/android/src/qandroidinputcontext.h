/******************************************************************************
* Copyright 2012  BogDan Vatra <bogdan@kde.org>                               *
*                                                                             *
* This library is free software; you can redistribute it and/or               *
* modify it under the terms of the GNU Lesser General Public                  *
* License as published by the Free Software Foundation; either                *
* version 2.1 of the License, or (at your option) version 3, or any           *
* later version accepted by the membership of KDE e.V. (or its                *
* successor approved by the membership of KDE e.V.), which shall              *
* act as a proxy defined in Section 6 of version 3 of the license.            *
*                                                                             *
* This library is distributed in the hope that it will be useful,             *
* but WITHOUT ANY WARRANTY; without even the implied warranty of              *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU           *
* Lesser General Public License for more details.                             *
*                                                                             *
* You should have received a copy of the GNU Lesser General Public            *
* License along with this library. If not, see <http://www.gnu.org/licenses/>.*
******************************************************************************/

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
