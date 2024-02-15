noinst_PROGRAMS = \
    alltests

AM_CPPFLAGS = -I$(top_srcdir)/lib -I$(top_builddir)/include

alltests_SOURCES = \
    test-main.cpp \
    datamessage.cpp \
    subscription.cpp

alltests_LDADD = $(top_builddir)/lib/libtntpub.la
alltests_LDFLAGS = -lcxxtools -lcxxtools-bin -lcxxtools-unit
