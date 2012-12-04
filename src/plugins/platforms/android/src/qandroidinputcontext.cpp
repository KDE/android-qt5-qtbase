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

#include <android/log.h>

#include "qandroidinputcontext.h"
#include "androidjnimain.h"
#include <QDebug>

static QAndroidInputContext * m_androidInputContext = 0;
static char const * const QtNativeInputConnectionClassName = "org/kde/necessitas/industrius/QtNativeInputConnection";
static char const * const QtExtractedTextClassName = "org/kde/necessitas/industrius/QtExtractedText";
static jclass m_extractedTextClass = 0;
static jmethodID m_classConstructorMethodID=0;
static jfieldID m_partialEndOffsetFieldID=0;
static jfieldID m_partialStartOffsetFieldID=0;
static jfieldID m_selectionEndFieldID=0;
static jfieldID m_selectionStartFieldID=0;
static jfieldID m_startOffsetFieldID=0;
static jfieldID m_textFieldID=0;

static jboolean commitText(JNIEnv *env, jobject /*thiz*/, jstring text, jint newCursorPosition)
{
    if (!m_androidInputContext)
        return JNI_FALSE;
    jboolean isCopy;
    const jchar * jstr = env->GetStringChars(text, &isCopy);
    QString str((const QChar*)jstr,  env->GetStringLength(text));
    env->ReleaseStringChars(text, jstr);
    return JNI_FALSE;
//FIXME
//    return m_androidInputContext->commitText( str
//                                            , newCursorPosition );
}

static jboolean deleteSurroundingText(JNIEnv */*env*/, jobject /*thiz*/, jint leftLength, jint rightLength)
{
    if (!m_androidInputContext)
        return JNI_FALSE;
    return JNI_FALSE;
//FIXME
//    return m_androidInputContext->deleteSurroundingText(leftLength, rightLength);
}

static jboolean finishComposingText(JNIEnv */*env*/, jobject /*thiz*/)
{
    if (!m_androidInputContext)
        return JNI_FALSE;
    return JNI_FALSE;
//FIXME
//    return m_androidInputContext->finishComposingText();
}

static jint getCursorCapsMode(JNIEnv */*env*/, jobject /*thiz*/, jint reqModes)
{
    if (!m_androidInputContext)
        return 0;
    return 0;
//FIXME
//    return m_androidInputContext->getCursorCapsMode(reqModes);
}

static jobject getExtractedText(JNIEnv *env, jobject /*thiz*/, int hintMaxChars, int hintMaxLines, jint flags)
{
    if (!m_androidInputContext)
        return 0;

    return 0;
//FIXME
//    const QAndroidInputContext::ExtractedText & extractedText=m_androidInputContext->getExtractedText(hintMaxChars, hintMaxLines, flags);
//    jobject object = env->NewObject(m_extractedTextClass, m_classConstructorMethodID);
//    env->SetIntField(object, m_partialStartOffsetFieldID, extractedText.partialStartOffset);
//    env->SetIntField(object, m_partialEndOffsetFieldID, extractedText.partialEndOffset);
//    env->SetIntField(object, m_selectionStartFieldID, extractedText.selectionStart);
//    env->SetIntField(object, m_selectionEndFieldID, extractedText.selectionEnd);
//    env->SetIntField(object, m_startOffsetFieldID, extractedText.startOffset);
//    env->SetObjectField(object, m_textFieldID, env->NewString((jchar*)extractedText.text.constData(), (jsize)extractedText.text.length()));
//    return object;
}

static jstring getSelectedText(JNIEnv * env, jobject /*thiz*/, jint flags)
{
    if (!m_androidInputContext)
        return 0;
    return 0;
//FIXME
//    const QString & text = m_androidInputContext->getSelectedText(flags);
//    return env->NewString((jchar*)text.constData(), (jsize)text.length());
}

static jstring getTextAfterCursor(JNIEnv * env, jobject /*thiz*/, jint length, jint flags)
{
    if (!m_androidInputContext)
        return 0;
    return 0;
//FIXME
//    const QString & text = m_androidInputContext->getTextAfterCursor(length, flags);
//    return env->NewString((jchar*)text.constData(), (jsize)text.length());
}

static jstring getTextBeforeCursor(JNIEnv * env, jobject /*thiz*/, jint length, jint flags)
{
    if (!m_androidInputContext)
        return 0;
    return 0;
//FIXME
//    const QString & text = m_androidInputContext->getTextBeforeCursor(length, flags);
//    return env->NewString((jchar*)text.constData(), (jsize)text.length());
}

static jboolean setComposingText(JNIEnv * env, jobject /*thiz*/, jstring text, jint newCursorPosition)
{
    if (!m_androidInputContext)
        return JNI_FALSE;
    return JNI_FALSE;
//FIXME
//    jboolean isCopy;
//    const jchar * jstr = env->GetStringChars(text, &isCopy);
//    QString str((const QChar*)jstr,  env->GetStringLength(text));
//    env->ReleaseStringChars(text, jstr);
//    return m_androidInputContext->setComposingText(str, newCursorPosition);
}

static jboolean setSelection(JNIEnv */*env*/, jobject /*thiz*/, jint start, jint end)
{
    if (!m_androidInputContext)
        return JNI_FALSE;
    return JNI_FALSE;
//FIXME
//    return m_androidInputContext->setSelection(start, end);
}

static jboolean selectAll(JNIEnv */*env*/, jobject /*thiz*/)
{
    if (!m_androidInputContext)
        return JNI_FALSE;
    return JNI_FALSE;
//FIXME
//    return m_androidInputContext->selectAll();
}

static jboolean cut(JNIEnv */*env*/, jobject /*thiz*/)
{
    if (!m_androidInputContext)
        return JNI_FALSE;
    return JNI_FALSE;
//FIXME
//    return m_androidInputContext->cut();
}

static jboolean copy(JNIEnv */*env*/, jobject /*thiz*/)
{
    if (!m_androidInputContext)
        return JNI_FALSE;
    return JNI_FALSE;
//FIXME
//    return m_androidInputContext->copy();
}

static jboolean copyURL(JNIEnv */*env*/, jobject /*thiz*/)
{
    if (!m_androidInputContext)
        return JNI_FALSE;
    return JNI_FALSE;
//FIXME
//    return m_androidInputContext->copyURL();
}

static jboolean paste(JNIEnv */*env*/, jobject /*thiz*/)
{
    if (!m_androidInputContext)
        return JNI_FALSE;
    return JNI_FALSE;
//FIXME
//    return m_androidInputContext->paste();
}


static JNINativeMethod methods[] = {
    {"commitText", "(Ljava/lang/String;I)Z", (void *)commitText},
    {"deleteSurroundingText", "(II)Z", (void *)deleteSurroundingText},
    {"finishComposingText", "()Z", (void *)finishComposingText},
    {"getCursorCapsMode", "(I)I", (void *)getCursorCapsMode},
    {"getExtractedText", "(III)Lorg/kde/necessitas/industrius/QtExtractedText;", (void *)getExtractedText},
    {"getSelectedText", "(I)Ljava/lang/String;", (void *)getSelectedText},
    {"getTextAfterCursor", "(II)Ljava/lang/String;", (void *)getTextAfterCursor},
    {"getTextBeforeCursor", "(II)Ljava/lang/String;", (void *)getTextBeforeCursor},
    {"setComposingText", "(Ljava/lang/String;I)Z", (void *)setComposingText},
    {"setSelection", "(II)Z", (void *)setSelection},
    {"selectAll", "()Z", (void *)selectAll},
    {"cut", "()Z", (void *)cut},
    {"copy", "()Z", (void *)copy},
    {"copyURL", "()Z", (void *)copyURL},
    {"paste", "()Z", (void *)paste}
};


