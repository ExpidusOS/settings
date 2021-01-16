AM_CPPFLAGS = \
	-I${top_srcdir} \
	-DDATADIR=\"$(datadir)\" \
	-DSRCDIR=\"$(top_srcdir)\" \
	-DLOCALEDIR=\"$(localedir)\" \
	-DHELPERDIR=\"$(HELPER_PATH_PREFIX)/expidus1/settings\" \
	-DG_LOG_DOMAIN=\"expidus1-appearance-settings\" \
	$(PLATFORM_CPPFLAGS)

bin_PROGRAMS = \
	expidus1-appearance-settings

expidus1_appearance_settings_SOURCES = \
	main.c \
	images.h \
	appearance-dialog_ui.h

expidus1_appearance_settings_CFLAGS = \
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

expidus1_appearance_settings_LDFLAGS = \
	-no-undefined \
	$(PLATFORM_LDFLAGS)

expidus1_appearance_settings_LDADD = \
	$(GTK_LIBS) \
	$(LIBEXPIDUS1UI_LIBS) \
	$(ESCONF_LIBS)

expidus1_appearance_scriptsdir = $(HELPER_PATH_PREFIX)/expidus1/settings
expidus1_appearance_scripts_SCRIPTS = appearance-install-theme

if MAINTAINER_MODE

BUILT_SOURCES = \
	appearance-dialog_ui.h

appearance-dialog_ui.h: appearance-dialog.glade
	xdt-csource --static --strip-comments --strip-content --name=appearance_dialog_ui $< >$@

endif

desktopdir = $(datadir)/applications
desktop_in_files = expidus-ui-settings.desktop.in
desktop_DATA = $(desktop_in_files:.desktop.in=.desktop)

@INTLTOOL_DESKTOP_RULE@

EXTRA_DIST = \
	$(desktop_in_files) \
	appearance-install-theme \
	appearance-dialog.glade

DISTCLEANFILES = \
	$(desktop_DATA)

# vi:set ts=8 sw=8 noet ai nocindent syntax=automake:
