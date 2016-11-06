AUTOMAKE_OPTIONS = subdir-objects

lib_LTLIBRARIES = libtntpub.la

libtntpub_la_SOURCES = \
	tntpub/client.cpp \
	tntpub/datamessage.cpp \
	tntpub/subscribemessage.cpp

libtntpub_la_LDFLAGS = -version-info @sonumber@
libtntpub_la_LIBADD = -lcxxtools -lcxxtools-bin

nobase_include_HEADERS = \
	tntpub/datamessage.h \
	tntpub/subscribemessage.h
