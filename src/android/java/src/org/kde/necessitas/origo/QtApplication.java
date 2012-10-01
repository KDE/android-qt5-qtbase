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

package org.kde.necessitas.origo;

import java.lang.reflect.Field;
import java.lang.reflect.Method;
import java.util.ArrayList;
import java.util.HashMap;

import android.app.Application;

public class QtApplication extends Application
{
    public final static String QtTAG="Qt";
    public static Object m_delegateObject = null;
    public static HashMap<String, ArrayList<Method>> m_delegateMethods= new HashMap<String, ArrayList<Method>>();
    public static Method dispatchKeyEvent = null;
    public static Method dispatchPopulateAccessibilityEvent = null;
    public static Method dispatchTouchEvent = null;
    public static Method dispatchTrackballEvent = null;
    public static Method onKeyDown = null;
    public static Method onKeyMultiple = null;
    public static Method onKeyUp = null;
    public static Method onTouchEvent = null;
    public static Method onTrackballEvent = null;
    public static Method onActivityResult = null;
    public static Method onCreate = null;
    public static Method onKeyLongPress = null;
    public static Method dispatchKeyShortcutEvent = null;
    public static Method onKeyShortcut = null;
    public static Method dispatchGenericMotionEvent = null;
    public static Method onGenericMotionEvent = null;
/*
    public static Method onTerminate = null;
    public static Method onApplyThemeResource = null;
    public static Method onChildTitleChanged = null;
    public static Method onConfigurationChanged = null;
    public static Method onContentChanged  = null;
    public static Method onContextItemSelected  = null;
    public static Method onContextMenuClosed = null;
    public static Method onCreateContextMenu = null;
    public static Method onCreateDescription = null;
    public static Method onCreateDialog = null;
    public static Method onCreateOptionsMenu = null;
    public static Method onCreatePanelMenu = null;
    public static Method onCreatePanelView = null;
    public static Method onCreateThumbnail = null;
    public static Method onCreateView = null;
    public static Method onDestroy = null;
    public static Method onLowMemory = null;
    public static Method onMenuItemSelected = null;
    public static Method onMenuOpened = null;
    public static Method onNewIntent = null;
    public static Method onOptionsItemSelected = null;
    public static Method onOptionsMenuClosed = null;
    public static Method onPanelClosed = null;
    public static Method onPause = null;
    public static Method onPostCreate = null;
    public static Method onPostResume = null;
    public static Method onPrepareDialog = null;
    public static Method onPrepareOptionsMenu = null;
    public static Method onPreparePanel = null;
    public static Method onRestart = null;
    public static Method onRestoreInstanceState = null;
    public static Method onResume = null;
    public static Method onRetainNonConfigurationInstance = null;
    public static Method onSaveInstanceState = null;
    public static Method onSearchRequested = null;
    public static Method onStart = null;
    public static Method onStop = null;
    public static Method onTitleChanged = null;
    public static Method onUserInteraction = null;
    public static Method onUserLeaveHint = null;
    public static Method onWindowAttributesChanged = null;
    public static Method onWindowFocusChanged = null;
    public static Method onAttachedToWindow = null;
    public static Method onBackPressed = null;
    public static Method onDetachedFromWindow = null;
    public static Method onCreateDialog8 = null;
    public static Method onPrepareDialog8 = null;
    public static Method onActionModeFinished = null;
    public static Method onActionModeStarted = null;
    public static Method onAttachFragment = null;
    public static Method onCreateView11 = null;
    public static Method onWindowStartingActionMode = null;
*/

    public static void setQtActivityDelegate(Object listener)
    {
        QtApplication.m_delegateObject = listener;

        ArrayList<Method> delegateMethods = new ArrayList<Method>();
        for (Method m: listener.getClass().getMethods())
            if (m.getDeclaringClass().getName().startsWith("org.kde.necessitas"))
                delegateMethods.add(m);

        ArrayList<Field> applicationFields = new ArrayList<Field>();
        for (Field f: QtApplication.class.getFields())
            if (f.getDeclaringClass().getName().equals(QtApplication.class.getName()))
                applicationFields.add(f);

        for (Method delegateMethod:delegateMethods)
        {
            try {
                QtActivity.class.getDeclaredMethod(delegateMethod.getName(), delegateMethod.getParameterTypes());
                if (QtApplication.m_delegateMethods.containsKey(delegateMethod.getName()))
                    QtApplication.m_delegateMethods.get(delegateMethod.getName()).add(delegateMethod);
                else
                {
                    ArrayList<Method> delegateSet = new ArrayList<Method>();
                    delegateSet.add(delegateMethod);
                    QtApplication.m_delegateMethods.put(delegateMethod.getName(), delegateSet);
                }
                for(Field applicationField:applicationFields)
                {
                    if (applicationField.getName().equals(delegateMethod.getName()))
                    {
                        try {
                            applicationField.set(null, delegateMethod);
                        } catch (Exception e) {
                            e.printStackTrace();
                        }
                    }
                }
            }
            catch (Exception e)
            {
            }
        }
    }

    @Override
    public void onTerminate() {
        if (m_delegateObject != null && m_delegateMethods.containsKey("onTerminate"))
            invokeDelegateMethod(m_delegateMethods.get("onTerminate").get(0));
        super.onTerminate();
    }

    public static class InvokeResult
    {
        public boolean invoked = false;
        public Object methodReturns = null;
    }

    private static int stackDeep=-1;
    public static InvokeResult invokeDelegate(Object... args)
    {
        InvokeResult result = new InvokeResult();
        if (m_delegateObject==null)
            return result;
        StackTraceElement[] elements=Thread.currentThread().getStackTrace();
        if (-1 == stackDeep)
        {
            String activityClassName=QtActivity.class.getCanonicalName();
            for(int it=0;it<elements.length;it++)
                if (elements[it].getClassName().equals(activityClassName))
                {
                    stackDeep=it;
                    break;
                }
        }
        final String methodName=elements[stackDeep].getMethodName();
        if (-1 == stackDeep || !m_delegateMethods.containsKey(methodName))
            return result;

        for (Method m:m_delegateMethods.get(methodName))
            if (m.getParameterTypes().length == args.length)
            {
                result.methodReturns=invokeDelegateMethod(m, args);
                result.invoked=true;
                return result;
            }
        return result;
    }

    public static Object invokeDelegateMethod(Method m, Object... args)
    {
        try {
            return m.invoke(m_delegateObject, args);
        } catch (Exception e) {
            e.printStackTrace();
        }
        return null;
    }
}
