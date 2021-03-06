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

#ifndef OPTION_H
#define OPTION_H

#include "project.h"
#include <qstring.h>
#include <qstringlist.h>
#include <qfile.h>
#include <qlibraryinfo.h>

QT_BEGIN_NAMESPACE

#define QMAKE_VERSION_MAJOR 2
#define QMAKE_VERSION_MINOR 1
#define QMAKE_VERSION_PATCH 0
const char *qmake_version();

QString qmake_getpwd();
bool qmake_setpwd(const QString &p);

#define debug_msg if(Option::debug_level) debug_msg_internal
void debug_msg_internal(int level, const char *fmt, ...); //don't call directly, use debug_msg
enum QMakeWarn {
    WarnNone    = 0x00,
    WarnParser  = 0x01,
    WarnLogic   = 0x02,
    WarnDeprecated = 0x04,
    WarnAll     = 0xFF
};
void warn_msg(QMakeWarn t, const char *fmt, ...);

struct Option
{
    //simply global convenience
    static QString libtool_ext;
    static QString pkgcfg_ext;
    static QString prf_ext;
    static QString prl_ext;
    static QString ui_ext;
    static QStringList h_ext;
    static QStringList cpp_ext;
    static QStringList c_ext;
    static QString cpp_moc_ext;
    static QString obj_ext;
    static QString lex_ext;
    static QString yacc_ext;
    static QString h_moc_mod;
    static QString lex_mod;
    static QString yacc_mod;
    static QString dir_sep;
    static QString dirlist_sep;
    static QString pro_ext;
    static QString res_ext;
    static char field_sep;
    static const char *application_argv0;

    enum CmdLineFlags {
        QMAKE_CMDLINE_SUCCESS       = 0x00,
        QMAKE_CMDLINE_SHOW_USAGE    = 0x01,
        QMAKE_CMDLINE_BAIL          = 0x02,
        QMAKE_CMDLINE_ERROR         = 0x04
    };

    //both of these must be called..
    static int init(int argc=0, char **argv=0); //parse cmdline
    static void prepareProject(const QString &pfile);
    static bool postProcessProject(QMakeProject *);

    enum StringFixFlags {
        FixNone                 = 0x00,
        FixEnvVars              = 0x01,
        FixPathCanonicalize     = 0x02,
        FixPathToLocalSeparators  = 0x04,
        FixPathToTargetSeparators = 0x08,
        FixPathToNormalSeparators = 0x10
    };
    static QString fixString(QString string, uchar flags);

    //and convenience functions
    inline static QString fixPathToLocalOS(const QString &in, bool fix_env=true, bool canonical=true)
    {
        uchar flags = FixPathToLocalSeparators;
        if(fix_env)
            flags |= FixEnvVars;
        if(canonical)
            flags |= FixPathCanonicalize;
        return fixString(in, flags);
    }
    inline static QString fixPathToTargetOS(const QString &in, bool fix_env=true, bool canonical=true)
    {
        uchar flags = FixPathToTargetSeparators;
        if(fix_env)
            flags |= FixEnvVars;
        if(canonical)
            flags |= FixPathCanonicalize;
        return fixString(in, flags);
    }
    inline static QString normalizePath(const QString &in, bool fix_env=true, bool canonical=true)
    {
        uchar flags = FixPathToNormalSeparators;
        if (fix_env)
            flags |= FixEnvVars;
        if (canonical)
            flags |= FixPathCanonicalize;
        return fixString(in, flags);
    }

    inline static bool hasFileExtension(const QString &str, const QStringList &extensions)
    {
        foreach (const QString &ext, extensions)
            if (str.endsWith(ext))
                return true;
        return false;
    }

    //global qmake mode, can only be in one mode per invocation!
    enum QMAKE_MODE { QMAKE_GENERATE_NOTHING,
                      QMAKE_GENERATE_PROJECT, QMAKE_GENERATE_MAKEFILE, QMAKE_GENERATE_PRL,
                      QMAKE_SET_PROPERTY, QMAKE_UNSET_PROPERTY, QMAKE_QUERY_PROPERTY };
    static QMAKE_MODE qmake_mode;

    //all modes
    static QString qmake_abslocation;
    static QStringList qmake_args;
    static QFile output;
    static QString output_dir;
    static int debug_level;
    static int warn_level;
    static bool recursive;
    static QStringList before_user_vars, after_user_vars;
    static QString user_template, user_template_prefix;

    //QMAKE_*_PROPERTY options
    struct prop {
        static QStringList properties;
    };

    //QMAKE_GENERATE_PROJECT options
    struct projfile {
        static bool do_pwd;
        static QStringList project_dirs;
    };

    //QMAKE_GENERATE_MAKEFILE options
    struct mkfile {
        static QString qmakespec;
        static QString xqmakespec;
        static bool do_cache;
        static bool do_deps;
        static bool do_mocs;
        static bool do_dep_heuristics;
        static bool do_preprocess;
        static bool do_stub_makefile;
        static QString source_root;
        static QString build_root;
        static QString cachefile;
        static int cachefile_depth;
        static QStringList project_files;
    };

private:
    static int parseCommandLine(QStringList &args);
};

inline QString fixEnvVariables(const QString &x) { return Option::fixString(x, Option::FixEnvVars); }
inline QStringList splitPathList(const QString &paths) { return paths.isEmpty() ? QStringList() : paths.split(Option::dirlist_sep); }

QT_END_NAMESPACE

#endif // OPTION_H
