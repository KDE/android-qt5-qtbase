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

#ifndef QPIXMAPDATA_P_H
#define QPIXMAPDATA_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtGui/qpixmap.h>
#include <QtCore/qatomic.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE


class QImageReader;

class Q_GUI_EXPORT QPlatformPixmap
{
public:
    enum PixelType {
        // WARNING: Do not change the first two
        // Must match QPixmap::Type
        PixmapType, BitmapType
    };

    enum ClassId { RasterClass, DirectFBClass,
                   BlitterClass, CustomClass = 1024 };

    QPlatformPixmap(PixelType pixelType, int classId);
    virtual ~QPlatformPixmap();

    virtual QPlatformPixmap *createCompatiblePlatformPixmap() const;

    virtual void resize(int width, int height) = 0;
    virtual void fromImage(const QImage &image,
                           Qt::ImageConversionFlags flags) = 0;
    virtual void fromImageReader(QImageReader *imageReader,
                                 Qt::ImageConversionFlags flags);

    virtual bool fromFile(const QString &filename, const char *format,
                          Qt::ImageConversionFlags flags);
    virtual bool fromData(const uchar *buffer, uint len, const char *format,
                          Qt::ImageConversionFlags flags);

    virtual void copy(const QPlatformPixmap *data, const QRect &rect);
    virtual bool scroll(int dx, int dy, const QRect &rect);

    virtual int metric(QPaintDevice::PaintDeviceMetric metric) const = 0;
    virtual void fill(const QColor &color) = 0;

    virtual bool hasAlphaChannel() const = 0;
    virtual QPixmap transformed(const QTransform &matrix,
                                Qt::TransformationMode mode) const;

    virtual QImage toImage() const = 0;
    virtual QImage toImage(const QRect &rect) const;
    virtual QPaintEngine* paintEngine() const = 0;

    inline int serialNumber() const { return ser_no; }

    inline PixelType pixelType() const { return type; }
    inline ClassId classId() const { return static_cast<ClassId>(id); }

    virtual QImage* buffer();

    inline int width() const { return w; }
    inline int height() const { return h; }
    inline int colorCount() const { return metric(QPaintDevice::PdmNumColors); }
    inline int depth() const { return d; }
    inline bool isNull() const { return is_null; }
    inline qint64 cacheKey() const {
        int classKey = id;
        if (classKey >= 1024)
            classKey = -(classKey >> 10);
        return ((((qint64) classKey) << 56)
                | (((qint64) ser_no) << 32)
                | ((qint64) detach_no));
    }

    static QPlatformPixmap *create(int w, int h, PixelType type);

protected:

    void setSerialNumber(int serNo);
    int w;
    int h;
    int d;
    bool is_null;

private:
    friend class QPixmap;
    friend class QImagePixmapCleanupHooks; // Needs to set is_cached
    friend class QOpenGLTextureCache; //Needs to check the reference count
    friend class QExplicitlySharedDataPointer<QPlatformPixmap>;

    QAtomicInt ref;
    int detach_no;

    PixelType type;
    int id;
    int ser_no;
    uint is_cached;
};

#  define QT_XFORM_TYPE_MSBFIRST 0
#  define QT_XFORM_TYPE_LSBFIRST 1
extern bool qt_xForm_helper(const QTransform&, int, int, int, uchar*, int, int, int, const uchar*, int, int, int);

QT_END_NAMESPACE

QT_END_HEADER

#endif // QPIXMAPDATA_P_H