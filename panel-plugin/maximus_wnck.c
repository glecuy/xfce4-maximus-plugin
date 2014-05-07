/*  $Id$
 *
 *  Copyright (C) 2014 glecuy
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif
#include <malloc.h>

#include <gtk/gtk.h>

/* MXS_DBG_ONLY: hide xfce stuff for test and debug purpose within a simple gtk application */
#ifndef MXS_DBG_ONLY
#include <libxfce4util/libxfce4util.h>
#include <libxfce4panel/xfce-panel-plugin.h>
#include <libxfce4panel/xfce-hvbox.h>
#endif

#include "maximus.h"
#include "maximus_wnd.h"
#ifndef MXS_DBG_ONLY
#include "maximus-dialogs.h"
#endif

void MAXIMUS_Log( char *fmt, ... )
{
    static FILE * flog;

    if ( flog == NULL )
    {
        flog = fopen("/tmp/maximus.log", "at" );
        fprintf(flog, "maximus is starting...\n");
    }

    if ( flog != NULL )
    {
        va_list  VAList;
        va_start(VAList, fmt);
        vfprintf(flog, fmt, VAList);
        va_end(VAList);
    }
    fflush(flog);
}


static gboolean mxs_need_to_restore_decorations (MaximusPlugin *maximus, WnckWindow *window)
{
    int x, y;
    int h, w;

    if ( mxs_get_maximused(window) != 1 )
    {
        MAXIMUS_Printf ("Excluding (maximused): %s\n",wnck_window_get_name (window));
        return FALSE;
    }

    wnck_window_get_geometry(   window,
                                &x, &y,
                                &h, &w );

    MAXIMUS_Printf ("maximused win: %d, %d\n", x, y );
    if ( x > 100 && y > 100 )
    {
        return TRUE;
    }

    return FALSE;
}

/*
    menu containing items to manipulate a window.
    See wnck_action_menu_new()
*/
static void mxs_WindowMenuCreate( GtkWidget *widget, WnckWindow *window )
{
    GtkWidget *menu;
    menu = wnck_action_menu_new(window);

    gtk_menu_attach_to_widget(GTK_MENU(menu), widget, NULL);
    gtk_menu_popup (GTK_MENU(menu),
                        NULL,
                        NULL,
                        NULL,
                        NULL,
                        1,
                        gtk_get_current_event_time() );
}

static void mxs_OnClickedIcons(GtkWidget *widget, GdkEventButton *event, WindowIcon_t * pIcon)
{
    GList *window_l;
    WnckWindow *window;
    WnckScreen *screen;

    MAXIMUS_Printf("OnClickedIcons(Button%u)\n", event->button);

    screen = ((MaximusPlugin *)(pIcon->Parent))->ActiveScreen;
    for (window_l = wnck_screen_get_windows (screen); window_l != NULL; window_l = window_l->next)
    {
        window = WNCK_WINDOW (window_l->data);
        if ( window == pIcon->Window )
        {
            MAXIMUS_Printf ("Found %s Window\n", wnck_window_get_name (window) );
            if ( event->button == 1 )
            {
                wnck_window_activate (window, event->time );
            }
            else if ( event->button == 3 )
            {
                mxs_WindowMenuCreate( widget, window );
            }
        }
    }
}

WindowIcon_t * mxs_NewIconAdd( MaximusPlugin *maximus, WnckWindow * win )
{
    WindowIcon_t * pIcon;
    WindowIcon_t * It;

    pIcon = malloc( sizeof(WindowIcon_t) );

    if ( pIcon != NULL )
    {
        pIcon->Pixbuf = wnck_window_get_mini_icon(win);
        pIcon->Window = win;
        pIcon->Parent = maximus;
        pIcon->pNext = NULL;

        if ( maximus->IconList == NULL )
        {
            maximus->IconList = pIcon;
        }
        else
        {
            // Add this New icon at the end of the list
            It = maximus->IconList;
            while ( It->pNext )
            {
                It = It->pNext;
            }
            It->pNext = pIcon;
        }
        pIcon->Image = gtk_image_new();
        gtk_image_set_from_pixbuf(GTK_IMAGE(pIcon->Image), pIcon->Pixbuf);
        pIcon->EventBox = gtk_event_box_new ();
        gtk_container_add( GTK_CONTAINER(maximus->hIconBox), pIcon->EventBox);
        //gtk_widget_set_sensitive(pIcon->Image, TRUE);
        gtk_widget_set_tooltip_text (pIcon->EventBox, wnck_window_get_name(win));

        //gtk_container_add(GTK_CONTAINER (maximus->hIconBox), pIcon->Image);
        gtk_container_add (GTK_CONTAINER (pIcon->EventBox), pIcon->Image);
        //gtk_widget_show (pIcon->EventBox);
        gtk_widget_show_all(GTK_WIDGET(maximus->hIconBox));

        gtk_widget_add_events(pIcon->EventBox, GDK_BUTTON_PRESS_MASK);
        g_signal_connect(G_OBJECT(pIcon->EventBox), "button_press_event", G_CALLBACK(mxs_OnClickedIcons), pIcon);
    }
    return pIcon;
}


void on_mxs_window_closed(WnckScreen *screen, WnckWindow *window, MaximusPlugin *maximus)
{
    WindowIcon_t *It, *TmpIcon, *Prev;

    // Iter the Icon list
    It = maximus->IconList;
    Prev=NULL;
    while ( It )
    {
        if ( window == It->Window )
        {
            MAXIMUS_Printf ("Window Icon to delete\n" );
            //gtk_container_remove(GTK_CONTAINER (maximus->hIconBox), It->Image);
            gtk_widget_destroy(It->Image);
            gtk_widget_destroy(It->EventBox);
            gtk_widget_show_all(GTK_WIDGET(maximus->hIconBox));
            TmpIcon = It;
            free( It );
            It = TmpIcon->pNext;
            if ( Prev )
                Prev->pNext = It;
            else
                maximus->IconList = It;
            break;
        }
        else
        {
            Prev=It;
            It = It->pNext;
        }
    }
    if ( maximus->IconList == NULL )
    {
        maximus->MaximizedWindow = NULL;
        gtk_label_set_text( GTK_LABEL(maximus->WinTitle), _("Desktop") );
    }
    MAXIMUS_Printf("on_mxs_window_closed: %s\n", wnck_window_get_name(window) );
}


/* Triggers when a new active window is selected */
static void active_window_changed (WnckScreen *screen,
                                   WnckWindow *previous,
                                   MaximusPlugin *maximus )
{
    WnckWindow *active_window;

    active_window = wnck_screen_get_active_window(maximus->ActiveScreen);

    if (active_window)
    {
        const char * pTitle;
        pTitle =  wnck_window_get_name(active_window);
        MAXIMUS_Printf ("Active window:%s\n", pTitle );
        if( ! mxs_is_excluded( maximus, active_window ) )
        {
            mxs_window_set_decorations( active_window, 0 );
            wnck_window_maximize(active_window);

            mxs_NewIconAdd( maximus, active_window );
            // Mark window "maximused" */
            mxs_set_maximused(active_window, MAXIMUSED_ON);
            gdk_flush();
        }
        else
        {
            if ( mxs_need_to_restore_decorations( maximus, active_window ) )
            {
                mxs_window_set_decorations( active_window, 1 );
                mxs_set_maximused(active_window, MAXIMUSED_DECORATED);
            }
            MAXIMUS_Printf ("Window:%s excluded\n", wnck_window_get_name(active_window) );
        }
        if ( GPOINTER_TO_INT(g_object_get_data (G_OBJECT(active_window), "maximused")) != 0 )
        {
            gtk_label_set_text( GTK_LABEL(maximus->WinTitle), pTitle );
            maximus->MaximizedWindow = active_window;
        }
    }
}



void track_active_wnd( MaximusPlugin *maximus )
{
    MAXIMUS_Printf ("track_active_wnd()\n" );

    /* get window proprieties */
    maximus->ActiveScreen = wnck_screen_get_default();
    wnck_screen_force_update(maximus->ActiveScreen);

    /* Global window tracking */
    g_signal_connect(maximus->ActiveScreen, "active-window-changed", G_CALLBACK (active_window_changed), maximus);
    /* Want to be notified if window closed */
    g_signal_connect(maximus->ActiveScreen, "window-closed", G_CALLBACK (on_mxs_window_closed), maximus);

}


void mxs_OnClickedCloseWindow(GtkWidget *widget, GdkEventButton *event, MaximusPlugin *maximus)
{
    if ( event->button == 1 )
    {
        MAXIMUS_Printf( "mxs_OnClickedCloseWindow(1)\n" );
        if ( maximus->MaximizedWindow != NULL )
            wnck_window_close(maximus->MaximizedWindow, event->time );
    }
    else if ( event->button == 3 )
    {
        MAXIMUS_Printf( "mxs_OnClickedCloseWindow(3)\n" );
    }

}


#ifdef MXS_DBG_ONLY
void Wrapper_SimNewWin (MaximusPlugin * maximus)
{
    active_window_changed( NULL, NULL, maximus );
}
#endif
