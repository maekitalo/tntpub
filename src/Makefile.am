AUTOMAKE_OPTIONS = subdir-objects

AM_CPPFLAGS = -I$(top_srcdir)/include

lib_LTLIBRARIES = libtntpub.la
bin_PROGRAMS = tntpubserver

libtntpub_la_SOURCES = \
	tntpub/datamessage.cpp \
	tntpub/messagesink.cpp \
	tntpub/responder.cpp \
	tntpub/server.cpp \
	tntpub/subscription.cpp \
	tntpub/client.cpp

libtntpub_la_LDFLAGS = -version-info @sonumber@
libtntpub_la_LIBADD = -lcxxtools -lcxxtools-bin

tntpubserver_SOURCES = tntpubserver.cpp
LDADD = libtntpub.la -lcxxtools -lcxxtools-bin
