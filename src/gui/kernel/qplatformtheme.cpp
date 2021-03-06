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

#include "qplatformtheme.h"

#include <QtCore/QVariant>
#include <QtCore/QStringList>
#include <qpalette.h>
#include <qtextformat.h>

QT_BEGIN_NAMESPACE

/*!
    \class QPlatformTheme
    \since 5.0
    \internal
    \preliminary
    \ingroup qpa
    \brief The QPlatformTheme class allows customizing the UI based on themes.

*/

/*!
    \enum QPlatformTheme::ThemeHint

    This enum describes the available theme hints.

    \value CursorFlashTime (int) Cursor flash time in ms, overriding
                                 QPlatformIntegration::styleHint.

    \value KeyboardInputInterval (int) Keyboard input interval in ms, overriding
                                 QPlatformIntegration::styleHint.

    \value MouseDoubleClickInterval (int) Mouse double click interval in ms,
                                    overriding QPlatformIntegration::styleHint.

    \value StartDragDistance (int) Start drag distance,
                             overriding QPlatformIntegration::styleHint.

    \value StartDragTime (int) Start drag time in ms,,
                               overriding QPlatformIntegration::styleHint.

    \value KeyboardAutoRepeatRate (int) Keyboard auto repeat rate,
                                  overriding QPlatformIntegration::styleHint.

    \value PasswordMaskDelay (int) Pass word mask delay in ms,,
                                   overriding QPlatformIntegration::styleHint.

    \value StartDragVelocity (int) Velocity of a drag,
                                   overriding QPlatformIntegration::styleHint.

    \value TextCursorWidth  (int) Determines the width of the text cursor.

    \value DropShadow       (bool) Determines whether the drop shadow effect for
                            tooltips or whatsthis is enabled.

    \value MaximumScrollBarDragDistance (int) Determines the value returned by
                            QStyle::pixelMetric(PM_MaximumDragDistance)

    \value ToolButtonStyle (int) A value representing a Qt::ToolButtonStyle.

    \value ToolBarIconSize Icon size for tool bars.

    \value SystemIconThemeName (QString) Name of the icon theme.

    \value SystemIconFallbackThemeName (QString) Name of the fallback icon theme.

    \value IconThemeSearchPaths (QStringList) Search paths for icons.

    \value ItemViewActivateItemOnSingleClick (bool) Activate items by single click.

    \value StyleNames (QStringList) A list of preferred style names.

    \value WindowAutoPlacement (bool) A boolean value indicating whether Windows
                               (particularly dialogs) are placed by the system
                               (see _NET_WM_FULL_PLACEMENT in X11).

    \value DialogButtonBoxLayout (int) An integer representing a
                                 QDialogButtonBox::ButtonLayout value.

    \value DialogButtonBoxButtonsHaveIcons (bool) A boolean value indicating whether
                                            the buttons of a QDialogButtonBox should have icons.

    \value UseFullScreenForPopupMenu (bool) Pop menus can cover the full screen including task bar.

    \value KeyboardScheme (int) An integer value (enum KeyboardSchemes) specifying the
                           keyboard scheme.

    \value UiEffects (int) A flag value consisting of UiEffect values specifying the enabled UI animations.

    \value SpellCheckUnderlineStyle (int) A QTextCharFormat::UnderlineStyle specifying
                                    the underline style used misspelled words when spell checking.

    \sa themeHint(), QStyle::pixelMetric()
*/

QPlatformTheme::~QPlatformTheme()
{

}

bool QPlatformTheme::usePlatformNativeDialog(DialogType type) const
{
    Q_UNUSED(type);
    return false;
}

QPlatformDialogHelper *QPlatformTheme::createPlatformDialogHelper(DialogType type) const
{
    Q_UNUSED(type);
    return 0;
}

const QPalette *QPlatformTheme::palette(Palette type) const
{
    Q_UNUSED(type)
    return 0;
}

const QFont *QPlatformTheme::font(Font type) const
{
    Q_UNUSED(type)
    return 0;
}

QVariant QPlatformTheme::themeHint(ThemeHint hint) const
{
    return QPlatformTheme::defaultThemeHint(hint);
}

QVariant QPlatformTheme::defaultThemeHint(ThemeHint hint)
{
    switch (hint) {
    case QPlatformTheme::CursorFlashTime:
        return QVariant(1000);
    case QPlatformTheme::KeyboardInputInterval:
        return QVariant(400);
    case QPlatformTheme::KeyboardAutoRepeatRate:
        return QVariant(30);
    case QPlatformTheme::MouseDoubleClickInterval:
        return QVariant(400);
    case QPlatformTheme::StartDragDistance:
        return QVariant(10);
    case QPlatformTheme::StartDragTime:
        return QVariant(500);
    case QPlatformTheme::PasswordMaskDelay:
        return QVariant(int(0));
    case QPlatformTheme::StartDragVelocity:
        return QVariant(int(0)); // no limit
    case QPlatformTheme::UseFullScreenForPopupMenu:
        return QVariant(false);
    case QPlatformTheme::WindowAutoPlacement:
        return QVariant(false);
    case QPlatformTheme::DialogButtonBoxLayout:
        return QVariant(int(0));
    case QPlatformTheme::DialogButtonBoxButtonsHaveIcons:
        return QVariant(false);
    case QPlatformTheme::ItemViewActivateItemOnSingleClick:
        return QVariant(false);
    case QPlatformTheme::ToolButtonStyle:
        return QVariant(int(Qt::ToolButtonIconOnly));
    case QPlatformTheme::ToolBarIconSize:
        return QVariant(int(0));
    case QPlatformTheme::SystemIconThemeName:
    case QPlatformTheme::SystemIconFallbackThemeName:
        return QVariant(QString());
    case QPlatformTheme::IconThemeSearchPaths:
        return QVariant(QStringList());
    case QPlatformTheme::StyleNames:
        return QVariant(QStringList());
    case TextCursorWidth:
        return QVariant(1);
    case DropShadow:
        return QVariant(false);
    case MaximumScrollBarDragDistance:
        return QVariant(-1);
    case KeyboardScheme:
        return QVariant(int(WindowsKeyboardScheme));
    case UiEffects:
        return QVariant(int(0));
    case SpellCheckUnderlineStyle:
        return QVariant(int(QTextCharFormat::SpellCheckUnderline));
    }
    return QVariant();
}

QPlatformMenuItem *QPlatformTheme::createPlatformMenuItem() const
{
    return 0;
}

QPlatformMenu *QPlatformTheme::createPlatformMenu() const
{
    return 0;
}

QPlatformMenuBar *QPlatformTheme::createPlatformMenuBar() const
{
    return 0;
}

#ifndef QT_NO_SYSTEMTRAYICON
/*!
   Factory function for QSystemTrayIcon. This function will return 0 if the platform
   integration does not support creating any system tray icon.
*/
QPlatformSystemTrayIcon *QPlatformTheme::createPlatformSystemTrayIcon() const
{
    return 0;
}
#endif

QT_END_NAMESPACE
