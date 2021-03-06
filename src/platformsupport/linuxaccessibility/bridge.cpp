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


#include "bridge_p.h"

#include <atspi/atspi-constants.h>
#include <qstring.h>

#include "atspiadaptor_p.h"

#include "cache_p.h"
#include "constant_mappings_p.h"
#include "dbusconnection_p.h"
#include "struct_marshallers_p.h"

#include "deviceeventcontroller_adaptor.h"

QT_BEGIN_NAMESPACE

/*!
    \class QSpiAccessibleBridge
    \internal
*/

QSpiAccessibleBridge::QSpiAccessibleBridge()
    : cache(0)
{
    dbusConnection = new DBusConnection();
    if (!dBusConnection().isConnected())
        qWarning() << "Could not connect to dbus.";

    qSpiInitializeStructTypes();
    initializeConstantMappings();

    /* Create the cache of accessible objects */
    cache = new QSpiDBusCache(dBusConnection(), this);
    dec = new DeviceEventControllerAdaptor(this);

    bool reg = dBusConnection().registerObject(QLatin1String(ATSPI_DBUS_PATH_DEC), this, QDBusConnection::ExportAdaptors);
    qDebug() << "Registered DEC: " << reg;

    dbusAdaptor = new AtSpiAdaptor(dbusConnection, this);
    dBusConnection().registerVirtualObject(QLatin1String(QSPI_OBJECT_PATH_ACCESSIBLE), dbusAdaptor, QDBusConnection::SubPath);
    dbusAdaptor->registerApplication();
}

QSpiAccessibleBridge::~QSpiAccessibleBridge()
{
    delete dbusConnection;
} // Qt currently doesn't delete plugins.

QDBusConnection QSpiAccessibleBridge::dBusConnection() const
{
    return dbusConnection->connection();
}

void QSpiAccessibleBridge::setRootObject(QAccessibleInterface *interface)
{
    // the interface we get will be for the QApplication object.
    // we already cache it in the constructor.
    Q_ASSERT(interface->object() == qApp);
    dbusAdaptor->setInitialized(true);
}

void QSpiAccessibleBridge::notifyAccessibilityUpdate(QAccessibleEvent *event)
{
    dbusAdaptor->notify(event);
}

struct RoleMapping {
    QAccessible::Role role;
    AtspiRole spiRole;
    const char *name;
    const char *localizedName;
};

