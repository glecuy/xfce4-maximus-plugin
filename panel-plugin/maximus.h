/*  $Id$
 *
 *  Copyright (C) 2014 - 2020 glecuyer
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef __MAXIMUS_H__
#define __MAXIMUS_H__

void MAXIMUS_Log( char *fmt, ... );

//#define MAXIMUS_Printf MAXIMUS_Log
#define MAXIMUS_Printf(fmt, ...) ((void)0)

#define WNCK_I_KNOW_THIS_IS_UNSTABLE
#include <libwnck/libwnck.h>

G_BEGIN_DECLS

typedef struct WindowIcon_s{
    WnckWindow * Window;
    GdkPixbuf * Pixbuf;
    GtkWidget * Image;
    GtkWidget * EventBox;
    void * Parent;
    struct WindowIcon_s * pNext;
} WindowIcon_t;

/* plugin structure */
typedef struct
{
    XfcePanelPlugin *plugin;

    /* panel widgets */
    GtkWidget       *ebox;
    GtkWidget       *hvbox;
    GtkWidget       *label;
    GtkWidget       *hIconBox;
    GtkWidget       *WinTitle;
    GtkWidget       *CloseButton;

    /* maximus settings TODO */
    gchar           *setting1;
    gint             setting2;
    gboolean         setting3;

    /* Maximus objects */
    WnckScreen * ActiveScreen;
    WnckWindow * MaximizedWindow;
    gulong maximized_xid;

    WindowIcon_t *IconList;

    GSList * exclude_class_list;
}
MaximusPlugin;

// Data to be attached to window
#define MAXIMUSED_NULL 0
#define MAXIMUSED_ON   1
#define MAXIMUSED_DECORATED 2


void mxs_OnClickedCloseWindow(GtkWidget *widget, GdkEventButton *event, MaximusPlugin *maximus);

void
maximus_save (XfcePanelPlugin *plugin,
             MaximusPlugin    *maximus);

G_END_DECLS

#endif /* !__MAXIMUS_H__ */
