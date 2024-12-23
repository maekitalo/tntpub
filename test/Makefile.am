noinst_PROGRAMS = \
    alltests \
	tntpubReadBench \
	tntpubWriteBench

AM_CPPFLAGS = -I$(top_srcdir)/include -I$(top_builddir)/include

alltests_SOURCES = \
    test-main.cpp \
    datamessage.cpp \
    subscription.cpp

alltests_LDFLAGS = -lcxxtools -lcxxtools-bin -lcxxtools-unit

tntpubReadBench_SOURCES = tntpubReadBench.cpp
tntpubWriteBench_SOURCES = tntpubWriteBench.cpp

LDADD = $(top_builddir)/lib/libtntpub.la

AM_LDFLAGS = -lcxxtools