static RoleMapping map[] = {
    { QAccessible::NoRole, ATSPI_ROLE_INVALID, "invalid", QT_TR_NOOP("invalid role") },
    { QAccessible::TitleBar, ATSPI_ROLE_TEXT, "text", QT_TR_NOOP("title bar") },
    { QAccessible::MenuBar, ATSPI_ROLE_MENU_BAR, "menu bar", QT_TR_NOOP("menu bar") },
    { QAccessible::ScrollBar, ATSPI_ROLE_SCROLL_BAR, "scroll bar", QT_TR_NOOP("scroll bar") },
    { QAccessible::Grip, ATSPI_ROLE_UNKNOWN, "unknown", QT_TR_NOOP("grip") },
    { QAccessible::Sound, ATSPI_ROLE_UNKNOWN, "unknown", QT_TR_NOOP("sound") },
    { QAccessible::Cursor, ATSPI_ROLE_ARROW, "arrow", QT_TR_NOOP("cursor") },
    { QAccessible::Caret, ATSPI_ROLE_UNKNOWN, "unknown", QT_TR_NOOP("caret") },
    { QAccessible::AlertMessage, ATSPI_ROLE_ALERT, "alert", QT_TR_NOOP("alert message") },
    { QAccessible::Window, ATSPI_ROLE_WINDOW, "window", QT_TR_NOOP("window") },
    { QAccessible::Client, ATSPI_ROLE_FILLER, "filler", QT_TR_NOOP("filler") },
    { QAccessible::PopupMenu, ATSPI_ROLE_POPUP_MENU, "popup menu", QT_TR_NOOP("popup menu") },
    { QAccessible::MenuItem, ATSPI_ROLE_MENU_ITEM, "menu item", QT_TR_NOOP("menu item") },
    { QAccessible::ToolTip, ATSPI_ROLE_TOOL_TIP, "tool tip", QT_TR_NOOP("tool tip") },
    { QAccessible::Application, ATSPI_ROLE_APPLICATION, "application", QT_TR_NOOP("application") },
    { QAccessible::Document, ATSPI_ROLE_DOCUMENT_FRAME, "document frame", QT_TR_NOOP("document") },
    { QAccessible::Pane, ATSPI_ROLE_PANEL, "panel", QT_TR_NOOP("pane") },
    { QAccessible::Chart, ATSPI_ROLE_CHART, "chart", QT_TR_NOOP("chart") },
    { QAccessible::Dialog, ATSPI_ROLE_DIALOG, "dialog", QT_TR_NOOP("dialog") },
    { QAccessible::Border, ATSPI_ROLE_FRAME, "frame", QT_TR_NOOP("border") },
    { QAccessible::Grouping, ATSPI_ROLE_PANEL, "panel", QT_TR_NOOP("grouping") },
    { QAccessible::Separator, ATSPI_ROLE_SEPARATOR, "separator", QT_TR_NOOP("separator") },
    { QAccessible::ToolBar, ATSPI_ROLE_TOOL_BAR, "tool bar", QT_TR_NOOP("tool bar") },
    { QAccessible::StatusBar, ATSPI_ROLE_STATUS_BAR, "statusbar", QT_TR_NOOP("status bar") },
    { QAccessible::Table, ATSPI_ROLE_TABLE, "table", QT_TR_NOOP("table") },
    { QAccessible::ColumnHeader, ATSPI_ROLE_TABLE_COLUMN_HEADER, "column header", QT_TR_NOOP("column header") },
    { QAccessible::RowHeader, ATSPI_ROLE_TABLE_ROW_HEADER, "row header", QT_TR_NOOP("row header") },
    { QAccessible::Column, ATSPI_ROLE_TABLE_CELL, "table cell", QT_TR_NOOP("column") },
    { QAccessible::Row, ATSPI_ROLE_TABLE_CELL, "table cell", QT_TR_NOOP("row") },
    { QAccessible::Cell, ATSPI_ROLE_TABLE_CELL, "table cell", QT_TR_NOOP("cell") },
    { QAccessible::Link, ATSPI_ROLE_LINK, "link", QT_TR_NOOP("link") },
    { QAccessible::HelpBalloon, ATSPI_ROLE_DIALOG, "dialog", QT_TR_NOOP("help balloon") },
    { QAccessible::Assistant, ATSPI_ROLE_DIALOG, "dialog", QT_TR_NOOP("assistant") },
    { QAccessible::List, ATSPI_ROLE_LIST, "list", QT_TR_NOOP("list") },
    { QAccessible::ListItem, ATSPI_ROLE_LIST_ITEM, "list item", QT_TR_NOOP("list item") },
    { QAccessible::Tree, ATSPI_ROLE_TREE, "tree", QT_TR_NOOP("tree") },
    { QAccessible::TreeItem, ATSPI_ROLE_TABLE_CELL, "tree item", QT_TR_NOOP("tree item") },
    { QAccessible::PageTab, ATSPI_ROLE_PAGE_TAB, "page tab", QT_TR_NOOP("page tab") },
    { QAccessible::PropertyPage, ATSPI_ROLE_PAGE_TAB, "page tab", QT_TR_NOOP("property page") },
    { QAccessible::Indicator, ATSPI_ROLE_UNKNOWN, "unknown", QT_TR_NOOP("indicator") },
    { QAccessible::Graphic, ATSPI_ROLE_IMAGE, "image", QT_TR_NOOP("graphic") },
    { QAccessible::StaticText, ATSPI_ROLE_LABEL, "label", QT_TR_NOOP("label") },
    { QAccessible::EditableText, ATSPI_ROLE_TEXT, "text", QT_TR_NOOP("text") },
    { QAccessible::PushButton, ATSPI_ROLE_PUSH_BUTTON, "push button", QT_TR_NOOP("push button") },
    { QAccessible::CheckBox, ATSPI_ROLE_CHECK_BOX, "check box", QT_TR_NOOP("check box") },
    { QAccessible::RadioButton, ATSPI_ROLE_RADIO_BUTTON, "radio button", QT_TR_NOOP("radio box") },
    { QAccessible::ComboBox, ATSPI_ROLE_COMBO_BOX, "combo box", QT_TR_NOOP("combo box") },
    { QAccessible::ProgressBar, ATSPI_ROLE_PROGRESS_BAR, "progress bar", QT_TR_NOOP("progress bar") },
    { QAccessible::Dial, ATSPI_ROLE_DIAL, "accelerator label", QT_TR_NOOP("dial") },
    { QAccessible::HotkeyField, ATSPI_ROLE_TEXT, "text", QT_TR_NOOP("hotkey field") }, //FIXME text?
    { QAccessible::Slider, ATSPI_ROLE_SLIDER, "slider", QT_TR_NOOP("slider") },
    { QAccessible::SpinBox, ATSPI_ROLE_SPIN_BUTTON, "spin button", QT_TR_NOOP("spin box") },
    { QAccessible::Canvas, ATSPI_ROLE_CANVAS, "canvas", QT_TR_NOOP("canvas") },
    { QAccessible::Animation, ATSPI_ROLE_ANIMATION, "animation", QT_TR_NOOP("animation") },
    { QAccessible::Equation, ATSPI_ROLE_TEXT, "text", QT_TR_NOOP("equation") },
    { QAccessible::ButtonDropDown, ATSPI_ROLE_PUSH_BUTTON, "push button", QT_TR_NOOP("button drop down") },
    { QAccessible::ButtonMenu, ATSPI_ROLE_PUSH_BUTTON, "push button", QT_TR_NOOP("button menu") },
    { QAccessible::ButtonDropGrid, ATSPI_ROLE_PUSH_BUTTON, "push button", QT_TR_NOOP("button drop grid") },
    { QAccessible::Whitespace, ATSPI_ROLE_FILLER, "filler", QT_TR_NOOP("whitespace") },
    { QAccessible::PageTabList, ATSPI_ROLE_PAGE_TAB_LIST, "page tab list", QT_TR_NOOP("page tab list") },
    { QAccessible::Clock, ATSPI_ROLE_UNKNOWN, "unknown", QT_TR_NOOP("clock") },
    { QAccessible::Splitter, ATSPI_ROLE_SPLIT_PANE, "split pane", QT_TR_NOOP("splitter") },
    { QAccessible::LayeredPane, ATSPI_ROLE_LAYERED_PANE, "layered pane", QT_TR_NOOP("layered pane") },
    { QAccessible::UserRole, ATSPI_ROLE_UNKNOWN, "unknown", QT_TR_NOOP("user role") }
};

void QSpiAccessibleBridge::initializeConstantMappings()
{
    for (uint i = 0; i < sizeof(map) / sizeof(RoleMapping); ++i)
        qSpiRoleMapping.insert(map[i].role, RoleNames(map[i].spiRole, QLatin1String(map[i].name), tr(map[i].localizedName)));
}

QT_END_NAMESPACE
