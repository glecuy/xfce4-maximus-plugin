INCLUDES = \
	-I$(top_srcdir) \
	-DG_LOG_DOMAIN=\"xfce4-maximus-plugin\" \
	-DPACKAGE_LOCALE_DIR=\"$(localedir)\" \
	$(PLATFORM_CPPFLAGS)

#
# Sample plugin
#
plugin_LTLIBRARIES = \
	libmaximus.la

plugindir = \
	$(libdir)/xfce4/panel/plugins

libmaximus_la_SOURCES = \
	maximus.c \
	maximus.h \
	maximus_wnck.c \
	maximus_xlib.c \
	maximus-dialogs.c \
	maximus_wnck.h \
	maximus_wnd.h \
	maximus-dialogs.h

libmaximus_la_CFLAGS = \
	$(LIBXFCE4UTIL_CFLAGS) \
	$(LIBXFCE4UI_CFLAGS) \
	$(LIBXFCE4PANEL_CFLAGS) \
	$(LIBWNCK_CFLAGS) \
	$(PLATFORM_CFLAGS)

libmaximus_la_LDFLAGS = \
       -avoid-version \
       -module \
       -no-undefined \
       -export-symbols-regex '^xfce_panel_module_(preinit|init|construct)' \
       $(PLATFORM_LDFLAGS)

libmaximus_la_LIBADD = \
	$(LIBXFCE4UTIL_LIBS) \
	$(LIBXFCE4UI_LIBS) \
	$(LIBXFCE4PANEL_LIBS) \
	$(LIBWNCK_LIBS)

#
# Desktop file
#
desktopdir =								\
	$(datadir)/xfce4/panel/plugins

desktop_DATA =								\
	maximus.desktop

@INTLTOOL_DESKTOP_RULE@

EXTRA_DIST =								\
	test/main.c								\
	test/maximus_xlib.c						\
	maximus.cbp						        \
	maximus.desktop.in

CLEANFILES =								\
	$(desktop_DATA)

# vi:set ts=8 sw=8 noet ai nocindent syntax=automake:
