/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
**
** This file is part of the QtCore module of the Qt Toolkit.
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

#ifndef QLIBRARYINFO_H
#define QLIBRARYINFO_H

#include <QtCore/qstring.h>
#include <QtCore/QDate>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class Q_CORE_EXPORT QLibraryInfo
{
public:
    static QString licensee();
    static QString licensedProducts();

#ifndef QT_NO_DATESTRING
    static QDate buildDate();
#endif //QT_NO_DATESTRING

    static bool isDebugBuild();

    enum LibraryLocation
    {
        PrefixPath = 0,
        DocumentationPath,
        HeadersPath,
        LibrariesPath,
        BinariesPath,
        PluginsPath,
        ImportsPath,
        DataPath,
        TranslationsPath,
        ExamplesPath,
        TestsPath,
        // Insert new values above this line
#ifdef QT_BUILD_QMAKE
        // These are not subject to binary compatibility constraints
        SysrootPath,
        HostPrefixPath,
        HostBinariesPath,
        HostDataPath,
        LastHostPath = HostDataPath,
#endif
        SettingsPath = 100
    };
    static QString location(LibraryLocation); // ### Qt 6: consider renaming it to path()
#ifdef QT_BUILD_QMAKE
    enum PathGroup { FinalPaths, EffectivePaths };
    static QString rawLocation(LibraryLocation, PathGroup);
#endif

private:
    QLibraryInfo();
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QLIBRARYINFO_H
