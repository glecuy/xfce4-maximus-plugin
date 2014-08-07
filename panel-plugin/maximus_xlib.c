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
#include <X11/Xatom.h>
#include <X11/Xutil.h>

#include <libxfce4util/libxfce4util.h>
#include <libxfce4panel/xfce-panel-plugin.h>
#include <libxfce4panel/xfce-hvbox.h>

#include "maximus.h"
#include "maximus_wnd.h"


/* A set of default exceptions */
static gchar *default_exclude_classes[] =
{
  "xfce4-panel",
  "xfce4-mixer",
  "wrapper",
  "mintUpdate",
  "Bluetooth-properties",
  "Bluetooth-wizard",
  "Places", /* Firefox Download Window */
  "Ekiga",
  "Extension", /* Firefox Add-Ons/Extension Window */
  "Gcalctool",
  "inkscape",
  "Global", /* Firefox Error Console Window */
  "Gnome-dictionary",
  "Gnome-launguage-selector",
  "Gnome-nettool",
  "Gnome-volume-control",
  "Kiten",
  "Kmplot",
  "Nm-editor",
  "Pidgin",
  "Polkit-gnome-authorization",
  "Update-manager",
  "Skype",
  "Toplevel", /* Firefox "Clear Private Data" Window */
  "Transmission"
};

/* TAKEN FROM MAXIMUS, xfce-appmenu-plugin */
typedef struct {
    unsigned long flags;
    unsigned long functions;
    unsigned long decorations;
    long input_mode;
    unsigned long status;
} MotifWmHints, MwmHints;

#define MWM_HINTS_FUNCTIONS     (1L << 0)
#define MWM_HINTS_DECORATIONS   (1L << 1)
#define _XA_MOTIF_WM_HINTS      "_MOTIF_WM_HINTS"

static void gdk_window_set_mwm_hints (WnckWindow *window, MotifWmHints *new_hints);
static void _window_set_decorations (WnckWindow      *window, GdkWMDecoration decorations);

void _wnck_get_wmclass (Window xwindow, char **res_class,  char **res_name);


static char* latin1_to_utf8 (const char *latin1)
{
  GString *str;
  const char *p;

  str = g_string_new (NULL);

  p = latin1;
  while (*p)
    {
      g_string_append_unichar (str, (gunichar) *p);
      ++p;
    }

  return g_string_free (str, FALSE);
}

void _wnck_get_wmclass (Window xwindow,
                   char **res_class,
                   char **res_name)
{
  XClassHint ch;

  ch.res_name = NULL;
  ch.res_class = NULL;

  XGetClassHint (gdk_display, xwindow, &ch);

  if (res_class)
    *res_class = NULL;

  if (res_name)
    *res_name = NULL;

  if (ch.res_name)
    {
      if (res_name)
        *res_name = latin1_to_utf8 (ch.res_name);

      XFree (ch.res_name);
    }

  if (ch.res_class)
    {
      if (res_class)
        *res_class = latin1_to_utf8 (ch.res_class);

      XFree (ch.res_class);
    }
}

static void gdk_window_set_mwm_hints (WnckWindow *window, MotifWmHints *new_hints)
{
  GdkDisplay *display = gdk_display_get_default();
  Atom hints_atom = None;
  guchar *data = NULL;
  MotifWmHints *hints = NULL;
  Atom type = None;
  gint format;
  gulong nitems;
  gulong bytes_after;

  g_return_if_fail (WNCK_IS_WINDOW (window));
  g_return_if_fail (GDK_IS_DISPLAY (display));

  hints_atom = gdk_x11_get_xatom_by_name_for_display (display,
                                                      _XA_MOTIF_WM_HINTS);
  XGetWindowProperty (GDK_DISPLAY_XDISPLAY (display),
                      wnck_window_get_xid (window),
                          hints_atom, 0, sizeof (MotifWmHints)/sizeof (long),
                          False, AnyPropertyType, &type, &format, &nitems,
                          &bytes_after, &data);

  if (type != hints_atom || !data)
    hints = new_hints;
  else
  {
    hints = (MotifWmHints *)data;

    if (new_hints->flags & MWM_HINTS_FUNCTIONS)
    {
      hints->flags |= MWM_HINTS_FUNCTIONS;
      hints->functions = new_hints->functions;
    }
    if (new_hints->flags & MWM_HINTS_DECORATIONS)
    {
      hints->flags |= MWM_HINTS_DECORATIONS;
      hints->decorations = new_hints->decorations;
    }
  }

  XChangeProperty (GDK_DISPLAY_XDISPLAY (display),
                   wnck_window_get_xid (window),
                   hints_atom, hints_atom, 32, PropModeReplace,
                   (guchar *)hints, sizeof (MotifWmHints)/sizeof (long));
  gdk_flush ();

  if (data)
    XFree (data);
}



