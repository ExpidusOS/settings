AM_CPPFLAGS = \
	-I${top_srcdir} \
	-DDATADIR=\"$(datadir)\" \
	-DSRCDIR=\"$(top_srcdir)\" \
	-DLOCALEDIR=\"$(localedir)\" \
	-DG_LOG_DOMAIN=\"expidus1-display-settings\" \
	$(PLATFORM_CPPFLAGS)

bin_PROGRAMS = \
	expidus1-display-settings

expidus1_display_settings_SOURCES = \
	main.c \
	confirmation-dialog_ui.h \
	display-dialog_ui.h \
	minimal-display-dialog_ui.h \
	profile-changed-dialog_ui.h \
	identity-popup_ui.h \
	scrollarea.c \
	scrollarea.h \
	foo-marshal.c \
	foo-marshal.h

expidus1_display_settings_CFLAGS = \
	$(GTK_CFLAGS) \
	$(LIBEXPIDUS1UI_CFLAGS) \
	$(ESCONF_CFLAGS) \
	$(ENDO_CFLAGS) \
	$(XRANDR_CFLAGS) \
	$(LIBX11_CFLAGS) \
	$(PLATFORM_CFLAGS)

expidus1_display_settings_LDFLAGS = \
	-no-undefined \
	$(PLATFORM_LDFLAGS)

expidus1_display_settings_LDADD = \
	$(GTK_LIBS) \
	$(LIBEXPIDUS1UI_LIBS) \
	$(ESCONF_LIBS) \
	$(ENDO_LIBS) \
	$(LIBX11_LIBS) -lm \
	$(XRANDR_LIBS) \
	$(GLIB_LIBS) \
	$(top_builddir)/common/libexpidus1-settings.la

# Optional support for colord
if HAVE_COLORD
expidus1_display_settings_CFLAGS += \
	$(COLORD_CFLAGS)

expidus1_display_settings_LDADD += \
	$(COLORD_LIBS)
endif

if MAINTAINER_MODE

BUILT_SOURCES = \
	confirmation-dialog_ui.h \
	display-dialog_ui.h \
	minimal-display-dialog_ui.h \
	profile-changed-dialog_ui.h \
	identity-popup_ui.h

confirmation-dialog_ui.h: confirmation-dialog.glade
	xdt-csource --static --strip-comments --strip-content --name=confirmation_dialog_ui $< >$@

display-dialog_ui.h: display-dialog.glade
	xdt-csource --static --strip-comments --strip-content --name=display_dialog_ui $< >$@

minimal-display-dialog_ui.h: minimal-display-dialog.glade
	xdt-csource --static --strip-comments --strip-content --name=minimal_display_dialog_ui $< >$@

profile-changed-dialog_ui.h: profile-changed-dialog.glade
	xdt-csource --static --strip-comments --strip-content --name=profile_changed_dialog_ui $< >$@

identity-popup_ui.h: identity-popup.glade
	xdt-csource --static --strip-comments --strip-content --name=identity_popup_ui $< >$@

endif

desktopdir = $(datadir)/applications
desktop_in_files = expidus-display-settings.desktop.in
desktop_DATA = $(desktop_in_files:.desktop.in=.desktop)

@INTLTOOL_DESKTOP_RULE@

iconsdir = $(datadir)/icons/hicolor/128x128/devices
icons_DATA = \
	expidus-display-extend.png \
	expidus-display-external.png \
	expidus-display-internal.png \
	expidus-display-mirror.png \
	expidus-display-profile.png

EXTRA_DIST = \
	$(icons_DATA) \
	$(desktop_in_files) \
	confirmation-dialog.glade \
	display-dialog.glade \
	minimal-display-dialog.glade \
	profile-changed-dialog.glade \
	identity-popup.glade

DISTCLEANFILES = \
	$(desktop_DATA)

# vi:set ts=8 sw=8 noet ai nocindent syntax=automake:
