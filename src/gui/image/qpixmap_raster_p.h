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

#ifndef QPIXMAPDATA_RASTER_P_H
#define QPIXMAPDATA_RASTER_P_H

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

#include <qpa/qplatformpixmap.h>


QT_BEGIN_NAMESPACE

class Q_GUI_EXPORT QRasterPlatformPixmap : public QPlatformPixmap
{
public:
    QRasterPlatformPixmap(PixelType type);
    ~QRasterPlatformPixmap();

    QPlatformPixmap *createCompatiblePlatformPixmap() const;

    void resize(int width, int height);
    bool fromData(const uchar *buffer, uint len, const char *format, Qt::ImageConversionFlags flags);
    void fromImage(const QImage &image, Qt::ImageConversionFlags flags);
    void fromImageReader(QImageReader *imageReader, Qt::ImageConversionFlags flags);

    void copy(const QPlatformPixmap *data, const QRect &rect);
    bool scroll(int dx, int dy, const QRect &rect);
    void fill(const QColor &color);
    bool hasAlphaChannel() const;
    QImage toImage() const;
    QImage toImage(const QRect &rect) const;
    QPaintEngine* paintEngine() const;
    QImage* buffer();

protected:
    int metric(QPaintDevice::PaintDeviceMetric metric) const;
    void createPixmapForImage(QImage &sourceImage, Qt::ImageConversionFlags flags, bool inPlace);
    void setImage(const QImage &image);
    QImage image;

private:
    friend class QPixmap;
    friend class QBitmap;
    friend class QPixmapCacheEntry;
    friend class QRasterPaintEngine;
};

QT_END_NAMESPACE

#endif // QPIXMAPDATA_RASTER_P_H


