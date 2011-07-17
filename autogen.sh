#! /bin/sh
gtkdocize || exit 1
ACLOCAL="${ACLOCAL-aclocal} $ACLOCAL_FLAGS" autoreconf -v -f --install || exit 1
./configure --enable-maintainer-mode --enable-debug "$@"
