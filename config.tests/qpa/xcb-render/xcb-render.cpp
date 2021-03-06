/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
**
** This file is part of the config.tests of the Qt Toolkit.
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

#include <xcb/xcb.h>
#include <xcb/render.h>

// 'template' is used as a function argument name in xcb_renderutil.h
#define template template_param
// extern "C" is missing too
extern "C" {
#include <xcb/xcb_renderutil.h>
}
#undef template

int main(int, char **)
{
    int primaryScreen = 0;
    xcb_generic_error_t *error = 0;

    xcb_connection_t *connection = xcb_connect("", &primaryScreen);
    xcb_render_query_pict_formats_cookie_t formatsCookie =
        xcb_render_query_pict_formats(connection);

    xcb_render_query_pict_formats_reply_t *formatsReply =
        xcb_render_query_pict_formats_reply(
                connection,
                formatsCookie,
                &error);

    xcb_render_util_find_standard_format(formatsReply,
            XCB_PICT_STANDARD_ARGB_32);

    return 0;
}
