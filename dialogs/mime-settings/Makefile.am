SUBDIRS = \
	helpers

AM_CPPFLAGS = \
	-I${top_srcdir} \
	-DBINDIR=\"$(bindir)\" \
	-DDATADIR=\"$(datadir)\" \
	-DLOCALEDIR=\"$(localedir)\" \
	-DG_LOG_DOMAIN=\"expidus1-mime-settings\" \
	$(PLATFORM_CPPFLAGS)

bin_PROGRAMS = \
	expidus1-mime-settings	\
	expidus1-mime-helper

expidus1_mime_helper_built_sources = \
	expidus-mime-helper-enum-types.c \
	expidus-mime-helper-enum-types.h

expidus_mime_helper_headers = \
	expidus-mime-helper.h

expidus1_mime_settings_SOURCES = \
	$(expidus1_mime_helper_built_sources) \
	$(expidus_mime_helper_headers) \
	main.c \
	expidus-mime-helper-utils.c \
	expidus-mime-helper-utils.h \
	expidus-mime-helper.c \
	expidus-mime-helper-chooser.c \
	expidus-mime-helper-chooser.h \
	expidus-mime-chooser.c \
	expidus-mime-chooser.h \
	expidus-mime-window.c \
	expidus-mime-window.h

expidus1_mime_settings_CFLAGS = \
	$(GTK_CFLAGS) \
	$(GIO_CFLAGS) \
	$(GIO_UNIX_CFLAGS) \
	$(LIBEXPIDUS1UI_CFLAGS) \
	$(ENDO_CFLAGS) \
	$(ESCONF_CFLAGS) \
	$(PLATFORM_CFLAGS) \
    -DGTK_DISABLE_SINGLE_INCLUDES \
    -DGDK_DISABLE_DEPRECATED \
    -DGTK_DISABLE_DEPRECATED \
    -DGSEAL_ENABLE \
    -DGDK_PIXBUF_DISABLE_DEPRECATED \
    -DG_DISABLE_DEPRECATED

expidus1_mime_settings_LDFLAGS = \
	-no-undefined \
	$(PLATFORM_LDFLAGS)

expidus1_mime_settings_LDADD = \
	$(GTK_LIBS) \
	$(GIO_LIBS) \
	$(GIO_UNIX_LIBS) \
	$(LIBEXPIDUS1UI_LIBS) \
	$(LIBEXPIDUS1UTIL_LIBS) \
	$(ENDO_LIBS) \
	$(ESCONF_LIBS)

expidus1_mime_helperdir = $(HELPER_PATH_PREFIX)/expidus1

expidus1_mime_helper_SOURCES = \
	$(expidus1_mime_helper_built_sources) \
	$(expidus1_mime_helper_headers) \
	expidus-mime-helper-chooser.c \
	expidus-mime-helper-chooser.h \
	expidus-mime-helper-launcher-dialog.c \
	expidus-mime-helper-launcher-dialog.h \
	expidus-mime-helper-utils.c \
	expidus-mime-helper-utils.h \
	expidus-mime-helper.c \
	expidus-mime-helper-main.c

expidus1_mime_helper_CFLAGS = \
	$(GTK3_CFLAGS) \
	$(GIO_UNIX_CFLAGS) \
	$(LIBEXPIDUS1UTIL_CFLAGS) \
	$(LIBEXPIDUS1UI_CFLAGS) \
    $(ENDO_CFLAGS)

expidus1_mime_helper_LDADD = \
	$(GTK3_LIBS) \
	$(GIO_UNIX_LIBS) \
	$(LIBEXPIDUS1UTIL_LIBS) \
	$(LIBEXPIDUS1UI_LIBS) \
	$(ENDO_LIBS)

desktopdir = $(datadir)/applications
desktop_in_files = \
	expidus1-mime-settings.desktop.in \
	expidus1-file-manager.desktop.in \
	expidus1-mail-reader.desktop.in \
	expidus1-terminal-emulator.desktop.in \
	expidus1-web-browser.desktop.in
desktop_DATA = $(desktop_in_files:.desktop.in=.desktop)

@INTLTOOL_DESKTOP_RULE@

if MAINTAINER_MODE
DISTCLEANFILES = \
	stamp-expidus-mime-helper-enum-types.h \
	$(expidus1_mime_helper_built_sources)

BUILT_SOURCES = \
	$(expidus1_mime_helper_built_sources)

expidus-mime-helper-enum-types.h: stamp-expidus-mime-helper-enum-types.h
	@true
stamp-expidus-mime-helper-enum-types.h: $(expidus_mime_helper_headers) Makefile
	$(AM_V_GEN) ( cd $(srcdir) && glib-mkenums \
		--fhead "#ifndef __EXPIDUS_MIME_HELPER_ENUM_TYPES_H__\n#define __EXPIDUS_MIME_HELPER_ENUM_TYPES_H__\n#include <endo/endo.h>\nG_BEGIN_DECLS\n" \
		--fprod "/* enumerations from \"@filename@\" */\n" \
		--vhead "GType @enum_name@_get_type (void) G_GNUC_CONST;\n#define EXPIDUS_MIME_TYPE_@ENUMSHORT@ (@enum_name@_get_type())\n" \
		--ftail "G_END_DECLS\n\n#endif /* !__EXPIDUS_MIME_HELPER_ENUM_TYPES_H__ */" \
		$(expidus_mime_helper_headers) ) >> xgen-eeth \
	&& (cmp -s xgen-eeth expidus-mime-helper-enum-types.h || cp xgen-eeth expidus-mime-helper-enum-types.h ) \
	&& rm -f xgen-eeth \
	&& echo timestamp > $(@F)

expidus-mime-helper-enum-types.c: $(expidus_mime_helper_headers) Makefile
	$(AM_V_GEN) ( cd $(srcdir) && glib-mkenums \
		--fhead "#undef GTK_DISABLE_DEPRECATED\n#define GTK_ENABLE_BROKEN\n#include <dialogs/mime-settings/expidus-mime-helper.h>\n\n#include \"expidus-mime-helper-enum-types.h\"" \
		--fprod "\n/* enumerations from \"@filename@\" */" \
		--vhead "GType\n@enum_name@_get_type (void)\n{\n\tstatic GType type = 0;\n\tif (type == 0) {\n\tstatic const G@Type@Value values[] = {"\
		--vprod "\t{ @VALUENAME@, \"@VALUENAME@\", \"@valuenick@\" }," \
		--vtail "\t{ 0, NULL, NULL }\n\t};\n\ttype = g_@type@_register_static (\"@EnumName@\", values);\n  }\n\treturn type;\n}\n" \
		$(expidus_mime_helper_headers) ) >> xgen-eetc \
	&& cp xgen-eetc expidus-mime-helper-enum-types.c \
	&& rm -f xgen-eetc
else
DISTCLEANFILES = \
	$(desktop_DATA)
endif

EXTRA_DIST = \
	$(desktop_in_files)


