/*  $Id$
 *
 *  Copyright (C) 2014 -2020 glecuyer
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

#include <gtk/gtk.h>
#include <libxfce4util/libxfce4util.h>
#include <libxfce4panel/libxfce4panel.h>

#include "maximus.h"
#include "maximus-dialogs.h"
#include "maximus_wnck.h"

/* default settings */
#define DEFAULT_SETTING1 NULL
#define DEFAULT_SETTING2 1
#define DEFAULT_SETTING3 FALSE



/* prototypes */
static void
maximus_construct (XfcePanelPlugin *plugin);


/* register the plugin */
XFCE_PANEL_PLUGIN_REGISTER (maximus_construct);



void
maximus_save (XfcePanelPlugin *plugin,
             MaximusPlugin    *maximus)
{
  XfceRc *rc;
  gchar  *file;

  /* get the config file location */
  file = xfce_panel_plugin_save_location (plugin, TRUE);

  if (G_UNLIKELY (file == NULL))
    {
       DBG ("Failed to open config file");
       return;
    }

  /* open the config file, read/write */
  rc = xfce_rc_simple_open (file, FALSE);
  g_free (file);

  if (G_LIKELY (rc != NULL))
    {
      /* save the settings */
      DBG(".");
      if (maximus->setting1)
        xfce_rc_write_entry    (rc, "setting1", maximus->setting1);

      xfce_rc_write_int_entry  (rc, "setting2", maximus->setting2);
      xfce_rc_write_bool_entry (rc, "setting3", maximus->setting3);

      /* close the rc file */
      xfce_rc_close (rc);
    }
}



static void
maximus_read (MaximusPlugin *maximus)
{
  XfceRc      *rc;
  gchar       *file;
  const gchar *value;

  /* get the plugin config file location */
  file = xfce_panel_plugin_save_location (maximus->plugin, TRUE);

  if (G_LIKELY (file != NULL))
    {
      /* open the config file, readonly */
      rc = xfce_rc_simple_open (file, TRUE);

      /* cleanup */
      g_free (file);

      if (G_LIKELY (rc != NULL))
        {
          /* read the settings */
          value = xfce_rc_read_entry (rc, "setting1", DEFAULT_SETTING1);
          maximus->setting1 = g_strdup (value);

          maximus->setting2 = xfce_rc_read_int_entry (rc, "setting2", DEFAULT_SETTING2);
          maximus->setting3 = xfce_rc_read_bool_entry (rc, "setting3", DEFAULT_SETTING3);

          /* cleanup */
          xfce_rc_close (rc);

          /* leave the function, everything went well */
          return;
        }
    }

  /* something went wrong, apply default values */
  DBG ("Applying default settings");

  maximus->setting1 = g_strdup (DEFAULT_SETTING1);
  maximus->setting2 = DEFAULT_SETTING2;
  maximus->setting3 = DEFAULT_SETTING3;
}



static MaximusPlugin *
maximus_new (XfcePanelPlugin *plugin)
{
	MaximusPlugin   *maximus;
	GtkOrientation  orientation;
	//GtkWidget      *label;
	//GtkWidget       *box;
	GtkWidget * Image;

	/* allocate memory for the plugin structure */
	maximus = g_slice_new0 (MaximusPlugin);

	/* pointer to plugin */
	maximus->plugin = plugin;

	/* read the user settings */
	maximus_read (maximus);

	/* get the current orientation */
	orientation = xfce_panel_plugin_get_orientation (plugin);

	/* create some panel widgets */
	maximus->ebox = gtk_event_box_new ();
	gtk_widget_show (maximus->ebox);

	maximus->hvbox = gtk_box_new (orientation, 2);
	gtk_widget_show (maximus->hvbox);
	gtk_container_add (GTK_CONTAINER (maximus->ebox), maximus->hvbox);
	
    /* 1- Widget for windows' icon list */
    maximus->IconList = NULL;
    maximus->hIconBox = gtk_box_new (orientation, 2);
    gtk_widget_show (maximus->hIconBox);
    gtk_box_pack_start (GTK_BOX (maximus->hvbox), maximus->hIconBox, FALSE, FALSE, 0);

    /* 2- Widget window title bar */
    maximus->WinTitle = gtk_label_new (_("Desktop"));
    gtk_widget_show (maximus->WinTitle);
    gtk_box_pack_start (GTK_BOX (maximus->hvbox), maximus->WinTitle, FALSE, FALSE, 0);
	
	xfce_panel_plugin_set_expand( maximus->plugin, TRUE );

    /* 3- Widget for close button */
    maximus->CloseButton = gtk_button_new();   
    // See https://specifications.freedesktop.org/icon-naming-spec/icon-naming-spec-latest.html 
    //Image = gtk_image_new_from_icon_name ("window-close", GTK_ICON_SIZE_SMALL_TOOLBAR); 
    //Image = xfce_panel_pixbuf_from_source ("xfce_close", NULL, 32); 
    Image = gtk_image_new_from_pixbuf( xfce_panel_pixbuf_from_source ("xfce_close", NULL, 24) );
    
    gtk_button_set_image( GTK_BUTTON(maximus->CloseButton), Image );
    gtk_container_add (GTK_CONTAINER (maximus->CloseButton), Image);
    gtk_button_set_relief( GTK_BUTTON(maximus->CloseButton), GTK_RELIEF_NONE );        
    gtk_widget_set_tooltip_text( maximus->CloseButton, _("Close current active window") );    
    gtk_box_pack_end(GTK_BOX (maximus->hvbox), maximus->CloseButton, FALSE, FALSE, 0);
    g_signal_connect(G_OBJECT(maximus->CloseButton), "button_press_event", G_CALLBACK(mxs_OnClickedCloseWindow), (gpointer)maximus);
    gtk_widget_show (maximus->CloseButton);

	xfce_panel_plugin_set_expand( maximus->plugin, TRUE );
	return maximus;
}



