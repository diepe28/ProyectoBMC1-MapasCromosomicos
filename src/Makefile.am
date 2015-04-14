## Process this file with automake to produce Makefile.in

## Created by Anjuta


uidir = $(pkgdatadir)/ui
ui_DATA = mapascromosomicos.ui


AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(localedir)"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(pkgdatadir)"\" \
	$(MAPASCROMOSOMICOS_CFLAGS)

AM_CFLAGS =\
	 -Wall\
	 -g

bin_PROGRAMS = mapascromosomicos

mapascromosomicos_SOURCES = \
	main.c \
	mapascromosomicos.h \
	mapascromosomicos.c \
	Algoritmo.c \
	Algoritmo.h \
	gridview.c \
	IntList.c \
	IntList.h \
	mapascromosomicos.callbacks.c \
	GenomeToolsSupport.c \
	GenomeToolsSupport.h \
	default.style \
	chromosomes_fit.jpg \
	icon.png \
	icon_window.png	

mapascromosomicos_CFLAGS = -lm \
	-I/usr/include/genometools/ \
	-I/usr/local/include/genometools/ 


mapascromosomicos_LDFLAGS = \
	-Wl,--export-dynamic \
	-lgenometools

mapascromosomicos_LDADD = $(MAPASCROMOSOMICOS_LIBS)




EXTRA_DIST = $(ui_DATA)

# Remove ui directory on uninstall
uninstall-local:
	-rm -r $(uidir)
	-rm -r $(pkgdatadir)

