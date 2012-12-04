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

#include "qandroidplatformmenu.h"
#include "qandroidplatformmenuitem.h"
#include "androidjnimenu.h"

QAndroidPlatformMenu::QAndroidPlatformMenu()
{
    m_tag = 0;
    m_enabled = true;
    m_isVisible = true;
}

QAndroidPlatformMenu::~QAndroidPlatformMenu()
{
    QtAndroidMenu::androidPlatformMenuDestroyed(this);
}

void QAndroidPlatformMenu::insertMenuItem(QPlatformMenuItem *menuItem, QPlatformMenuItem *before)
{
    QMutexLocker lock(&m_menuItemsMutex);
    m_menuItems.insert(qFind(m_menuItems.begin(), m_menuItems.end(), static_cast<QAndroidPlatformMenuItem*>(before))
                       , static_cast<QAndroidPlatformMenuItem*>(menuItem));
}

void QAndroidPlatformMenu::removeMenuItem(QPlatformMenuItem *menuItem)
{
    QMutexLocker lock(&m_menuItemsMutex);
    m_menuItems.erase(qFind(m_menuItems.begin(), m_menuItems.end(), static_cast<QAndroidPlatformMenuItem*>(menuItem)));
}

void QAndroidPlatformMenu::syncMenuItem(QPlatformMenuItem *menuItem)
{
    PlatformMenuItemsType::iterator it;
    for(it = m_menuItems.begin(); it != m_menuItems.end();++it)
        if ((*it)->tag() == menuItem->tag())
            break;
    if (it != m_menuItems.end())
        QtAndroidMenu::syncMenu(this);
}

void QAndroidPlatformMenu::syncSeparatorsCollapsible(bool enable)
{
    Q_UNUSED(enable)
}

void QAndroidPlatformMenu::setTag(quintptr tag)
{
    m_tag = tag;
}

quintptr QAndroidPlatformMenu::tag() const
{
    return m_tag;
}

void QAndroidPlatformMenu::setText(const QString &text)
{
    m_text = text;
}

const QString &QAndroidPlatformMenu::text()
{
    return m_text;
}

void QAndroidPlatformMenu::setIcon(const QIcon &icon)
{
    m_icon = icon;
}

const QIcon &QAndroidPlatformMenu::icon()
{
    return m_icon;
}

void QAndroidPlatformMenu::setEnabled(bool enabled)
{
    m_enabled = enabled;
}

bool QAndroidPlatformMenu::isEnabled()
{
    return m_enabled;
}

void QAndroidPlatformMenu::setVisible(bool visible)
{
    m_isVisible = visible;
}

bool QAndroidPlatformMenu::isVisible()
{
    return m_isVisible;
}

QPlatformMenuItem *QAndroidPlatformMenu::menuItemAt(int position) const
{
    if (position<m_menuItems.size())
        return m_menuItems[position];
    return 0;
}

QPlatformMenuItem *QAndroidPlatformMenu::menuItemForTag(quintptr tag) const
{
    foreach(QPlatformMenuItem *menuItem, m_menuItems)
        if (menuItem->tag() == tag)
            return menuItem;
    return 0;
}

const QAndroidPlatformMenu::PlatformMenuItemsType &QAndroidPlatformMenu::menuItems()
{
    return m_menuItems;
}

QMutex *QAndroidPlatformMenu::menuItemsMutex()
{
    return &m_menuItemsMutex;
}
