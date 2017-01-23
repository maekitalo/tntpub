AUTOMAKE_OPTIONS = subdir-objects

lib_LTLIBRARIES = libtntpub.la

libtntpub_la_SOURCES = \
	tntpub/client.cpp \
	tntpub/datamessage.cpp \
	tntpub/responder.cpp \
	tntpub/server.cpp \
	tntpub/subscribemessage.cpp \
	tntpub/unsubscribemessage.cpp

libtntpub_la_LDFLAGS = -version-info @sonumber@
libtntpub_la_LIBADD = -lcxxtools -lcxxtools-bin

nobase_include_HEADERS = \
	tntpub/client.h \
	tntpub/datamessage.h \
	tntpub/responder.h \
	tntpub/server.h \
	tntpub/subscribemessage.h \
	tntpub/tcpclient.h \
	tntpub/unsubscribemessage.h
