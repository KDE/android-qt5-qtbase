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

#ifndef ANDROID_APP_H
#define ANDROID_APP_H
#ifdef ANDROID_PLUGIN_OPENGL
#include <EGL/eglplatform.h>
#endif

#include <jni.h>
#include "native/include/android/asset_manager.h"
#include <QString>

class QImage;
class QRect;
class QPoint;
class QThread;
class QAndroidPlatformIntegration;
class QMenuBar;
class QMenu;
class QWidget;
class QAndroidPlatformClipboard;

namespace QtAndroid
{
    typedef union {
        JNIEnv* nativeEnvironment;
        void* venv;
    } UnionJNIEnvToVoid;

    void setAndroidPlatformIntegration(QAndroidPlatformIntegration * androidGraphicsSystem);
    void setQtThread(QThread * thread);

    void setFullScreen(QWidget * widget);

    // Software keyboard support
    void showSoftwareKeyboard(int top, int left, int width, int height, int inputHints);
    void resetSoftwareKeyboard();
    void hideSoftwareKeyboard();
    // Software keyboard support

    // Clipboard support
    void setClipboardListener(QAndroidPlatformClipboard* listener);
    void setClipboardText(const QString &text);
    bool hasClipboardText();
    QString clipboardText();
    // Clipboard support

#ifndef ANDROID_PLUGIN_OPENGL
    void flushImage(const QPoint & pos, const QImage & image, const QRect & rect);
#else
    EGLNativeWindowType getNativeWindow(bool waitToCreate=true);
#endif

    JavaVM * javaVM();
    jclass findClass(const QString & className, JNIEnv * env);
    AAssetManager * assetManager();
    jclass applicationClass();
}
#endif // ANDROID_APP_H
