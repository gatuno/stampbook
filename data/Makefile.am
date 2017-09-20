gamedatadir = $(pkgdatadir)/data

nobase_dist_gamedata_DATA = \
	images/portada_negra.png \
	images/clasp.png \
	images/clasp_thumb_1.png \
	burbanksb.ttf

# Instalar los archivos .desktop e iconos
applicationsdir = $(datadir)/applications
applications_DATA = desktop/stampbook.desktop

icons16dir = $(datadir)/icons/hicolor/16x16/apps
icons16_DATA = desktop/16x16/stampbook.png

icons32dir = $(datadir)/icons/hicolor/32x32/apps
icons32_DATA = desktop/32x32/stampbook.png

icons48dir = $(datadir)/icons/hicolor/48x48/apps
icons48_DATA = desktop/48x48/stampbook.png

icons64dir = $(datadir)/icons/hicolor/64x64/apps
icons64_DATA = desktop/64x64/stampbook.png

icons128dir = $(datadir)/icons/hicolor/128x128/apps
icons128_DATA = desktop/128x128/stampbook.png

icons256dir = $(datadir)/icons/hicolor/256x256/apps
icons256_DATA = desktop/256x256/stampbook.png

# touch the toplevel icon directory as required by the icon theme spec
install-data-hook:
	touch $(DESTDIR)$(datadir)/icons/hicolor

EXTRA_DIST = \
	desktop/stampbook.desktop	\
	desktop/16x16/stampbook.png		\
	desktop/32x32/stampbook.png		\
	desktop/48x48/stampbook.png		\
	desktop/64x64/stampbook.png		\
	desktop/128x128/stampbook.png		\
	desktop/256x256/stampbook.png		\
	stampbook.ico
