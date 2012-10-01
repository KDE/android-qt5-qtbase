/******************************************************************************
* Copyright 2012  BogDan Vatra <bog_dan_ro@yahoo.com>                         *
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

package org.kde.necessitas.industrius;

import android.content.Context;
import android.content.Intent;
import android.text.Editable;
import android.text.InputFilter;
import android.util.Log;
import android.view.KeyEvent;
import android.view.View;
import android.view.inputmethod.BaseInputConnection;
import android.view.inputmethod.CompletionInfo;
import android.view.inputmethod.ExtractedText;
import android.view.inputmethod.ExtractedTextRequest;
import android.view.inputmethod.InputMethodManager;

class QtExtractedText
{
    public int partialEndOffset;
    public int partialStartOffset;
    public int selectionEnd;
    public int selectionStart;
    public int startOffset;
    public String text;
}

class QtNativeInputConnection
{
    static native boolean commitText(String text, int newCursorPosition);
    static native boolean commitCompletion(String text, int position);
    static native boolean deleteSurroundingText(int leftLength, int rightLength);
    static native boolean finishComposingText();
    static native int getCursorCapsMode(int reqModes);
    static native QtExtractedText getExtractedText(int hintMaxChars, int hintMaxLines, int flags);
    static native String getSelectedText(int flags);
    static native String getTextAfterCursor(int length, int flags);
    static native String getTextBeforeCursor(int length, int flags);
    static native boolean setComposingText(String text, int newCursorPosition);
    static native boolean setSelection(int start, int end);
    static native boolean selectAll();
    static native boolean cut();
    static native boolean copy();
    static native boolean copyURL();
    static native boolean paste();
}

public class QtInputConnection extends BaseInputConnection
{
    private static final int ID_SELECT_ALL = android.R.id.selectAll;
    private static final int ID_START_SELECTING_TEXT = android.R.id.startSelectingText;
    private static final int ID_STOP_SELECTING_TEXT = android.R.id.stopSelectingText;
    private static final int ID_CUT = android.R.id.cut;
    private static final int ID_COPY = android.R.id.copy;
    private static final int ID_PASTE = android.R.id.paste;
    private static final int ID_COPY_URL = android.R.id.copyUrl;
    private static final int ID_SWITCH_INPUT_METHOD = android.R.id.switchInputMethod;
    private static final int ID_ADD_TO_DICTIONARY = android.R.id.addToDictionary;
    View m_view;
    boolean m_closing;
    public QtInputConnection(View targetView)
    {
        super(targetView, true);
        m_view = targetView;
        m_closing = false;
    }

    @Override
    public boolean beginBatchEdit() {
        m_closing = false;
        return true;
    }

    @Override
    public boolean endBatchEdit() {
        m_closing = false;
        return true;
    }

    @Override
    public boolean commitCompletion(CompletionInfo text) {
        m_closing = false;
        return QtNativeInputConnection.commitCompletion(text.getText().toString(), text.getPosition());
    }

    @Override
    public boolean commitText(CharSequence text, int newCursorPosition) {
        m_closing = false;
        return QtNativeInputConnection.commitText(text.toString(), newCursorPosition);
    }

    @Override
    public boolean deleteSurroundingText(int leftLength, int rightLength) {
        m_closing = false;
        return QtNativeInputConnection.deleteSurroundingText(leftLength, rightLength);
    }

    @Override
    public boolean finishComposingText() {
        if (m_closing)
        {
            QtNative.activityDelegate().m_keyboardIsHiding=true;
            m_view.postDelayed(new Runnable() {
                @Override
                public void run() {
                    if (QtNative.activityDelegate().m_keyboardIsHiding)
                        QtNative.activityDelegate().m_keyboardIsVisible=false;
                }
            }, 5000); // it seems finishComposingText comes musch faster than onKeyUp event,
                      // so we must delay hide notification
            m_closing = false;
        }
        else
            m_closing = true;
        return QtNativeInputConnection.finishComposingText();
    }

    @Override
    public int getCursorCapsMode(int reqModes) {
        return QtNativeInputConnection.getCursorCapsMode(reqModes);
    }

    @Override
    public ExtractedText getExtractedText(ExtractedTextRequest request, int flags) {
        QtExtractedText qExtractedText = QtNativeInputConnection.getExtractedText(request.hintMaxChars, request.hintMaxLines, flags);
        ExtractedText extractedText = new ExtractedText();
        extractedText.partialEndOffset = qExtractedText.partialEndOffset;
        extractedText.partialStartOffset = qExtractedText.partialStartOffset;
        extractedText.selectionEnd = qExtractedText.selectionEnd;
        extractedText.selectionStart = qExtractedText.selectionStart;
        extractedText.startOffset = qExtractedText.startOffset;
        extractedText.text = qExtractedText.text;
        return extractedText;
    }

    public CharSequence getSelectedText(int flags) {
        return QtNativeInputConnection.getSelectedText(flags);
    }

    @Override
    public CharSequence getTextAfterCursor(int length, int flags) {
        return QtNativeInputConnection.getTextAfterCursor(length, flags);
    }

    @Override
    public CharSequence getTextBeforeCursor(int length, int flags) {
        return QtNativeInputConnection.getTextBeforeCursor(length, flags);
    }

    @Override
    public boolean performContextMenuAction(int id)
    {
        switch(id)
        {
        case ID_SELECT_ALL:
            return QtNativeInputConnection.selectAll();
        case ID_COPY:
            return QtNativeInputConnection.copy();
        case ID_COPY_URL:
            return QtNativeInputConnection.copyURL();
        case ID_CUT:
            return QtNativeInputConnection.cut();
        case ID_PASTE:
            return QtNativeInputConnection.paste();

        case ID_SWITCH_INPUT_METHOD:
            InputMethodManager imm = (InputMethodManager)m_view.getContext().getSystemService(Context.INPUT_METHOD_SERVICE);
            if (imm != null) {
                imm.showInputMethodPicker();
            }
            return true;

        case ID_ADD_TO_DICTIONARY:
// TODO
//            String word = m_editable.subSequence(0, m_editable.length()).toString();
//            if (word != null) {
//                Intent i = new Intent("com.android.settings.USER_DICTIONARY_INSERT");
//                i.putExtra("word", word);
//                i.setFlags(i.getFlags() | Intent.FLAG_ACTIVITY_NEW_TASK);
//                m_view.getContext().startActivity(i);
//            }
            return true;
        }
        return super.performContextMenuAction(id);
    }

    @Override
    public boolean setComposingText(CharSequence text, int newCursorPosition) {
        return QtNativeInputConnection.setComposingText(text.toString(), newCursorPosition);
    }

    @Override
    public boolean setSelection(int start, int end) {
        return QtNativeInputConnection.setSelection(start, end);
    }
}
