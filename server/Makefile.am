AUTOMAKE_OPTIONS = subdir-objects

AM_CPPFLAGS = -I$(top_srcdir)/lib
bin_PROGRAMS = tntpubserver

tntpubserver_SOURCES = \
	main.cpp

tntpubserver_LDADD = $(top_builddir)/lib/libtntpub.la
