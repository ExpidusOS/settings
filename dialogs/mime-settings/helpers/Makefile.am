
defaultsdir = $(sysconfdir)/xdg/expidus1
defaults_DATA = helpers.rc

desktopdir = $(datadir)/expidus1/helpers
desktop_in_in_files =							\
	aterm.desktop.in.in							\
	balsa.desktop.in.in							\
	brave.desktop.in.in							\
	caja.desktop.in.in							\
	chromium.desktop.in.in						\
	debian-sensible-browser.desktop.in.in		\
	debian-x-terminal-emulator.desktop.in.in	\
	dillo.desktop.in.in							\
	encompass.desktop.in.in						\
	epiphany.desktop.in.in						\
	eterm.desktop.in.in							\
	evolution.desktop.in.in						\
	firefox.desktop.in.in						\
	galeon.desktop.in.in						\
	geary.desktop.in.in							\
	google-chrome.desktop.in.in					\
	gnome-terminal.desktop.in.in				\
	icecat.desktop.in.in						\
	icedove.desktop.in.in						\
	iceweasel.desktop.in.in						\
	jumanji.desktop.in.in						\
	kmail.desktop.in.in							\
	konqueror.desktop.in.in						\
	links.desktop.in.in							\
	lynx.desktop.in.in							\
	midori.desktop.in.in						\
	mozilla-browser.desktop.in.in				\
	mozilla-mailer.desktop.in.in				\
	mutt.desktop.in.in							\
	nautilus.desktop.in.in						\
	netscape-navigator.desktop.in.in			\
	nxterm.desktop.in.in						\
	opera-browser.desktop.in.in					\
	pcmanfm.desktop.in.in						\
	pcmanfm-qt.desktop.in.in					\
	qterminal.desktop.in.in						\
	qtfm.desktop.in.in							\
	qupzilla.desktop.in.in						\
	rodent.desktop.in.in						\
	rox-filer.desktop.in.in						\
	sakura.desktop.in.in						\
	surf.desktop.in.in							\
	sylpheed.desktop.in.in						\
	sylpheed-claws.desktop.in.in				\
	terminator.desktop.in.in					\
	lunar.desktop.in.in						\
	thunderbird.desktop.in.in					\
	urxvt.desktop.in.in							\
	vimprobable2.desktop.in.in					\
	w3m.desktop.in.in							\
	expidus1-terminal.desktop.in.in				\
	xfe.desktop.in.in							\
	xterm.desktop.in.in


desktop_in_files = $(desktop_in_in_files:.desktop.in.in=.desktop.in)
desktop_DATA = $(desktop_in_files:.desktop.in=.desktop)
%.desktop.in: %.desktop.in.in
	sed -e 's,\@HELPERDIR\@,$(HELPER_PATH_PREFIX)/expidus1,g' < $< > $@
@INTLTOOL_DESKTOP_RULE@

expidus1_composedir = $(HELPER_PATH_PREFIX)/expidus1
expidus1_compose_SCRIPTS =						\
	expidus1-compose-mail

DISTCLEANFILES =							\
	$(desktop_in_files)						\
	$(desktop_DATA)

EXTRA_DIST =								\
	$(defaults_DATA)						\
	$(desktop_in_in_files)					\
	expidus1-compose-mail

# vi:set ts=8 sw=8 noet ai nocindent syntax=automake:
