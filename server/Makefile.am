AUTOMAKE_OPTIONS = subdir-objects

AM_CPPFLAGS = -I$(top_srcdir)/lib
bin_PROGRAMS = tntpubserver tntpubsender

tntpubserver_SOURCES = \
	main.cpp

tntpubserver_LDADD = $(top_builddir)/lib/libtntpub.la

tntpubserver_LDFLAGS = -lcxxtools -lcxxtools-bin

tntpubsender_SOURCES = \
	tntpubsender.cpp

LDADD = $(top_builddir)/lib/libtntpub.la -lcxxtools -lcxxtools-bin

tntpubsender_LDFLAGS = -lcxxtools
