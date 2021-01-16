AM_CPPFLAGS = \
	-I${top_srcdir}	\
	-DPNP_IDS=\"$(PNP_IDS)\" \
	$(PLATFORM_CPPFLAGS)

noinst_LTLIBRARIES = \
	libexpidus1-settings.la

libexpidus1_settings_la_SOURCES = \
	display-name.c \
	display-profiles.c \
	display-profiles.h \
	edid-parse.c \
	edid.h \
	expidus-randr.c \
	expidus-randr.h

libexpidus1_settings_la_CFLAGS = \
	$(GTK_CFLAGS) \
	$(LIBEXPIDUS1UTIL_CFLAGS) \
	$(ESCONF_CFLAGS) \
	$(XRANDR_CFLAGS) \
	$(LIBX11_CFLAGS) \
	$(PLATFORM_CFLAGS)

libexpidus1_settings_la_LDFLAGS = \
	-no-undefined \
	$(PLATFORM_LDFLAGS)

libexpidus1_settings_la_LIBADD = \
	$(GTK_LIBS) \
	$(ESCONF_LIBS) \
	$(LIBX11_LIBS) -lm \
	$(XRANDR_LIBS) \
	$(GLIB_LIBS)
