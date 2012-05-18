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

#include "qwindowsinternalmimedata.h"
#include "qwindowscontext.h"
#include "qplatformfunctions_wince.h"
#include "qwindowsmime.h"
#include <QDebug>
/*!
    \class QWindowsInternalMimeDataBase
    \brief Base for implementations of QInternalMimeData using a IDataObject COM object.

    In clipboard handling and Drag and drop, static instances
    of QInternalMimeData implementations are kept and passed to the client.

    QInternalMimeData provides virtuals that query the formats and retrieve
    mime data on demand when the client invokes functions like QMimeData::hasHtml(),
    QMimeData::html() on the instance returned. Otherwise, expensive
    construction of a new QMimeData object containing all possible
    formats would have to be done in each call to mimeData().

    The base class introduces new virtuals to obtain and release
    the instances IDataObject from the clipboard or Drag and Drop and
    does conversion using QWindowsMime classes.

    \sa QInternalMimeData, QWindowsMime, QWindowsMimeConverter
    \ingroup qt-lighthouse-win
*/

bool QWindowsInternalMimeData::hasFormat_sys(const QString &mime) const
{
    IDataObject *pDataObj = retrieveDataObject();
    if (!pDataObj)
        return false;

    const QWindowsMimeConverter &mc = QWindowsContext::instance()->mimeConverter();
    const bool has = mc.converterToMime(mime, pDataObj) != 0;
    releaseDataObject(pDataObj);
    if (QWindowsContext::verboseOLE)
        qDebug() << __FUNCTION__ <<  mime << has;
    return has;
}

QStringList QWindowsInternalMimeData::formats_sys() const
{
    IDataObject *pDataObj = retrieveDataObject();
    if (!pDataObj)
        return QStringList();

    const QWindowsMimeConverter &mc = QWindowsContext::instance()->mimeConverter();
    const QStringList fmts = mc.allMimesForFormats(pDataObj);
    releaseDataObject(pDataObj);
    if (QWindowsContext::verboseOLE)
        qDebug() << __FUNCTION__ <<  fmts;
    return fmts;
}

QVariant QWindowsInternalMimeData::retrieveData_sys(const QString &mimeType,
                                                        QVariant::Type type) const
{
    IDataObject *pDataObj = retrieveDataObject();
    if (!pDataObj)
        return QVariant();

    QVariant result;
    const QWindowsMimeConverter &mc = QWindowsContext::instance()->mimeConverter();
    if (const QWindowsMime *converter = mc.converterToMime(mimeType, pDataObj))
        result = converter->convertToMime(mimeType, pDataObj, type);
    releaseDataObject(pDataObj);
    if (QWindowsContext::verboseOLE) {
        QDebug nospace = qDebug().nospace();
        nospace << __FUNCTION__ <<  ' '  << mimeType << ' ' << type
                << " returns " << result.type();
        if (result.type() != QVariant::ByteArray)
            nospace << ' ' << result;
    }
    return result;
}