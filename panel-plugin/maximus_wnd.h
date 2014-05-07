/*  maximus_wnd.h
 *
 *  Private definitions for maxumus xfce panel plugin
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

#ifndef __MAXIMUS_WND_H__
#define __MAXIMUS_WND_H__

extern void MAXIMUS_Log( char *fmt, ... );

//#define MAXIMUS_Printf g_print
//#define MAXIMUS_Printf MAXIMUS_Log
#define MAXIMUS_Printf(fmt, ...) ((void)0)


#define mxs_get_maximused(win)         (GPOINTER_TO_INT(g_object_get_data (G_OBJECT(win), "maximused")))
#define mxs_set_maximused(win, value)  (g_object_set_data( G_OBJECT (win), "maximused", GINT_TO_POINTER(value)))


gboolean mxs_is_excluded (MaximusPlugin *maximus, WnckWindow *window);

void mxs_window_set_decorations (WnckWindow *window, GdkWMDecoration decorations);

#endif // __MAXIMUS_WND_H__
