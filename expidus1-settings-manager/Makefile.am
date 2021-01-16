AM_CPPFLAGS = \
	-I${top_srcdir} \
	-DMENUFILE=\"$(sysconfdir)/xdg/menus/expidus-settings-manager.menu\" \
	-DLOCALEDIR=\"$(localedir)\" \
	-DG_LOG_DOMAIN=\"expidus1-settings-manager\" \
	$(PLATFORM_CPPFLAGS)

bin_PROGRAMS = \
	expidus1-settings-manager

expidus1_settings_manager_SOURCES = \
	main.c \
	expidus-settings-manager-dialog.c \
	expidus-settings-manager-dialog.h

expidus1_settings_manager_CFLAGS = \
	$(GTK_CFLAGS) \
	$(LIBEXPIDUS1UTIL_CFLAGS) \
	$(LIBEXPIDUS1UI_CFLAGS) \
	$(MARKON_CFLAGS) \
	$(ESCONF_CFLAGS) \
	$(ENDO_CFLAGS) \
	$(PLATFORM_CFLAGS) \
    -DGTK_DISABLE_SINGLE_INCLUDES \
    -DGSEAL_ENABLE

expidus1_settings_manager_LDFLAGS = \
	-no-undefined \
	$(PLATFORM_LDFLAGS)

expidus1_settings_manager_LDADD = \
	$(GTK_LIBS) \
	$(LIBEXPIDUS1UTIL_LIBS) \
	$(LIBEXPIDUS1UI_LIBS) \
	$(ESCONF_LIBS) \
	$(ENDO_LIBS) \
	$(MARKON_LIBS)

menudir = $(sysconfdir)/xdg/menus
menu_DATA = expidus-settings-manager.menu

desktopdir = $(datadir)/applications
desktop_in_files = expidus-settings-manager.desktop.in
desktop_DATA = $(desktop_in_files:.desktop.in=.desktop)

@INTLTOOL_DESKTOP_RULE@

EXTRA_DIST = \
	$(desktop_in_files) \
	$(menu_DATA)

DISTCLEANFILES = \
	$(desktop_DATA)

# vi:set ts=8 sw=8 noet ai nocindent syntax=automake:
