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

#include "qdir.h"
#include "qfile.h"
#include "qconfig.h"
#include "qsettings.h"
#include "qlibraryinfo.h"
#include "qscopedpointer.h"

#ifdef QT_BUILD_QMAKE
QT_BEGIN_NAMESPACE
extern QString qmake_libraryInfoFile();
QT_END_NAMESPACE
#else
# include "qcoreapplication.h"
#endif

#ifdef Q_OS_MAC
#  include "private/qcore_mac_p.h"
#endif

#include "qconfig.cpp"

QT_BEGIN_NAMESPACE

extern void qDumpCPUFeatures(); // in qsimd.cpp

#ifndef QT_NO_SETTINGS

struct QLibrarySettings
{
    QLibrarySettings();
    QScopedPointer<QSettings> settings;
#ifdef QT_BUILD_QMAKE
    bool haveEffectivePaths;
    bool havePaths;
#endif
};
Q_GLOBAL_STATIC(QLibrarySettings, qt_library_settings)

class QLibraryInfoPrivate
{
public:
    static QSettings *findConfiguration();
#ifndef QT_BUILD_QMAKE
    static void cleanup()
    {
        QLibrarySettings *ls = qt_library_settings();
        if (ls)
            ls->settings.reset(0);
    }
#else
    static bool haveGroup(QLibraryInfo::PathGroup group)
    {
        QLibrarySettings *ls = qt_library_settings();
        return ls ? (group == QLibraryInfo::EffectivePaths
                     ? ls->haveEffectivePaths : ls->havePaths) : false;
    }
#endif
    static QSettings *configuration()
    {
        QLibrarySettings *ls = qt_library_settings();
        return ls ? ls->settings.data() : 0;
    }
};

QLibrarySettings::QLibrarySettings()
    : settings(QLibraryInfoPrivate::findConfiguration())
{
#ifndef QT_BUILD_QMAKE
    qAddPostRoutine(QLibraryInfoPrivate::cleanup);
    bool haveEffectivePaths;
    bool havePaths;
#endif
    if (settings) {
        // This code needs to be in the regular library, as otherwise a qt.conf that
        // works for qmake would break things for dynamically built Qt tools.
        QStringList children = settings->childGroups();
        haveEffectivePaths = children.contains(QLatin1String("EffectivePaths"));
        // Backwards compat: an existing but empty file is claimed to contain the Paths section.
        havePaths = !haveEffectivePaths || children.contains(QLatin1String("Paths"));
#ifndef QT_BUILD_QMAKE
        if (!havePaths)
            settings.reset(0);
#else
    } else {
        haveEffectivePaths = false;
        havePaths = false;
#endif
    }
}

QSettings *QLibraryInfoPrivate::findConfiguration()
{
    QString qtconfig = QStringLiteral(":/qt/etc/qt.conf");
#ifdef QT_BUILD_QMAKE
    if(!QFile::exists(qtconfig))
        qtconfig = qmake_libraryInfoFile();
#else
    if (!QFile::exists(qtconfig) && QCoreApplication::instance()) {
#ifdef Q_OS_MAC
	CFBundleRef bundleRef = CFBundleGetMainBundle();
        if (bundleRef) {
	    QCFType<CFURLRef> urlRef = CFBundleCopyResourceURL(bundleRef,
							       QCFString(QLatin1String("qt.conf")),
							       0,
							       0);
	    if (urlRef) {
	        QCFString path = CFURLCopyFileSystemPath(urlRef, kCFURLPOSIXPathStyle);
		qtconfig = QDir::cleanPath(path);
	    }
	}
	if (qtconfig.isEmpty())
#endif
            {
                QDir pwd(QCoreApplication::applicationDirPath());
                qtconfig = pwd.filePath(QLatin1String("qt.conf"));
	    }
    }
#endif
    if (QFile::exists(qtconfig))
        return new QSettings(qtconfig, QSettings::IniFormat);
    return 0;     //no luck
}

#endif // QT_NO_SETTINGS

/*!
    \class QLibraryInfo
    \inmodule QtCore
    \brief The QLibraryInfo class provides information about the Qt library.

    Many pieces of information are established when Qt is configured and built.
    This class provides an abstraction for accessing that information.
    By using the static functions of this class, an application can obtain
    information about the instance of the Qt library which the application
    is using at run-time.

    You can also use a \c qt.conf file to override the hard-coded paths
    that are compiled into the Qt library. For more information, see
    the \l {Using qt.conf} documentation.

    \sa QSysInfo, {Using qt.conf}
*/

#ifndef QT_BUILD_QMAKE

/*!
    \internal

   You cannot create a QLibraryInfo, instead only the static functions are available to query
   information.
*/

QLibraryInfo::QLibraryInfo()
{ }

/*!
  Returns the person to whom this build of Qt is licensed.

  \sa licensedProducts()
*/

QString
QLibraryInfo::licensee()
{
    const char *str = QT_CONFIGURE_LICENSEE;
    return QString::fromLocal8Bit(str);
}

/*!
  Returns the products that the license for this build of Qt has access to.

  \sa licensee()
*/

QString
QLibraryInfo::licensedProducts()
{
    const char *str = QT_CONFIGURE_LICENSED_PRODUCTS;
    return QString::fromLatin1(str);
}

/*!
    \since 4.6
    Returns the installation date for this build of Qt. The install date will
    usually be the last time that Qt sources were configured.
*/
#ifndef QT_NO_DATESTRING
QDate
QLibraryInfo::buildDate()
{
    return QDate::fromString(QString::fromLatin1(qt_configure_installation + 12), Qt::ISODate);
}
#endif //QT_NO_DATESTRING

/*!
    \since 5.0
    Returns true if this build of Qt was built with debugging enabled, or
    false if it was built in release mode.
*/
bool
QLibraryInfo::isDebugBuild()
{
#ifdef QT_DEBUG
    return true;
#endif
    return false;
}

#endif // QT_BUILD_QMAKE

static const struct {
    char key[14], value[13];
} qtConfEntries[] = {
    { "Prefix", "." },
    { "Documentation", "doc" },
    { "Headers", "include" },
    { "Libraries", "lib" },
    { "Binaries", "bin" },
    { "Plugins", "plugins" },
    { "Imports", "imports" },
    { "Data", "." },
    { "Translations", "translations" },
    { "Examples", "examples" },
    { "Tests", "tests" },
#ifdef QT_BUILD_QMAKE
    { "Sysroot", "" },
    { "HostPrefix", "" },
    { "HostBinaries", "bin" },
    { "HostData", "." },
#endif
};

/*!
  Returns the location specified by \a loc.

*/
QString
QLibraryInfo::location(LibraryLocation loc)
{
#ifdef QT_BUILD_QMAKE
    QString ret = rawLocation(loc, FinalPaths);

    // Automatically prepend the sysroot to target paths
    if (loc < SysrootPath || loc > LastHostPath) {
        QString sysroot = rawLocation(SysrootPath, FinalPaths);
        if (!sysroot.isEmpty() && ret.length() > 2 && ret.at(1) == QLatin1Char(':')
            && (ret.at(2) == QLatin1Char('/') || ret.at(2) == QLatin1Char('\\')))
            ret.replace(0, 2, sysroot); // Strip out the drive on Windows targets
        else
            ret.prepend(sysroot);
    }

    return ret;
}

