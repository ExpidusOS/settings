AM_CPPFLAGS = \
	-I${top_srcdir} \
	-DDATADIR=\"$(datadir)\" \
	-DSRCDIR=\"$(top_srcdir)\" \
	-DLOCALEDIR=\"$(localedir)\" \
	-DG_LOG_DOMAIN=\"expidus1-accessibility-settings\" \
	$(PLATFORM_CPPFLAGS)

bin_PROGRAMS = \
	expidus1-accessibility-settings \
	expidus1-find-cursor

expidus1_find_cursor_SOURCES = \
  find-cursor.c

expidus1_find_cursor_CFLAGS = \
	$(GTK_CFLAGS) \
	$(ESCONF_CFLAGS) \
	$(PLATFORM_CFLAGS) \
    -DGTK_DISABLE_SINGLE_INCLUDES \
    -DGDK_DISABLE_DEPRECATED \
    -DGTK_DISABLE_DEPRECATED \
    -DGSEAL_ENABLE \
    -DGDK_PIXBUF_DISABLE_DEPRECATED \
    -DG_DISABLE_DEPRECATED

expidus1_find_cursor_LDFLAGS = \
	-no-undefined \
	$(PLATFORM_LDFLAGS)

expidus1_find_cursor_LDADD = \
	$(GTK_LIBS) \
	$(ESCONF_LIBS)

expidus1_accessibility_settings_SOURCES = \
	main.c \
	accessibility-dialog_ui.h

expidus1_accessibility_settings_CFLAGS = \
	$(GTK_CFLAGS) \
	$(LIBEXPIDUS1UI_CFLAGS) \
	$(ESCONF_CFLAGS) \
	$(PLATFORM_CFLAGS) \
    -DGTK_DISABLE_SINGLE_INCLUDES \
    -DGDK_DISABLE_DEPRECATED \
    -DGTK_DISABLE_DEPRECATED \
    -DGSEAL_ENABLE \
    -DGDK_PIXBUF_DISABLE_DEPRECATED \
    -DG_DISABLE_DEPRECATED

expidus1_accessibility_settings_LDFLAGS = \
	-no-undefined \
	$(PLATFORM_LDFLAGS)

expidus1_accessibility_settings_LDADD = \
	$(GTK_LIBS) \
	$(LIBEXPIDUS1UI_LIBS) \
	$(ESCONF_LIBS)

if MAINTAINER_MODE

BUILT_SOURCES = \
	accessibility-dialog_ui.h

accessibility-dialog_ui.h: accessibility-dialog.glade
	xdt-csource --static --strip-comments --strip-content --name=accessibility_dialog_ui $< >$@

endif

desktopdir = $(datadir)/applications
desktop_in_files = expidus1-accessibility-settings.desktop.in
desktop_DATA = $(desktop_in_files:.desktop.in=.desktop)

@INTLTOOL_DESKTOP_RULE@

EXTRA_DIST = \
	$(desktop_in_files) \
	accessibility-dialog.glade

DISTCLEANFILES = \
	$(desktop_DATA)

# vi:set ts=8 sw=8 noet ai nocindent syntax=automake:
