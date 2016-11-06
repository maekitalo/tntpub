AUTOMAKE_OPTIONS = subdir-objects

AM_CPPFLAGS = -I$(top_srcdir)/lib
bin_PROGRAMS = tntpubserver

nobase_include_HEADERS = \
	tntpub/responder.h \
	tntpub/server.h

tntpubserver_SOURCES = \
	main.cpp \
	tntpub/responder.cpp \
	tntpub/server.cpp

tntpubserver_LDADD = $(top_builddir)/lib/libtntpub.la
