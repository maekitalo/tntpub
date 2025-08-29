noinst_PROGRAMS = \
	tntpubReadObject \
	tntpubSendObject \
	tntpubAsyncRead \
	tntpubAsyncReadObject

noinst_HEADERS = \
	mymessage.h

tntpubReadObject_SOURCES = tntpubReadObject.cpp
tntpubSendObject_SOURCES = tntpubSendObject.cpp
tntpubAsyncRead_SOURCES = tntpubAsyncRead.cpp
tntpubAsyncReadObject_SOURCES = tntpubAsyncReadObject.cpp

AM_CPPFLAGS = -I$(top_srcdir)/include
AM_LDFLAGS = -lcxxtools
LDADD = $(top_builddir)/src/libtntpub.la