void mxs_window_set_decorations(WnckWindow      *window, GdkWMDecoration decorations)
{
  MotifWmHints *hints;

  g_return_if_fail (WNCK_IS_WINDOW (window));

  /* initialize to zero to avoid writing uninitialized data to socket */
  hints = g_slice_new0 (MotifWmHints);
  hints->flags = MWM_HINTS_DECORATIONS;
  hints->decorations = decorations;

  gdk_window_set_mwm_hints (window, hints);

  g_slice_free (MotifWmHints, hints);
}

gboolean mxs_is_excluded (MaximusPlugin *maximus, WnckWindow *window)
{
    WnckWindowType type;
    WnckWindowActions actions;
    WnckWindowState state;
    gchar *res_name;
    gchar *class_name;
    GSList *c;
    gint i, maximused;

    g_return_val_if_fail (WNCK_IS_WINDOW (window), TRUE);

    type = wnck_window_get_window_type (window);
    if (type != WNCK_WINDOW_NORMAL)
        return TRUE;

    MAXIMUS_Printf ("testing is_excluded: %s\n",wnck_window_get_name (window));

    /* Ignore if the window is already fullscreen */
    if (wnck_window_is_fullscreen (window))
    {
        MAXIMUS_Printf ("Excluding (is fullscreen): %s\n",wnck_window_get_name (window));
        return TRUE;
    }

    maximused = GPOINTER_TO_INT(g_object_get_data (G_OBJECT(window), "maximused"));
    MAXIMUS_Printf ("maximused = %d\n", maximused );
    if ( maximused == MAXIMUSED_ON )
    {
        MAXIMUS_Printf ("Excluding (maximused): %s\n",wnck_window_get_name (window));
        return TRUE;
    }
    else if ( maximused == MAXIMUSED_DECORATED )
    {
        state = wnck_window_get_state(window);
        if ( state & WNCK_WINDOW_STATE_MAXIMIZED_VERTICALLY )
            return FALSE;
        else
            return TRUE;
    }

    _wnck_get_wmclass (wnck_window_get_xid (window), &res_name, &class_name);
    MAXIMUS_Printf ("Window opened: res_name=%s -- class_name=%s\n", res_name, class_name);

    /* Check internal list of class_ids */
    for (i = 0; i < G_N_ELEMENTS (default_exclude_classes); i++)
    {
        if (   (class_name && strstr (class_name, default_exclude_classes[i]))
            || (res_name && strstr (res_name, default_exclude_classes[i])) )
        {
            MAXIMUS_Printf ("Excluding: %s [%s,%s]\n", wnck_window_get_name(window), class_name, res_name);
            return TRUE;
        }
    }

    /* Check user list TODO */
    for (c = maximus->exclude_class_list; c; c = c->next)
    {
        if ((class_name && c->data && strstr (class_name, c->data))
            || (res_name && c->data && strstr (res_name, c->data) ))
        {
            MAXIMUS_Printf ("Excluding: %s\n", wnck_window_get_name (window));
            return TRUE;
        }
    }

    g_free (res_name);
    g_free (class_name);
    return FALSE;
}



