noinst_HEADERS = \
	mymessage.h

noinst_PROGRAMS = \
	exampleAsyncReader \
	exampleReader \
	exampleSender

exampleAsyncReader_SOURCES = \
	exampleAsyncReader.cpp

exampleReader_SOURCES = \
	exampleReader.cpp

exampleSender_SOURCES = \
	exampleSender.cpp

AM_CPPFLAGS = -I$(top_builddir)/lib
LDADD = $(top_builddir)/lib/libtntpub.la
