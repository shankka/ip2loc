dnl $Id$
dnl config.m4 for extension ip2loc

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

dnl PHP_ARG_WITH(ip2loc, for ip2loc support,
dnl Make sure that the comment is aligned:
dnl [  --with-ip2loc             Include ip2loc support])

dnl Otherwise use enable:

PHP_ARG_ENABLE(ip2loc, whether to enable ip2loc support,
dnl Make sure that the comment is aligned:
[  --enable-ip2loc           Enable ip2loc support])

if test "$PHP_IP2LOC" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-ip2loc -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/ip2loc.h"  # you most likely want to change this
  dnl if test -r $PHP_IP2LOC/$SEARCH_FOR; then # path given as parameter
  dnl   IP2LOC_DIR=$PHP_IP2LOC
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for ip2loc files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       IP2LOC_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$IP2LOC_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the ip2loc distribution])
  dnl fi

  dnl # --with-ip2loc -> add include path
  dnl PHP_ADD_INCLUDE($IP2LOC_DIR/include)

  dnl # --with-ip2loc -> check for lib and symbol presence
  dnl LIBNAME=ip2loc # you may want to change this
  dnl LIBSYMBOL=ip2loc # you most likely want to change this 

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $IP2LOC_DIR/lib, IP2LOC_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_IP2LOCLIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong ip2loc lib version or lib not found])
  dnl ],[
  dnl   -L$IP2LOC_DIR/lib -lm
  dnl ])
  dnl
  dnl PHP_SUBST(IP2LOC_SHARED_LIBADD)

  PHP_NEW_EXTENSION(ip2loc, ip2loc.c, $ext_shared)
fi
