AC_PREREQ(2.53)
AC_INIT(m3ddity-player, 0.1.1, http://www.jcoderz.org/m3dditiez/wiki/m3ddity-player/)
AM_INIT_AUTOMAKE()
AC_CONFIG_SRCDIR(src/m3player.c)
AM_CONFIG_HEADER(config.h)
AM_MAINTAINER_MODE
AC_CANONICAL_HOST

m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

AC_ISC_POSIX
AC_PROG_CC
AM_PROG_CC_C_O
AC_STDC_HEADERS
AC_PROG_LIBTOOL
AC_CONFIG_MACRO_DIR([m4])
AC_FUNC_MMAP
AC_SYS_LARGEFILE

AC_MSG_CHECKING([operating system])
case "$host_os" in
     mingw*|cygwin*)
        AC_MSG_RESULT(Win32)
        UUID=uuid
        ;;
     darwin*)
        AC_MSG_RESULT(Darwin)
        UUID=
        ;;
     *)
        AC_MSG_RESULT($host_os)
        UUID=uuid
        ;;
esac

PKG_CHECK_MODULES(LIBGUPNP, gupnp-1.0 >= 0.13)
PKG_CHECK_MODULES(LIBGUPNPAV, gupnp-av-1.0 >= 0.8)

# Debugging
AC_ARG_ENABLE(debug,
	[  --enable-debug          enable debugging],,
        enable_debug=no)
if test "x$enable_debug" = "xyes"; then
        CFLAGS="$CFLAGS -g -Wall -Werror"
fi

# Gtk-doc
GTK_DOC_CHECK([1.0])

PKG_CHECK_MODULES(GST, gstreamer-0.10)
AC_OUTPUT([
Makefile
src/Makefile
])