QAndroidInputContext::QAndroidInputContext(QObject *parent)/* :
    QInputContext(parent)*/
{
    JNIEnv * env = 0;
    if (QtAndroid::javaVM()->AttachCurrentThread(&env, NULL)<0)
    {
        qCritical()<<"AttachCurrentThread failed";
        return;
    }
    jclass clazz = QtAndroid::findClass(QtNativeInputConnectionClassName, env);
    if (clazz == NULL)
    {
        qCritical()<<"Native registration unable to find class '"<<QtNativeInputConnectionClassName<<"'";
        return;
    }
    if (env->RegisterNatives(clazz, methods, sizeof(methods) / sizeof(methods[0])) < 0)
    {
        qCritical()<<"RegisterNatives failed for '"<<QtNativeInputConnectionClassName<<"'";
        return;
    }

    clazz = QtAndroid::findClass(QtExtractedTextClassName, env);
    if (clazz == NULL)
    {
        qCritical()<<"Native registration unable to find class '"<<QtExtractedTextClassName<<"'";
        return;
    }

    m_extractedTextClass = (jclass)env->NewGlobalRef(clazz);
    m_classConstructorMethodID = env->GetMethodID(m_extractedTextClass, "<init>", "()V");
    if (m_classConstructorMethodID == NULL)
    {
        qCritical()<<"GetMethodID failed";
        return;
    }

    m_partialEndOffsetFieldID = env->GetFieldID(m_extractedTextClass, "partialEndOffset", "I");
    if (m_partialEndOffsetFieldID == NULL)
    {
        qCritical()<<"Can't find field partialEndOffset";
        return;
    }

    m_partialStartOffsetFieldID = env->GetFieldID(m_extractedTextClass, "partialStartOffset", "I");
    if (m_partialStartOffsetFieldID == NULL)
    {
        qCritical()<<"Can't find field partialStartOffset";
        return;
    }

    m_selectionEndFieldID = env->GetFieldID(m_extractedTextClass, "selectionEnd", "I");
    if (m_selectionEndFieldID == NULL)
    {
        qCritical()<<"Can't find field selectionEnd";
        return;
    }

    m_selectionStartFieldID = env->GetFieldID(m_extractedTextClass, "selectionStart", "I");
    if (m_selectionStartFieldID == NULL)
    {
        qCritical()<<"Can't find field selectionStart";
        return;
    }

    m_startOffsetFieldID = env->GetFieldID(m_extractedTextClass, "startOffset", "I");
    if (m_startOffsetFieldID == NULL)
    {
        qCritical()<<"Can't find field startOffset";
        return;
    }

    m_textFieldID = env->GetFieldID(m_extractedTextClass, "text", "Ljava/lang/String;");
    if (m_textFieldID == NULL)
    {
        qCritical()<<"Can't find field text";
        return;
    }
//FIXME
//    qRegisterMetaType<QInputMethodEvent>("QInputMethodEvent");
    m_androidInputContext = this;
}

QAndroidInputContext::~QAndroidInputContext()
{
    m_androidInputContext = 0;
    m_extractedTextClass = 0;
    m_partialEndOffsetFieldID=0;
    m_partialStartOffsetFieldID=0;
    m_selectionEndFieldID=0;
    m_selectionStartFieldID=0;
    m_startOffsetFieldID=0;
    m_textFieldID=0;
}

bool QAndroidInputContext::isValid() const
{
    return m_androidInputContext;
}

void QAndroidInputContext::reset()
{
//FIXME
//    clear();
//    if (focusWidget())
//        QtAndroid::resetSoftwareKeyboard();
//    else
//        QtAndroid::hideSoftwareKeyboard();
}

void QAndroidInputContext::commit()
{
    //TODO
}

void QAndroidInputContext::update(Qt::InputMethodQueries)
{
    reset();
}

void QAndroidInputContext::invokeAction(QInputMethod::Action, int cursorPosition)
{
}

