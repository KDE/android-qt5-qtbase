/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
**
** This file is part of the qmake application of the Qt Toolkit.
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

#ifndef WINMAKEFILE_H
#define WINMAKEFILE_H

#include "makefile.h"

QT_BEGIN_NAMESPACE

class Win32MakefileGenerator : public MakefileGenerator
{
public:
    Win32MakefileGenerator();
    ~Win32MakefileGenerator();
protected:
    virtual QString defaultInstall(const QString &);
    virtual void writeCleanParts(QTextStream &t);
    virtual void writeStandardParts(QTextStream &t);
    virtual void writeIncPart(QTextStream &t);
    virtual void writeLibsPart(QTextStream &t);
    virtual void writeObjectsPart(QTextStream &t);
    virtual void writeImplicitRulesPart(QTextStream &t);
    virtual void writeBuildRulesPart(QTextStream &);
    virtual QString escapeFilePath(const QString &path) const;
    ProString escapeFilePath(const ProString &path) const { return MakefileGenerator::escapeFilePath(path); }

    virtual void writeRcFilePart(QTextStream &t);

    int findHighestVersion(const QString &dir, const QString &stem, const QString &ext = QLatin1String("lib"));
    virtual bool findLibraries();

    virtual void processPrlFiles();
    void processVars();
    void fixTargetExt();
    void processRcFileVar();
    virtual QString getLibTarget();
};

inline Win32MakefileGenerator::~Win32MakefileGenerator()
{ }

QT_END_NAMESPACE

#endif // WINMAKEFILE_H
