/******************************************************************************
* Copyright 2012  BogDan Vatra <bogdan@kde.org>                               *
*                                                                             *
* This library is free software; you can redistribute it and/or               *
* modify it under the terms of the GNU Lesser General Public                  *
* License as published by the Free Software Foundation; either                *
* version 2.1 of the License, or (at your option) version 3, or any           *
* later version accepted by the membership of KDE e.V. (or its                *
* successor approved by the membership of KDE e.V.), which shall              *
* act as a proxy defined in Section 6 of version 3 of the license.            *
*                                                                             *
* This library is distributed in the hope that it will be useful,             *
* but WITHOUT ANY WARRANTY; without even the implied warranty of              *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU           *
* Lesser General Public License for more details.                             *
*                                                                             *
* You should have received a copy of the GNU Lesser General Public            *
* License along with this library. If not, see <http://www.gnu.org/licenses/>.*
******************************************************************************/

#include "qandroidassetsfileenginehandler.h"
#include "androidjnimain.h"

#include <QCoreApplication>
#include <QDateTime>

class AndroidAbstractFileEngineIterator: public QAbstractFileEngineIterator
{
public:
    AndroidAbstractFileEngineIterator( QDir::Filters filters, const QStringList & nameFilters, AAssetDir* asset, const QString & path ):
        QAbstractFileEngineIterator(filters, nameFilters )
    {
        AAssetDir_rewind(asset);
        const char * fileName;
        while((fileName=AAssetDir_getNextFileName(asset)))
            m_items<<fileName;
        m_index = -1;
        m_path = path;
    }

    virtual QFileInfo currentFileInfo() const
    {
        return QFileInfo(currentFilePath());
    }

    virtual QString currentFileName() const
    {
        if (m_index < 0 || m_index >= m_items.size())
            return QString();
        return m_items[m_index];
    }

    virtual QString currentFilePath() const
    {
        return m_path+currentFileName();
    }

    virtual bool hasNext() const
    {
        return m_items.size()&&(m_index<m_items.size()-1);
    }

    virtual QString next()
    {
        if (!hasNext())
            return QString();
        m_index++;
        return currentFileName();
    }

private:
    QString     m_path;
    QStringList m_items;
    int         m_index;
};

class AndroidAbstractFileEngine : public QAbstractFileEngine
{
public:
    explicit AndroidAbstractFileEngine(AAsset* asset, const QString & fileName)
    {
        m_assetDir = 0;
        m_assetFile = asset;
        m_fileName =  fileName;
    }

    explicit AndroidAbstractFileEngine(AAssetDir* asset, const QString & fileName)
    {
        m_assetFile = 0;
        m_assetDir = asset;
        m_fileName =  fileName;
        if (!m_fileName.endsWith(QChar(QLatin1Char('/'))))
            m_fileName+="/";
    }

    ~AndroidAbstractFileEngine()
    {
        close();
        if (m_assetDir)
            AAssetDir_close(m_assetDir);
    }

    virtual bool open(QIODevice::OpenMode openMode)
    {
        if (m_assetFile)
            return openMode&QIODevice::ReadOnly;
        return false;
    }

    virtual bool close()
    {
        if (m_assetFile)
        {
            AAsset_close(m_assetFile);
            m_assetFile = 0;
            return true;
        }
        return false;
    }

    virtual qint64 size() const
    {
        if (m_assetFile)
            return AAsset_getLength(m_assetFile);
        return -1;
    }

    virtual qint64 pos() const
    {
        if (m_assetFile)
            return AAsset_seek(m_assetFile, 0, SEEK_CUR);
        return -1;
    }

    virtual bool seek(qint64 pos)
    {
        if (m_assetFile)
            return pos==AAsset_seek(m_assetFile, pos, SEEK_SET);
        return false;
    }

    virtual qint64 read(char *data, qint64 maxlen)
    {
        if (m_assetFile)
            return AAsset_read(m_assetFile, data, maxlen);
        return -1;
    }

    virtual bool isSequential() const
    {
        return false;
    }

    virtual bool caseSensitive() const
    {
        return true;
    }

    virtual bool isRelativePath() const
    {
        return false;
    }

    virtual FileFlags fileFlags(FileFlags type=FileInfoAll) const
    {
        FileFlags flags(ReadOwnerPerm|ReadUserPerm|ReadGroupPerm|ReadOtherPerm|ExistsFlag);
        if (m_assetFile)
            flags|=FileType;
        if(m_assetDir)
            flags|=DirectoryType;
        return type&flags;
    }

