/*  $Id$
 *
 *  Copyright (C) 2014 Gerard Lecuyer
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

#include "../maximus.h"
#include "../maximus_wnd.h"

/* Stubbed functions for test purpose */

void mxs_window_set_decorations (WnckWindow *window, GdkWMDecoration decorations)
{
    MAXIMUS_Printf ("_window_set_decorations()\n");
}

gboolean mxs_is_excluded (MaximusPlugin *maximus, WnckWindow *window)
{
	if ( mxs_get_maximused(window) != 0 )
	{
	    return TRUE;
	}
	return FALSE;
}

