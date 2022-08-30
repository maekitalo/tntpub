AC_INIT([tntpub],[0.5],[Tommi Maekitalo <tommi@tntnet.org>])
AM_INIT_AUTOMAKE
LT_INIT

AC_CONFIG_MACRO_DIR([m4])

abi_current=0
abi_revision=0
abi_age=0
sonumber=${abi_current}:${abi_revision}:${abi_age}
AC_SUBST(sonumber)

AC_PROG_CXX
AC_LANG(C++)
AX_CXX_COMPILE_STDCXX_11(noext)

AC_CONFIG_HEADERS([config.h])
AC_CHECK_HEADER([cxxtools/mutex.h], , AC_MSG_ERROR([cxxtools headers not found]))
AX_CHECK_COMPILE_FLAG([-Wall], [CPPFLAGS="$CPPFLAGS -Wall"])
AX_CHECK_COMPILE_FLAG([-pedantic], [CPPFLAGS="$CPPFLAGS -pedantic"])

AC_CONFIG_FILES([
    Makefile
    lib/Makefile
    server/Makefile
    demo/Makefile
    pkgconfig/tntpub.pc
])
AC_OUTPUT