void QAndroidInputContext::showInputPanel()
{
}

void QAndroidInputContext::hideInputPanel()
{
}

bool QAndroidInputContext::isInputPanelVisible() const
{
}

QLocale QAndroidInputContext::locale() const
{
    return QLocale();
}

Qt::LayoutDirection QAndroidInputContext::inputDirection() const
{
    return Qt::LayoutDirectionAuto;
}

void QAndroidInputContext::setFocusObject(QObject *object)
{
    Q_UNUSED(object);
}

//QString QAndroidInputContext::identifierName()
//{
//    return "QAndroidInputContext";
//}

//bool QAndroidInputContext::isComposing() const
//{
//    return m_composingText.length();
//}

//QString QAndroidInputContext::language()
//{
//    return qgetenv("LANG");
//}

//void QAndroidInputContext::clear()
//{
//    m_composingText.clear();
//    m_extractedText.clear();
//}


//void QAndroidInputContext::setFocusWidget( QWidget *w )
//{
//    QInputContext::setFocusWidget( w );
//    if (!w)
//        reset();
//}

//bool QAndroidInputContext::filterEvent( const QEvent * event )
//{
//    if (event->type() == QEvent::RequestSoftwareInputPanel)
//    {
//        QWidget * w = focusWidget();
//        if (!w)
//            return QInputContext::filterEvent(event);
//        QRect wrect(w->mapToGlobal(w->rect().topLeft()),w->rect().size());
//        QtAndroid::showSoftwareKeyboard(wrect.left(), wrect.top(), wrect.width(), wrect.height(), w->inputMethodHints());
//        return true;
//    }
//    else if (event->type() == QEvent::CloseSoftwareInputPanel)
//    {
//        QtAndroid::hideSoftwareKeyboard();
//        return true;
//    } else if ( (event->type() == QEvent::KeyPress || event->type() == QEvent::KeyRelease) && isComposing() )
//    {
//        finishComposingText();
//    }
//    return QInputContext::filterEvent(event);
//}

//void QAndroidInputContext::sendEvent(const QInputMethodEvent &event)
//{
//    QInputContext::sendEvent(event);
//}

//jboolean QAndroidInputContext::commitText(const QString &text, jint /*newCursorPosition*/)
//{
//    m_composingText = text;
//    return finishComposingText();
//}

//jboolean QAndroidInputContext::deleteSurroundingText(jint leftLength, jint rightLength)
//{
//    QWidget * w = focusWidget();
//    if (!w)
//        return JNI_TRUE;
//    const int cursorPos= w->inputMethodQuery(Qt::ImCursorPosition).toInt();
//    setSelection(cursorPos-leftLength, cursorPos+rightLength);
//    m_composingText.clear();
//    finishComposingText();
//    return JNI_TRUE;
//}

//jboolean QAndroidInputContext::finishComposingText()
//{
//    if (!focusWidget())
//        return JNI_TRUE;
//    QInputMethodEvent event;
//    event.setCommitString(m_composingText);
//    QMetaObject::invokeMethod(this, "sendEvent", Qt::AutoConnection, Q_ARG(QInputMethodEvent, event));
//    clear();
//    return JNI_TRUE;
//}

//jint QAndroidInputContext::getCursorCapsMode(jint /*reqModes*/)
//{
//    QWidget * w = focusWidget();
//    jint res = 0;
//    if (!w)
//        return res;

//    const int qtInputMethodHints = w->inputMethodHints();

//    if ( qtInputMethodHints & Qt::ImhPreferUppercase )
//        res = CAP_MODE_SENTENCES;

//    if ( qtInputMethodHints & Qt::ImhUppercaseOnly)
//        res = CAP_MODE_CHARACTERS;

//    return res;
//}

