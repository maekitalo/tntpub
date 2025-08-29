bin_PROGRAMS = \
	tntpubRead \
	tntpubSend

tntpubRead_SOURCES = tntpubRead.cpp
tntpubSend_SOURCES = tntpubSend.cpp

AM_CPPFLAGS = -I$(top_srcdir)/include
AM_LDFLAGS = -lcxxtools
LDADD = $(top_builddir)/src/libtntpub.la
