#! /bin/sh
rm -f config.cache
aclocal -I ./m4
autoconf
autoheader
automake --add-missing
exit 0
