/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef QWINDOWSCONTEXT_H
#define QWINDOWSCONTEXT_H

#include "qtwindowsglobal.h"
#include "qtwindows_additional.h"

#include <QtCore/QScopedPointer>
#include <QtCore/QSharedPointer>

struct IBindCtx;

QT_BEGIN_NAMESPACE

class QWindow;
class QPlatformScreen;
class QWindowsScreenManager;
class QWindowsWindow;
class QWindowsMimeConverter;
struct QWindowCreationContext;
struct QWindowsContextPrivate;
class QPoint;

#ifndef Q_OS_WINCE
struct QWindowsUser32DLL
{
    QWindowsUser32DLL();
    inline void init();
    inline bool initTouch();

    typedef BOOL (WINAPI *RegisterTouchWindow)(HWND, ULONG);
    typedef BOOL (WINAPI *GetTouchInputInfo)(HANDLE, UINT, PVOID, int);
    typedef BOOL (WINAPI *CloseTouchInputHandle)(HANDLE);
    typedef BOOL (WINAPI *SetLayeredWindowAttributes)(HWND, COLORREF, BYTE, DWORD);
    typedef BOOL (WINAPI *UpdateLayeredWindow)(HWND, HDC , const POINT *,
                 const SIZE *, HDC, const POINT *, COLORREF,
                 const BLENDFUNCTION *, DWORD);
    typedef BOOL (WINAPI *UpdateLayeredWindowIndirect)(HWND, const UPDATELAYEREDWINDOWINFO *);
    typedef BOOL (WINAPI *IsHungAppWindow)(HWND);

    // Functions missing in Q_CC_GNU stub libraries.
    SetLayeredWindowAttributes setLayeredWindowAttributes;
    UpdateLayeredWindow updateLayeredWindow;

    // Functions missing in older versions of Windows
    UpdateLayeredWindowIndirect updateLayeredWindowIndirect;
    IsHungAppWindow isHungAppWindow;

    // Touch functions from Windows 7 onwards (also for use with Q_CC_MSVC).
    RegisterTouchWindow registerTouchWindow;
    GetTouchInputInfo getTouchInputInfo;
    CloseTouchInputHandle closeTouchInputHandle;
};

struct QWindowsShell32DLL
{
    QWindowsShell32DLL();
    inline void init();

    typedef HRESULT (WINAPI *SHCreateItemFromParsingName)(PCWSTR, IBindCtx *, const GUID&, void **);

    SHCreateItemFromParsingName sHCreateItemFromParsingName;
};
#endif // Q_OS_WINCE

class QWindowsContext
{
    Q_DISABLE_COPY(QWindowsContext)
public:

    enum SystemInfoFlags
    {
        SI_RTL_Extensions = 0x1,
        SI_SupportsTouch = 0x2
    };

    // Verbose flag set by environment variable QT_LIGHTHOUSE_WINDOWS_VERBOSE
    static int verboseIntegration;
    static int verboseWindows;
    static int verboseBackingStore;
    static int verboseEvents;
    static int verboseFonts;
    static int verboseGL;
    static int verboseOLE;
    static int verboseInputMethods;
    static int verboseDialogs;
    static int verboseTheming;

    explicit QWindowsContext();
    ~QWindowsContext();

    int defaultDPI() const;

    QString registerWindowClass(const QWindow *w, bool isGL);
    QString registerWindowClass(QString cname, WNDPROC proc,
                                unsigned style = 0, HBRUSH brush = 0,
                                bool icon = false);
    HWND createDummyWindow(const QString &classNameIn,
                           const wchar_t *windowName,
                           WNDPROC wndProc = 0, DWORD style = WS_OVERLAPPED);

    HDC displayContext() const;
    int screenDepth() const;

    static QWindowsContext *instance();

    static QString windowsErrorMessage(unsigned long errorCode);

    void addWindow(HWND, QWindowsWindow *w);
    void removeWindow(HWND);

    QWindowsWindow *findClosestPlatformWindow(HWND) const;
    QWindowsWindow *findPlatformWindow(HWND) const;
    QWindow *findWindow(HWND) const;
    QWindowsWindow *findPlatformWindowAt(HWND parent, const QPoint &screenPoint,
                                             unsigned cwex_flags) const;

    QWindow *windowUnderMouse() const;

    inline bool windowsProc(HWND hwnd, UINT message,
                            QtWindows::WindowsEventType et,
                            WPARAM wParam, LPARAM lParam, LRESULT *result);

    QWindow *keyGrabber() const;
    void setKeyGrabber(QWindow *hwnd);

    void setWindowCreationContext(const QSharedPointer<QWindowCreationContext> &ctx);

    // Returns a combination of SystemInfoFlags
    unsigned systemInfo() const;

    bool useRTLExtensions() const;

    QWindowsMimeConverter &mimeConverter() const;
    QWindowsScreenManager &screenManager();
#ifndef Q_OS_WINCE
    static QWindowsUser32DLL user32dll;
    static QWindowsShell32DLL shell32dll;
#endif

    static QByteArray comErrorString(HRESULT hr);

private:
    void handleFocusEvent(QtWindows::WindowsEventType et, QWindowsWindow *w);
    void unregisterWindowClasses();

    QScopedPointer<QWindowsContextPrivate> d;
    static QWindowsContext *m_instance;
};

extern "C" LRESULT QT_WIN_CALLBACK qWindowsWndProc(HWND, UINT, WPARAM, LPARAM);

QT_END_NAMESPACE

#endif // QWINDOWSCONTEXT_H
