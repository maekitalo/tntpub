noinst_HEADERS = \
	alarmMessage.h \
	mymessage.h

noinst_PROGRAMS = \
	exampleAsyncReader \
	exampleReader \
	exampleAsyncSender \
	exampleSender \
	readerBenchmarker \
	readerBenchmarkerAsync \
	recoverQueue \
	recoverServer \
	plainMessageSender \
	plainMessageReceiver \
	alarmClient \
	alarmServer

exampleAsyncReader_SOURCES = \
	exampleAsyncReader.cpp

exampleAsyncReader_LDFLAGS = -lcxxtools -lcxxtools-bin

exampleReader_SOURCES = \
	exampleReader.cpp

exampleReader_LDFLAGS = -lcxxtools -lcxxtools-bin

exampleAsyncSender_SOURCES = \
	exampleAsyncSender.cpp

exampleAsyncSender_LDFLAGS = -lcxxtools -lcxxtools-bin

exampleSender_SOURCES = \
	exampleSender.cpp

exampleSender_LDFLAGS = -lcxxtools -lcxxtools-bin

readerBenchmarker_SOURCES = \
	readerBenchmarker.cpp

readerBenchmarker_LDFLAGS = -lcxxtools -lcxxtools-bin

readerBenchmarkerAsync_SOURCES = \
	readerBenchmarkerAsync.cpp

readerBenchmarkerAsync_LDFLAGS = -lcxxtools -lcxxtools-bin

recoverQueue_SOURCES = \
	recoverQueue.cpp

recoverQueue_LDFLAGS = -lcxxtools -lcxxtools-bin

alarmClient_SOURCES = \
	alarmMessage.cpp \
	alarmClient.cpp

alarmClient_LDFLAGS = -lcxxtools -lcxxtools-bin

alarmServer_SOURCES = \
	alarmMessage.cpp \
	alarmServer.cpp

alarmServer_LDFLAGS = -lcxxtools -lcxxtools-bin

recoverServer_SOURCES = \
    recoverServer.cpp

recoverServer_LDFLAGS = -lcxxtools -lcxxtools-bin

plainMessageSender_SOURCES = \
    plainMessageSender.cpp

plainMessageSender_LDFLAGS = -lcxxtools

plainMessageReceiver_SOURCES = \
    plainMessageReceiver.cpp

plainMessageReceiver_LDFLAGS = -lcxxtools

AM_CPPFLAGS = -I$(top_srcdir)/lib
LDADD = $(top_builddir)/lib/libtntpub.la

EXTRA_DIST = README
