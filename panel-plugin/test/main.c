/* Code Block project */
#include <stdlib.h>
#include <gtk/gtk.h>

#include "../maximus.h"

MaximusPlugin Maximus;

static void TimedSimNewWin (MaximusPlugin * maximus)
{
    extern void Wrapper_SimNewWin(MaximusPlugin * maximus);

    Wrapper_SimNewWin(maximus);
}

static void SimNewWin (GtkWidget *wid, MaximusPlugin * maximus)
{
    g_timeout_add(1000, (GSourceFunc)TimedSimNewWin, (gpointer)maximus);
}

int main (int argc, char *argv[])
{
  GtkWidget *button = NULL;
  GtkWidget *win = NULL;
  GtkWidget *hbox = NULL;
  GtkWidget * Image;

  /* Initialize GTK+ */
  g_log_set_handler ("Gtk", G_LOG_LEVEL_WARNING, (GLogFunc) gtk_false, NULL);
  gtk_init (&argc, &argv);
  g_log_set_handler ("Gtk", G_LOG_LEVEL_WARNING, g_log_default_handler, NULL);

    /* Create the main window */
    win = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_container_set_border_width (GTK_CONTAINER (win), 0);
    gtk_window_set_title (GTK_WINDOW (win), "FakeMaximus");
    gtk_window_set_position (GTK_WINDOW (win), GTK_WIN_POS_CENTER);
    gtk_widget_set_size_request(win, 1024, 32);
    gtk_window_set_resizable (GTK_WINDOW(win), FALSE);
    gtk_widget_realize (win);
    g_signal_connect (win, "destroy", gtk_main_quit, NULL);

    /* Create a horizontal box  */
    hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_container_add (GTK_CONTAINER (win), hbox);


    Maximus.hvbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_pack_start (GTK_BOX (hbox), Maximus.hvbox, TRUE, TRUE, 0);

    /* Widget for windows icom list */
    Maximus.IconList = NULL;
    Maximus.hIconBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_show (Maximus.hIconBox);
    gtk_box_pack_start (GTK_BOX (Maximus.hvbox), Maximus.hIconBox, FALSE, FALSE, 0);

    /* 2- Widget window title bar */
    Maximus.WinTitle = gtk_label_new ("Maximus");
    gtk_widget_show (Maximus.WinTitle);
    gtk_box_pack_start (GTK_BOX (Maximus.hvbox), Maximus.WinTitle, FALSE, FALSE, 0);

    Maximus.CloseEvtBox = gtk_event_box_new ();
    gtk_box_pack_end(GTK_BOX (Maximus.hvbox), Maximus.CloseEvtBox, FALSE, FALSE, 0);
    Image = gtk_image_new_from_stock( GTK_STOCK_CLOSE, GTK_ICON_SIZE_SMALL_TOOLBAR );
    //gtk_container_add(GTK_CONTAINER (maximus->hIconBox), pIcon->Image);
    gtk_container_add (GTK_CONTAINER (Maximus.CloseEvtBox), Image);
    gtk_widget_show (Maximus.CloseEvtBox);
    g_signal_connect(G_OBJECT(Maximus.CloseEvtBox), "button_press_event", G_CALLBACK(mxs_OnClickedCloseWindow), (gpointer)&Maximus);

    Maximus.ActiveScreen = wnck_screen_get_default();
    wnck_screen_force_update(Maximus.ActiveScreen);

    button = gtk_button_new_from_stock (GTK_STOCK_NEW);
    g_signal_connect (G_OBJECT (button), "clicked", G_CALLBACK (SimNewWin), (gpointer)&Maximus);
    gtk_box_pack_start (GTK_BOX (hbox), button, TRUE, TRUE, 0);
{
    extern void on_mxs_window_closed(WnckScreen *screen, WnckWindow *window, MaximusPlugin *maximus);
    /* Want to be notified if window closed */
    g_signal_connect(Maximus.ActiveScreen, "window-closed", G_CALLBACK (on_mxs_window_closed), (gpointer)&Maximus);
}


  /* Enter the main loop */
  gtk_widget_show_all (win);
  gtk_main ();
  return 0;
}