static void
maximus_free (XfcePanelPlugin *plugin,
             MaximusPlugin    *maximus)
{
  GtkWidget *dialog;

  /* check if the dialog is still open. if so, destroy it */
  dialog = g_object_get_data (G_OBJECT (plugin), "dialog");
  if (G_UNLIKELY (dialog != NULL))
    gtk_widget_destroy (dialog);

  /* destroy the panel widgets */
  gtk_widget_destroy (maximus->hvbox);

  /* cleanup the settings */
  if (G_LIKELY (maximus->setting1 != NULL))
    g_free (maximus->setting1);

  /* free the plugin structure */
  g_slice_free (MaximusPlugin, maximus);
}



static void
maximus_orientation_changed (XfcePanelPlugin *plugin,
                            GtkOrientation   orientation,
                            MaximusPlugin    *maximus)
{
  /* change the orientation of the box */
  gtk_orientable_set_orientation(GTK_ORIENTABLE(maximus->hvbox), orientation);
}



static gboolean
maximus_size_changed (XfcePanelPlugin *plugin,
                     gint             size,
                     MaximusPlugin    *maximus)
{
  GtkOrientation orientation;

  /* get the orientation of the plugin */
  orientation = xfce_panel_plugin_get_orientation (plugin);

	MAXIMUS_Printf("maximus_size_changed: %d\n", xfce_panel_plugin_get_size(plugin));

  /* set the widget size */
  if (orientation == GTK_ORIENTATION_HORIZONTAL)
    gtk_widget_set_size_request (GTK_WIDGET (plugin), -1, size);
  else
    gtk_widget_set_size_request (GTK_WIDGET (plugin), size, -1);

  /* we handled the orientation */
  return TRUE;
}



static void
maximus_construct (XfcePanelPlugin *plugin)
{
  MaximusPlugin *maximus;

  /* setup transation domain */
  xfce_textdomain(GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR, "UTF-8");

  /* create the plugin */
  maximus = maximus_new (plugin);

  /* add the ebox to the panel */
  gtk_container_add (GTK_CONTAINER (plugin), maximus->ebox);

  /* show the panel's right-click menu on this ebox */
  xfce_panel_plugin_add_action_widget (plugin, maximus->ebox);

  /* connect plugin signals */
  g_signal_connect (G_OBJECT (plugin), "free-data",
                    G_CALLBACK (maximus_free), maximus);

  g_signal_connect (G_OBJECT (plugin), "save",
                    G_CALLBACK (maximus_save), maximus);

  g_signal_connect (G_OBJECT (plugin), "size-changed",
                    G_CALLBACK (maximus_size_changed), maximus);

  g_signal_connect (G_OBJECT (plugin), "orientation-changed",
                    G_CALLBACK (maximus_orientation_changed), maximus);

  /* show the configure menu item and connect signal */
  xfce_panel_plugin_menu_show_configure (plugin);
  g_signal_connect (G_OBJECT (plugin), "configure-plugin",
                    G_CALLBACK (maximus_configure), maximus);

  /* show the about menu item and connect signal */
  xfce_panel_plugin_menu_show_about (plugin);
  g_signal_connect (G_OBJECT (plugin), "about",
                    G_CALLBACK (maximus_about), NULL);

   track_active_wnd( maximus );
}


#if 1
void MAXIMUS_Log( char *fmt, ... )
{
    static FILE * flog;

    if ( flog == NULL )
    {
		char logName[80];
		snprintf(logName, 80, "/tmp/maximus-%d.log", getuid() );
        flog = fopen(logName, "at" );
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
#endif