//const QAndroidInputContext::ExtractedText & QAndroidInputContext::getExtractedText(jint /*hintMaxChars*/, jint /*hintMaxLines*/, jint /*flags*/)
//{
//    QWidget * w = focusWidget();
//    if (!w)
//        return m_extractedText;
//    m_extractedText.text = w->inputMethodQuery(Qt::ImSurroundingText).toString();
//    if (!m_extractedText.text.length())
//        return m_extractedText;
//    m_extractedText.startOffset = w->inputMethodQuery(Qt::ImCursorPosition).toInt();
//    const QString & selection = w->inputMethodQuery(Qt::ImCurrentSelection).toString();
//    const int selLen=selection.length();
//    if (selLen)
//    {
//        m_extractedText.selectionStart = w->inputMethodQuery(Qt::ImAnchorPosition).toInt();
//        m_extractedText.selectionEnd = m_extractedText.startOffset;
//    }
//    return m_extractedText;
//}

//QString QAndroidInputContext::getSelectedText(jint /*flags*/)
//{
//    QWidget * w = focusWidget();
//    if (!w)
//        return QString();
//    return w->inputMethodQuery(Qt::ImCurrentSelection).toString();
//}

//QString QAndroidInputContext::getTextAfterCursor(jint /*length*/, jint /*flags*/)
//{
//    QWidget * w = focusWidget();
//    if (!w)
//        return QString();
//    QString text = w->inputMethodQuery(Qt::ImSurroundingText).toString();
//    if (!text.length())
//        return text;
//    int cursorPos = w->inputMethodQuery(Qt::ImCursorPosition).toInt();
//    return text.mid(cursorPos);
//}

//QString QAndroidInputContext::getTextBeforeCursor(jint /*length*/, jint /*flags*/)
//{
//    QWidget * w = focusWidget();
//    if (!w)
//        return QString();
//    QString text = w->inputMethodQuery(Qt::ImSurroundingText).toString();
//    if (!text.length())
//        return text;
//    int cursorPos = w->inputMethodQuery(Qt::ImCursorPosition).toInt();
//    return text.left(cursorPos+1);
//}

//jboolean QAndroidInputContext::setComposingText(const QString & text, jint newCursorPosition)
//{
//    QWidget * w = focusWidget();
//    if (!w)
//        return JNI_FALSE;
//    newCursorPosition+=text.length()-1;
//    int cursorPos = w->inputMethodQuery(Qt::ImCursorPosition).toInt();
//    m_composingText=text;
//    QList<QInputMethodEvent::Attribute> attributes;
//    attributes.append(QInputMethodEvent::Attribute(QInputMethodEvent::Cursor,
//                                                cursorPos+newCursorPosition,
//                                                1,
//                                                QVariant()));
//    QInputMethodEvent event(m_composingText, attributes);
//    QMetaObject::invokeMethod(this, "sendEvent", Qt::AutoConnection, Q_ARG(QInputMethodEvent, event));
//    return JNI_TRUE;
//}

//jboolean QAndroidInputContext::setSelection(jint start, jint end)
//{
//    finishComposingText();
//    QList<QInputMethodEvent::Attribute> attributes;
//    attributes.append(QInputMethodEvent::Attribute(QInputMethodEvent::Selection,
//                                                end,
//                                                start,
//                                                QVariant()));
//    QInputMethodEvent event(QString(), attributes);
//    QMetaObject::invokeMethod(this, "sendEvent", Qt::AutoConnection, Q_ARG(QInputMethodEvent, event));
//    return JNI_TRUE;
//}

//jboolean QAndroidInputContext::selectAll()
//{
//#warning TODO
//    return JNI_TRUE;
//}

//jboolean QAndroidInputContext::cut()
//{
//#warning TODO
//    return JNI_TRUE;
//}

//jboolean QAndroidInputContext::copy()
//{
//#warning TODO
//    return JNI_TRUE;
//}

//jboolean QAndroidInputContext::copyURL()
//{
//#warning TODO
//    return JNI_TRUE;
//}

//jboolean QAndroidInputContext::paste()
//{
//#warning TODO
//    return JNI_TRUE;
//}
