noinst_HEADERS = \
	mymessage.h

noinst_PROGRAMS = \
	exampleAsyncReader \
	exampleReader \
	exampleSender \
	readerBenchmarker \
	readerBenchmarkerAsync

exampleAsyncReader_SOURCES = \
	exampleAsyncReader.cpp

exampleReader_SOURCES = \
	exampleReader.cpp

exampleSender_SOURCES = \
	exampleSender.cpp

readerBenchmarker_SOURCES = \
	readerBenchmarker.cpp

readerBenchmarkerAsync_SOURCES = \
	readerBenchmarkerAsync.cpp

AM_CPPFLAGS = -I$(top_srcdir)/lib
LDADD = $(top_builddir)/lib/libtntpub.la
