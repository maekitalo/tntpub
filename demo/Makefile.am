noinst_PROGRAMS = \
	tntpubRead \
	tntpubReadObject \
	tntpubSend \
	tntpubSendObject \
	tntpubAsyncRead \
	tntpubAsyncReadObject

noinst_HEADERS = \
	mymessage.h

tntpubRead_SOURCES = tntpubRead.cpp
tntpubReadObject_SOURCES = tntpubReadObject.cpp
tntpubSend_SOURCES = tntpubSend.cpp
tntpubSendObject_SOURCES = tntpubSendObject.cpp
tntpubAsyncRead_SOURCES = tntpubAsyncRead.cpp
tntpubAsyncReadObject_SOURCES = tntpubAsyncReadObject.cpp

AM_CPPFLAGS = -I$(top_srcdir)/include
AM_LDFLAGS = -lcxxtools
LDADD = $(top_builddir)/src/libtntpub.la
