noinst_HEADERS = \
	alarmMessage.h \
	mymessage.h

noinst_PROGRAMS = \
	exampleAsyncReader \
	exampleReader \
	exampleSender \
	readerBenchmarker \
	readerBenchmarkerAsync \
	recoverQueue \
	recoverServer \
	alarmClient \
	alarmServer

exampleAsyncReader_SOURCES = \
	exampleAsyncReader.cpp

exampleAsyncReader_LDFLAGS = -lcxxtools

exampleReader_SOURCES = \
	exampleReader.cpp

exampleReader_LDFLAGS = -lcxxtools

exampleSender_SOURCES = \
	exampleSender.cpp

exampleSender_LDFLAGS = -lcxxtools

readerBenchmarker_SOURCES = \
	readerBenchmarker.cpp

readerBenchmarker_LDFLAGS = -lcxxtools

readerBenchmarkerAsync_SOURCES = \
	readerBenchmarkerAsync.cpp

readerBenchmarkerAsync_LDFLAGS = -lcxxtools

recoverQueue_SOURCES = \
	recoverQueue.cpp

recoverQueue_LDFLAGS = -lcxxtools

alarmClient_SOURCES = \
	alarmMessage.cpp \
	alarmClient.cpp

alarmClient_LDFLAGS = -lcxxtools

alarmServer_SOURCES = \
	alarmMessage.cpp \
	alarmServer.cpp

alarmServer_LDFLAGS = -lcxxtools

recoverServer_SOURCES = \
    recoverServer.cpp

recoverServer_LDFLAGS = -lcxxtools

AM_CPPFLAGS = -I$(top_srcdir)/lib
LDADD = $(top_builddir)/lib/libtntpub.la

EXTRA_DIST = README
