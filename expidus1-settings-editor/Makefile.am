
AM_CPPFLAGS = \
	-I${top_srcdir} \
	-DLOCALEDIR=\"$(localedir)\" \
	-DG_LOG_DOMAIN=\"expidus1-settings-editor\" \
	$(PLATFORM_CPPFLAGS)

bin_PROGRAMS = \
	expidus1-settings-editor

expidus1_settings_editor_built_sources = \
	expidus-settings-marshal.c \
	expidus-settings-marshal.h

expidus1_settings_editor_SOURCES = \
	$(expidus1_settings_editor_built_sources) \
	main.c \
	expidus-settings-cell-renderer.c \
	expidus-settings-cell-renderer.h \
	expidus-settings-editor-box.c \
	expidus-settings-editor-box.h \
	expidus-settings-prop-dialog.c \
	expidus-settings-prop-dialog.h

expidus1_settings_editor_CFLAGS = \
	$(GTK_CFLAGS) \
	$(LIBEXPIDUS1UTIL_CFLAGS) \
	$(LIBEXPIDUS1UI_CFLAGS) \
	$(ESCONF_CFLAGS) \
	$(DBUS_GLIB_CFLAGS) \
	$(PLATFORM_CFLAGS) \
    -DGTK_DISABLE_SINGLE_INCLUDES \
    -DGDK_DISABLE_DEPRECATED \
    -DGTK_DISABLE_DEPRECATED \
    -DGSEAL_ENABLE \
    -DGDK_PIXBUF_DISABLE_DEPRECATED \
    -DG_DISABLE_DEPRECATED

expidus1_settings_editor_LDFLAGS = \
	-no-undefined \
	$(PLATFORM_LDFLAGS)

expidus1_settings_editor_LDADD = \
	$(GTK_LIBS) \
	$(LIBEXPIDUS1UTIL_LIBS) \
	$(LIBEXPIDUS1UI_LIBS) \
	$(ESCONF_LIBS)

desktopdir = $(datadir)/applications
desktop_in_files = expidus1-settings-editor.desktop.in
desktop_DATA = $(desktop_in_files:.desktop.in=.desktop)

@INTLTOOL_DESKTOP_RULE@

#
# Build sources
#
if MAINTAINER_MODE

expidus-settings-marshal.h: expidus-settings-marshal.list Makefile
	$(AM_V_GEN) glib-genmarshal --prefix=_expidus_settings_marshal --internal --header $< > $@

expidus-settings-marshal.c: expidus-settings-marshal.list Makefile
	$(AM_V_GEN) echo "#include \"expidus-settings-marshal.h\"" > $@ \
	&& glib-genmarshal --prefix=_expidus_settings_marshal --body $< >> $@

DISTCLEANFILES = \
	$(expidus1_settings_editor_built_sources) \
	$(desktop_DATA)

BUILT_SOURCES = \
	$(expidus1_settings_editor_built_sources)

else

DISTCLEANFILES = \
	$(desktop_DATA)

endif

EXTRA_DIST = \
	expidus-settings-marshal.list \
	$(desktop_in_files)

# vi:set ts=8 sw=8 noet ai nocindent syntax=automake:
