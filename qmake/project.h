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

#ifndef PROJECT_H
#define PROJECT_H

#include <proitems.h>

#include <qstringlist.h>
#include <qtextstream.h>
#include <qstring.h>
#include <qstack.h>
#include <qhash.h>
#include <qmetatype.h>

QT_BEGIN_NAMESPACE

class QMakeProperty;

struct ParsableBlock;
struct IteratorBlock;
struct FunctionBlock;

class QMakeProject
{
    struct ScopeBlock
    {
        enum TestStatus { TestNone, TestFound, TestSeek };
        ScopeBlock() : iterate(0), ignore(false), else_status(TestNone) { }
        ScopeBlock(bool i) : iterate(0), ignore(i), else_status(TestNone) { }
        ~ScopeBlock();
        IteratorBlock *iterate;
        uint ignore : 1, else_status : 2;
    };
    friend struct ParsableBlock;
    friend struct IteratorBlock;
    friend struct FunctionBlock;

    QStack<ScopeBlock> scope_blocks;
    QStack<FunctionBlock *> function_blocks;
    IteratorBlock *iterator;
    FunctionBlock *function;
    QHash<QString, FunctionBlock*> testFunctions, replaceFunctions;

    bool host_build;
    bool need_restart;
    bool own_prop;
    bool backslashWarned;
    QString project_build_root;
    QString conffile;
    QString superfile;
    QString cachefile;
    QString real_spec, short_spec;
    QString pfile;
    QMakeProperty *prop;
    void reset();
    ProStringList extra_configs;
    ProValueMap extra_vars;
    QHash<QString, QStringList> vars, init_vars, base_vars;
    bool parse(const QString &text, QHash<QString, QStringList> &place, int line_count=1);

    enum IncludeStatus {
        IncludeSuccess,
        IncludeFeatureAlreadyLoaded,
        IncludeFailure,
        IncludeNoExist,
        IncludeParseFailure
    };
    enum IncludeFlags {
        IncludeFlagNone = 0x00,
        IncludeFlagFeature = 0x01,
        IncludeFlagNewParser = 0x02,
        IncludeFlagNewProject = 0x04
    };
    IncludeStatus doProjectInclude(QString file, uchar flags, QHash<QString, QStringList> &place);

    bool doProjectCheckReqs(const QStringList &deps, QHash<QString, QStringList> &place);
    bool doVariableReplace(QString &str, QHash<QString, QStringList> &place);
    QStringList doVariableReplaceExpand(const QString &str, QHash<QString, QStringList> &place, bool *ok=0);
    void init(QMakeProperty *);
    void cleanup();
    void loadDefaults();
    void setupProject();
    QStringList &values(const QString &v, QHash<QString, QStringList> &place);
    QStringList magicValues(const QString &v, const QHash<QString, QStringList> &place) const;
    QStringList qmakeFeaturePaths();

public:
    QMakeProject(QMakeProperty *p = 0) { init(p); }
    QMakeProject(QMakeProject *p, const QHash<QString, QStringList> *nvars=0);
    ~QMakeProject();

    void setExtraVars(const ProValueMap &_vars) { extra_vars = _vars; }
    void setExtraConfigs(const ProStringList &_cfgs) { extra_configs = _cfgs; }

    enum { ReadProFile=0x01, ReadSetup=0x02, ReadFeatures=0x04, ReadAll=0xFF };
    inline bool parse(const QString &text) { return parse(text, vars); }
    bool read(const QString &project, uchar cmd=ReadAll);
    bool read(uchar cmd=ReadAll);

    QStringList userExpandFunctions() { return replaceFunctions.keys(); }
    QStringList userTestFunctions() { return testFunctions.keys(); }

    QString projectFile();
    QString buildRoot() const { return project_build_root; }
    QString confFile() const { return conffile; }
    QString cacheFile() const { return cachefile; }
    QString specDir() const { return real_spec; }
    inline QMakeProperty *properties() { return prop; }

    bool doProjectTest(QString str, QHash<QString, QStringList> &place);
    bool doProjectTest(QString func, const QString &params,
                       QHash<QString, QStringList> &place);
    bool doProjectTest(QString func, QStringList args,
                       QHash<QString, QStringList> &place);
    bool doProjectTest(QString func, QList<QStringList> args,
                       QHash<QString, QStringList> &place);
    QStringList doProjectExpand(QString func, const QString &params,
                                QHash<QString, QStringList> &place);
    QStringList doProjectExpand(QString func, QStringList args,
                                QHash<QString, QStringList> &place);
    QStringList doProjectExpand(QString func, QList<QStringList> args,
                                QHash<QString, QStringList> &place);

    QStringList expand(const QString &v);
    QString expand(const QString &v, const QString &file, int line);
    QStringList expand(const ProKey &func, const QList<ProStringList> &args);
    bool test(const QString &v);
    bool test(const ProKey &func, const QList<ProStringList> &args);
    bool isActiveConfig(const QString &x, bool regex=false,
                        QHash<QString, QStringList> *place=NULL);

    bool isSet(const ProKey &v) const { return (*(const ProValueMap *)&vars).contains(v); }
    bool isEmpty(const ProKey &v) const;
    ProStringList values(const ProKey &v) const { return (*(const ProValueMap *)&vars)[v]; }
    ProStringList &values(const ProKey &v) { return (*(ProValueMap *)&vars)[v]; }
    ProString first(const ProKey &v) const;
    int intValue(const ProKey &v, int defaultValue = 0) const;
    const ProValueMap &variables() const { return *(const ProValueMap *)&vars; }
    ProValueMap &variables() { return *(ProValueMap *)&vars; }

    void dump() const;

    bool isHostBuild() const { return host_build; }

protected:
    friend class MakefileGenerator;
    bool read(const QString &file, QHash<QString, QStringList> &place);
    bool read(QTextStream &file, QHash<QString, QStringList> &place);

};
Q_DECLARE_METATYPE(QMakeProject*)

inline QString QMakeProject::projectFile()
{
    return pfile;
}

inline ProString QMakeProject::first(const ProKey &v) const
{
    const ProStringList &vals = values(v);
    if(vals.isEmpty())
        return ProString("");
    return vals.first();
}

inline int QMakeProject::intValue(const ProKey &v, int defaultValue) const
{
    const ProString &str = first(v);
    if (!str.isEmpty()) {
        bool ok;
        int i = str.toInt(&ok);
        if (ok)
            return i;
    }
    return defaultValue;
}

QT_END_NAMESPACE

#endif // PROJECT_H
