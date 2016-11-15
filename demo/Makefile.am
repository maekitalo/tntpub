noinst_HEADERS = \
	mymessage.h

noinst_PROGRAMS = \
	exampleAsyncReader \
	exampleReader \
	exampleSender \
	readerBenchmarker \
	readerBenchmarkerAsync \
	recoverQueue \
	alarmClient \
	alarmServer

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

recoverQueue_SOURCES = \
	recoverQueue.cpp

alarmClient_SOURCES = \
	alarmMessage.cpp \
	alarmClient.cpp

alarmServer_SOURCES = \
	alarmMessage.cpp \
	alarmServer.cpp

AM_CPPFLAGS = -I$(top_srcdir)/lib
LDADD = $(top_builddir)/lib/libtntpub.la