QString
QLibraryInfo::rawLocation(LibraryLocation loc, PathGroup group)
{
#else
# define rawLocation(loca, group) location(loca)
# define group dummy
#endif
    QString ret;
#ifdef QT_BUILD_QMAKE
    // Logic for choosing the right data source: if EffectivePaths are requested
    // and qt.conf with that section is present, use it, otherwise fall back to
    // FinalPaths. For FinalPaths, use qt.conf if present and contains not only
    // [EffectivePaths], otherwise fall back to builtins.
    if (!QLibraryInfoPrivate::haveGroup(group)
        && (group == FinalPaths
            || !(group = FinalPaths, QLibraryInfoPrivate::haveGroup(FinalPaths))))
#elif !defined(QT_NO_SETTINGS)
    if (!QLibraryInfoPrivate::configuration())
#endif
    {
        const char *path = 0;
        if (loc >= 0 && loc < sizeof(qt_configure_prefix_path_strs)/sizeof(qt_configure_prefix_path_strs[0]))
            path = qt_configure_prefix_path_strs[loc] + 12;
#ifndef Q_OS_WIN // On Windows we use the registry
        else if (loc == SettingsPath)
            path = QT_CONFIGURE_SETTINGS_PATH;
#endif

        if (path)
            ret = QString::fromLocal8Bit(path);
#ifndef QT_NO_SETTINGS
    } else {
        QString key;
        QString defaultValue;
        if (loc >= 0 && loc < sizeof(qtConfEntries)/sizeof(qtConfEntries[0])) {
            key = QLatin1String(qtConfEntries[loc].key);
            defaultValue = QLatin1String(qtConfEntries[loc].value);
        }
#ifndef Q_OS_WIN // On Windows we use the registry
        else if (loc == SettingsPath)
            key = QLatin1String("Settings");
#endif

        if(!key.isNull()) {
            QSettings *config = QLibraryInfoPrivate::configuration();
            config->beginGroup(QLatin1String(
#ifdef QT_BUILD_QMAKE
                   group == EffectivePaths ? "EffectivePaths" :
#endif
                                             "Paths"));

            ret = config->value(key, defaultValue).toString();

#ifdef QT_BUILD_QMAKE
            if (ret.isEmpty() && loc == HostPrefixPath)
                ret = config->value(QLatin1String(qtConfEntries[PrefixPath].key),
                                    QLatin1String(qtConfEntries[PrefixPath].value)).toString();
#endif

            // expand environment variables in the form $(ENVVAR)
            int rep;
            QRegExp reg_var(QLatin1String("\\$\\(.*\\)"));
            reg_var.setMinimal(true);
            while((rep = reg_var.indexIn(ret)) != -1) {
                ret.replace(rep, reg_var.matchedLength(),
                            QString::fromLocal8Bit(qgetenv(ret.mid(rep + 2,
                                reg_var.matchedLength() - 3).toLatin1().constData()).constData()));
            }

            config->endGroup();

            ret = QDir::fromNativeSeparators(ret);
        }
#endif // QT_NO_SETTINGS
    }

    if (!ret.isEmpty() && QDir::isRelativePath(ret)) {
        QString baseDir;
#ifdef QT_BUILD_QMAKE
        if (loc == HostPrefixPath || loc == PrefixPath || loc == SysrootPath) {
            // We make the prefix/sysroot path absolute to the executable's directory.
            // loc == PrefixPath while a sysroot is set would make no sense here.
            // loc == SysrootPath only makes sense if qmake lives inside the sysroot itself.
            baseDir = QFileInfo(qmake_libraryInfoFile()).absolutePath();
        } else if (loc > SysrootPath && loc <= LastHostPath) {
            // We make any other host path absolute to the host prefix directory.
            baseDir = rawLocation(HostPrefixPath, group);
#else
        if (loc == PrefixPath) {
            if (QCoreApplication::instance()) {
#ifdef Q_OS_MAC
                CFBundleRef bundleRef = CFBundleGetMainBundle();
                if (bundleRef) {
                    QCFType<CFURLRef> urlRef = CFBundleCopyBundleURL(bundleRef);
                    if (urlRef) {
                        QCFString path = CFURLCopyFileSystemPath(urlRef, kCFURLPOSIXPathStyle);
                        return QDir::cleanPath(QString(path) + QLatin1String("/Contents/") + ret);
                    }
                }
#endif
                // We make the prefix path absolute to the executable's directory.
                baseDir = QCoreApplication::applicationDirPath();
            } else {
                baseDir = QDir::currentPath();
            }
#endif
        } else {
            // we make any other path absolute to the prefix directory
            baseDir = rawLocation(PrefixPath, group);
        }
        ret = QDir::cleanPath(baseDir + QLatin1Char('/') + ret);
    }
    return ret;
}

/*!
    \enum QLibraryInfo::LibraryLocation

    \keyword library location

    This enum type is used to specify a specific location
    specifier:

    \value PrefixPath The default prefix for all paths.
    \value DocumentationPath The location for documentation upon install.
    \value HeadersPath The location for all headers.
    \value LibrariesPath The location of installed libraries.
    \value BinariesPath The location of installed Qt binaries (tools and applications).
    \value PluginsPath The location of installed Qt plugins.
    \value ImportsPath The location of installed QML extensions to import.
    \value DataPath The location of general Qt data.
    \value TranslationsPath The location of translation information for Qt strings.
    \value ExamplesPath The location for examples upon install.
    \value TestsPath The location of installed Qt testcases.
    \value SettingsPath The location for Qt settings. Not applicable on Windows.

    \sa location()
*/

QT_END_NAMESPACE

#if defined(Q_CC_GNU) && defined(ELF_INTERPRETER)
#  include <stdio.h>
#  include <stdlib.h>

extern const char qt_core_interpreter[] __attribute__((section(".interp")))
    = ELF_INTERPRETER;

extern "C" void qt_core_boilerplate();
void qt_core_boilerplate()
{
    printf("This is the QtCore library version " QT_VERSION_STR "\n"
           "Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).\n"
           "Contact: http://www.qt-project.org/\n"
           "\n"
           "Build date:          %s\n"
           "Installation prefix: %s\n"
           "Library path:        %s\n"
           "Include path:        %s\n",
           qt_configure_installation + 12,
           qt_configure_prefix_path_strs[QT_PREPEND_NAMESPACE(QLibraryInfo)::PrefixPath] + 12,
           qt_configure_prefix_path_strs[QT_PREPEND_NAMESPACE(QLibraryInfo)::LibrariesPath] + 12,
           qt_configure_prefix_path_strs[QT_PREPEND_NAMESPACE(QLibraryInfo)::HeadersPath] + 12);

    QT_PREPEND_NAMESPACE(qDumpCPUFeatures)();

#ifdef QT_EVAL
    extern void qt_core_eval_init(uint);
    qt_core_eval_init(1);
#endif

    exit(0);
}

#endif
