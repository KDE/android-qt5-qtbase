/******************************************************************************
* Copyright 2012  BogDan Vatra <bog_dan_ro@yahoo.com>                         *
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

package org.kde.necessitas.industrius;

import android.app.Activity;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.PixelFormat;
import android.graphics.Rect;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.MotionEvent;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

public class QtSurface extends SurfaceView implements SurfaceHolder.Callback
{
    private Bitmap m_bitmap=null;
    private boolean m_started = false;
    private boolean m_usesGL = false;
    public QtSurface(Context context, int id)
    {
        super(context);
        setFocusable(true);
        getHolder().addCallback(this);
        getHolder().setType(SurfaceHolder.SURFACE_TYPE_GPU);
        setId(id);
    }

    public void applicationStarted(boolean usesGL)
    {
        m_started = true;
        m_usesGL = usesGL;
        if (getWidth() < 1 ||  getHeight() < 1)
            return;
        if (m_usesGL)
            QtNative.setSurface(getHolder().getSurface());
        else
        {
            QtNative.lockSurface();
            QtNative.setSurface(null);
            m_bitmap=Bitmap.createBitmap(getWidth(), getHeight(), Bitmap.Config.RGB_565);
            QtNative.setSurface(m_bitmap);
            QtNative.unlockSurface();
        }
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder)
    {
        DisplayMetrics metrics = new DisplayMetrics();
        ((Activity) getContext()).getWindowManager().getDefaultDisplay().getMetrics(metrics);
        QtNative.setApplicationDisplayMetrics(metrics.widthPixels,
            metrics.heightPixels, getWidth(), getHeight(), metrics.xdpi, metrics.ydpi);

        if(m_usesGL)
            holder.setFormat(PixelFormat.RGBA_8888);
        else
            holder.setFormat(PixelFormat.RGB_565);

//        if (!m_started)
//            return;
//
//        if (m_usesGL)
//            QtNative.setSurface(holder.getSurface());
//        else
//        {
//            QtNative.lockSurface();
//            QtNative.setSurface(null);
//            m_bitmap=Bitmap.createBitmap(getWidth(), getHeight(), Bitmap.Config.RGB_565);
//            QtNative.setSurface(m_bitmap);
//            QtNative.unlockSurface();
//        }
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height)
    {
        Log.i(QtNative.QtTAG,"surfaceChanged: "+width+","+height);
        if (width<1 || height<1)
                return;

        DisplayMetrics metrics = new DisplayMetrics();
        ((Activity) getContext()).getWindowManager().getDefaultDisplay().getMetrics(metrics);
        QtNative.setApplicationDisplayMetrics(metrics.widthPixels,
            metrics.heightPixels, width, height, metrics.xdpi, metrics.ydpi);

        if (!m_started)
            return;

        if (m_usesGL)
            QtNative.setSurface(holder.getSurface());
        else
        {
            QtNative.lockSurface();
            QtNative.setSurface(null);
            m_bitmap=Bitmap.createBitmap(width, height, Bitmap.Config.RGB_565);
            QtNative.setSurface(m_bitmap);
            QtNative.unlockSurface();
            QtNative.updateWindow();
        }
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder)
    {
        Log.i(QtNative.QtTAG,"surfaceDestroyed ");
        if (m_usesGL)
            QtNative.destroySurface();
        else
        {
            if (!m_started)
                return;

            QtNative.lockSurface();
            QtNative.setSurface(null);
            QtNative.unlockSurface();
        }
    }

    public void drawBitmap(Rect rect)
    {
        if (!m_started)
            return;
        QtNative.lockSurface();
        if (null!=m_bitmap)
        {
            try
            {
                Canvas cv=getHolder().lockCanvas(rect);
                cv.drawBitmap(m_bitmap, rect, rect, null);
                getHolder().unlockCanvasAndPost(cv);
            }
            catch (Exception e)
            {
                Log.e(QtNative.QtTAG, "Can't create main activity", e);
            }
        }
        QtNative.unlockSurface();
    }

    @Override
    public boolean onTouchEvent(MotionEvent event)
    {
        if (!m_started)
            return false;
        QtNative.sendTouchEvent(event, getId());
        return true;
    }

    @Override
    public boolean onTrackballEvent(MotionEvent event)
    {
        if (!m_started)
            return false;
        QtNative.sendTrackballEvent(event, getId());
        return true;
    }
}
