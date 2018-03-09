# Automake file for stampbook

gamedatadir = $(pkgdatadir)/data

bin_PROGRAMS = stampbook
stampbook_SOURCES = book.c book.h \
	path.c path.h \
	stamp.c stamp.h \
	page.c page.h \
	cp-button.c cp-button.h \
	draw-text.c draw-text.h

if MINGW32
stampbook_SOURCES += 
endif

EXTRA_DIST = stampbook.rc

if MINGW32
stampbook_ico.o: stampbook.rc ../data/stampbook.ico
	$(WINDRES) $(srcdir)/stampbook.rc -O coff -o stampbook_ico.o

mingw_ldadd = stampbook_ico.o

else # NOT MINGW
mingw_ldadd =
endif

stampbook_CPPFLAGS = -DGAMEDATA_DIR=\"$(gamedatadir)/\" -DLOCALEDIR=\"$(localedir)\" $(AM_CPPFLAGS)
stampbook_CFLAGS = $(SDL_CFLAGS) $(SDL_image_CFLAGS) $(SDL_mixer_CFLAGS) $(SDL_ttf_CFLAGS) $(AM_CFLAGS)
if MACOSX
# En MAC OS X, hay que ligar/compilar contra los frameworks
stampbook_LDFLAGS = -Wl,-rpath,@loader_path/../Frameworks
else
stampbook_LDADD = $(SDL_LIBS) $(SDL_image_LIBS) $(SDL_mixer_LIBS) $(SDL_ttf_LIBS) $(mingw_ldadd)
endif
LDADD = $(LIBINTL)

#------------------ Packaging rules for Mac OSX ------------------------

bundle_root = $(top_builddir)/etc/macfiles
bundle_name = $(bundle_root)/stampbook.app
# Special target to copy all needed game files to PaddlePuffle.app/Contents/Resources.
# For Mac OS X bundle.
.PHONY: bundle
bundle:
	-rm -rf $(bundle_root)
	mkdir -p $(bundle_name)/Contents/MacOS
	mkdir -p $(bundle_name)/Contents/Resources/data/
	echo "APPL????" > $(bundle_name)/Contents/PkgInfo
	cp $(top_builddir)/etc/Info.plist $(bundle_name)/Contents/
	cp $(top_builddir)/etc/stampbook.icns $(bundle_name)/Contents/Resources/
	mkdir -p $(bundle_name)/Contents/Resources/data/images
	cp -R $(top_builddir)/data/images/* $(bundle_name)/Contents/Resources/data/images
	mkdir -p $(bundle_name)/Contents/Resources/data/music
	cp -R $(top_builddir)/data/music/* $(bundle_name)/Contents/Resources/data/music
	mkdir -p $(bundle_name)/Contents/Resources/data/sounds
	cp -R $(top_builddir)/data/sounds/* $(bundle_name)/Contents/Resources/data/sounds
	cp $(top_builddir)/data/*.ttf $(bundle_name)/Contents/Resources/data
	for cat in $(ALL_LINGUAS); do \
	  dir=$(bundle_name)/Contents/Resources/locale/$$cat/LC_MESSAGES; \
	  mkdir -p $$dir; \
	  echo "installing $$cat as $$dir/stampbook.mo"; \
	  cp $(top_builddir)/po/$$cat.gmo $$dir/stampbook.mo; \
	done

# Special target to copy the binary to PaddlePuffle.app/Contents/MacOS.
.PHONY: bundle-fw
bundle-fw: bundle
	cp stampbook $(bundle_name)/Contents/MacOS/stampbook
	mkdir  $(bundle_name)/Contents/Frameworks
	for i in SDL SDL_mixer SDL_image SDL_ttf ; do \
	  cp -a /Library/Frameworks/$${i}.framework $(bundle_name)/Contents/Frameworks/; \
	done
	strip $(bundle_name)/Contents/MacOS/stampbook

# Special target to make a disk image of the bundled app
.PHONY: bundle-dmg
bundle-dmg: bundle-fw
	ln -s /Applications $(bundle_root)/Applications
	hdiutil create -ov -srcfolder $(bundle_root) -volname stampbook -imagekey zlib-level=6 $(top_builddir)/etc/stampbook.dmg

