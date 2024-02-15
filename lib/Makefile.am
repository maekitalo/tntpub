AUTOMAKE_OPTIONS = subdir-objects

lib_LTLIBRARIES = libtntpub.la

libtntpub_la_SOURCES = \
	tntpub/datamessage.cpp \
	tntpub/messagesink.cpp \
	tntpub/responder.cpp \
	tntpub/server.cpp \
	tntpub/subscription.cpp \
	tntpub/client.cpp

libtntpub_la_LDFLAGS = -version-info @sonumber@
libtntpub_la_LIBADD = -lcxxtools -lcxxtools-bin

nobase_include_HEADERS = \
	tntpub/client.h \
	tntpub/datamessage.h \
	tntpub/messagesink.h \
	tntpub/messagesource.h \
	tntpub/messagesinksource.h \
	tntpub/responder.h \
	tntpub/server.h \
	tntpub/serviceprocedure.h \
	tntpub/subscription.h
