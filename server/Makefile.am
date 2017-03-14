AUTOMAKE_OPTIONS = subdir-objects

AM_CPPFLAGS = -I$(top_srcdir)/lib
bin_PROGRAMS = tntpubserver tntpubsender

tntpubserver_SOURCES = \
	main.cpp

tntpubserver_LDADD = $(top_builddir)/lib/libtntpub.la

tntpubsender_SOURCES = \
	tntpubsender.cpp

tntpubsender_LDADD = $(top_builddir)/lib/libtntpub.la
