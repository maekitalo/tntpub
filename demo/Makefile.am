noinst_HEADERS = \
	mymessage.h

noinst_PROGRAMS = \
	exampleReader \
	exampleSender

exampleReader_SOURCES = \
	exampleReader.cpp

exampleSender_SOURCES = \
	exampleSender.cpp

AM_CPPFLAGS = -I$(top_builddir)/lib
LDADD = $(top_builddir)/lib/libtntpub.la