    virtual QString fileName(FileName file=DefaultName) const
    {
        int pos;
        switch(file)
        {
        case DefaultName:
        case AbsoluteName:
        case CanonicalName:
                return m_fileName;
        case BaseName:
            if((pos=m_fileName.lastIndexOf(QChar(QLatin1Char('/'))))!=-1)
                return m_fileName.mid(pos);
            else
                return m_fileName;
        case PathName:
        case AbsolutePathName:
        case CanonicalPathName:
            if((pos=m_fileName.lastIndexOf(QChar(QLatin1Char('/'))))!=-1)
                return m_fileName.left(pos);
            else
                return m_fileName;
        default:
            return QString();
        }
    }

    virtual void setFileName(const QString &file)
    {
        if ( file == m_fileName )
            return;
        m_fileName = file;
        if (!m_fileName.endsWith(QChar(QLatin1Char('/'))))
            m_fileName+="/";
        close();
    }

    virtual Iterator *beginEntryList(QDir::Filters filters, const QStringList &filterNames)
    {
        if (m_assetDir)
            return new AndroidAbstractFileEngineIterator(filters, filterNames, m_assetDir, m_fileName);
        return 0;
    }

private:
    AAsset* m_assetFile;
    AAssetDir* m_assetDir;
    QString m_fileName;
};


AndroidAssetsFileEngineHandler::AndroidAssetsFileEngineHandler()
{
    m_assetManager = QtAndroid::assetManager();
    m_necessitasApiLevel = -1;
}

AndroidAssetsFileEngineHandler::~AndroidAssetsFileEngineHandler()
{

}

QAbstractFileEngine * AndroidAssetsFileEngineHandler::create ( const QString & fileName ) const
{
    if(fileName.isEmpty())
        return 0;
    if (-1 == m_necessitasApiLevel)
        m_necessitasApiLevel = qgetenv("NECESSITAS_API_LEVEL").toInt();

    if (m_necessitasApiLevel>1)
    {
        if (!fileName.startsWith(QLatin1String("assets:/")))
            return 0;

        int prefixSize=8;

        path.clear();
        if (!fileName.endsWith(QLatin1Char('/')))
        {
            path = fileName.toUtf8();
            AAsset* asset=AAssetManager_open(m_assetManager, path.constData()+prefixSize, AASSET_MODE_BUFFER);
            if (asset)
                return new AndroidAbstractFileEngine(asset, fileName);
        }

        if (!path.size())
             path = fileName.left(fileName.length()-1).toUtf8();

        AAssetDir* assetDir=AAssetManager_openDir(m_assetManager, path.constData()+prefixSize);
        if (assetDir)
        {
            if (AAssetDir_getNextFileName(assetDir))
                return new AndroidAbstractFileEngine(assetDir, fileName);
            else
                AAssetDir_close(assetDir);
        }
    }
    else
    {
        AAsset* asset;
        if (fileName[0]==QChar(QLatin1Char('/')))
            asset=AAssetManager_open(m_assetManager, fileName.toUtf8().constData()+1, AASSET_MODE_BUFFER);
        else
        {
            if (fileName.startsWith(QLatin1String("file://")))
                asset=AAssetManager_open(m_assetManager, fileName.toUtf8().constData()+7, AASSET_MODE_BUFFER);
            else
                asset=AAssetManager_open(m_assetManager, fileName.toUtf8().constData(), AASSET_MODE_BUFFER);
        }
        if (asset)
            return new AndroidAbstractFileEngine(asset, fileName);

        if (!fileName.endsWith(QChar(QLatin1Char('/'))))
            return 0;
        QString dirName;
        if (fileName[0]==QChar(QLatin1Char('/')))
            dirName = fileName.mid(1, fileName.size()-2);
        else
        {
            if (fileName.startsWith(QLatin1String("file://")))
                dirName = fileName.mid(7,fileName.size()-8);
            else
                dirName = fileName.left(fileName.size()-1);
        }

        AAssetDir* assetDir;
        assetDir=AAssetManager_openDir(m_assetManager, dirName.toUtf8().constData());
        if (assetDir)
        {
            if (AAssetDir_getNextFileName(assetDir))
                return new AndroidAbstractFileEngine(assetDir, fileName);
            else
                AAssetDir_close(assetDir);
        }
    }
    return 0;
}
