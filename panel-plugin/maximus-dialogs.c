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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>
#include <gtk/gtk.h>

#include <libxfce4ui/libxfce4ui.h>
#include <libxfce4panel/libxfce4panel.h>

#include "maximus.h"
#include "maximus-dialogs.h"

/* the website url */
#define PLUGIN_WEBSITE "https://docs.xfce.org/panel-plugins/xfce4-maximus-plugin"



static void
maximus_configure_response (GtkWidget    *dialog,
                           gint          response,
                           MaximusPlugin *maximus)
{
  gboolean result;

  if (response == GTK_RESPONSE_HELP)
    {
      /* show help */
      result = g_spawn_command_line_async ("exo-open --launch WebBrowser " PLUGIN_WEBSITE, NULL);

      if (G_UNLIKELY (result == FALSE))
        g_warning (_("Unable to open the following url: %s"), PLUGIN_WEBSITE);
    }
  else
    {
      /* remove the dialog data from the plugin */
      g_object_set_data (G_OBJECT (maximus->plugin), "dialog", NULL);

      /* unlock the panel menu */
      xfce_panel_plugin_unblock_menu (maximus->plugin);

      /* save the plugin */
      maximus_save (maximus->plugin, maximus);

      /* destroy the properties dialog */
      gtk_widget_destroy (dialog);
    }
}



void
maximus_configure (XfcePanelPlugin *plugin,
                  MaximusPlugin    *maximus)
{
  GtkWidget *dialog;

  /* block the plugin menu */
  xfce_panel_plugin_block_menu (plugin);

  /* create the dialog */
  dialog = xfce_titled_dialog_new_with_buttons (_("Maximus Plugin"),
                                                GTK_WINDOW (gtk_widget_get_toplevel (GTK_WIDGET (plugin))),
                                                GTK_DIALOG_DESTROY_WITH_PARENT,
                                                "gtk-help", GTK_RESPONSE_HELP,
                                                "gtk-close", GTK_RESPONSE_OK,
                                                NULL);

  /* center dialog on the screen */
  gtk_window_set_position (GTK_WINDOW (dialog), GTK_WIN_POS_CENTER);

  /* set dialog icon */
  gtk_window_set_icon_name (GTK_WINDOW (dialog), "xfce4-settings");

  /* link the dialog to the plugin, so we can destroy it when the plugin
   * is closed, but the dialog is still open */
  g_object_set_data (G_OBJECT (plugin), "dialog", dialog);

  /* connect the response signal to the dialog */
  g_signal_connect (G_OBJECT (dialog), "response",
                    G_CALLBACK(maximus_configure_response), maximus);

  /* show the entire dialog */
  gtk_widget_show (dialog);
}



void
maximus_about (XfcePanelPlugin *plugin)
{
	static const gchar * const authors[] = {
		"G Lecuyer <glecuyer207@gmail.com>",
		NULL
	};
	GdkPixbuf *icon;

    static const gchar copyright[] = \
        "Copyright \xc2\xa9 2014-2020 Gerard Lecuyer";
                   
	icon = xfce_panel_pixbuf_from_source ("xfce4-maximus-plugin", NULL, 32);	
	gtk_show_about_dialog (NULL,
						 "logo",         icon,
						 "license",      xfce_get_license_text (XFCE_LICENSE_TEXT_GPL),
						 "version",      PACKAGE_VERSION,
						 "program-name", PACKAGE_NAME,
						 "comments",     _("This is a sample plugin"),
						 "website",      "https://github.com/glecuy/xfce4-maximus",
						 "copyright",    copyright,
						 "authors",      authors,
						 NULL);

	if (icon)
		g_object_unref (G_OBJECT (icon) );
                   
}
