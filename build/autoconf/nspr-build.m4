dnl This Source Code Form is subject to the terms of the Mozilla Public
dnl License, v. 2.0. If a copy of the MPL was not distributed with this
dnl file, You can obtain one at http://mozilla.org/MPL/2.0/.

AC_DEFUN([MOZ_CONFIG_NSPR], [

ifelse([$1],,define(CONFIGURING_JS,yes))

dnl Possible ways this can be called:
dnl   from toplevel configure:
dnl     JS_STANDALONE=  MOZ_BUILD_APP!=js
dnl   from js/src/configure invoked by toplevel configure:
dnl     JS_STANDALONE=  MOZ_BUILD_APP=js
dnl   from standalone js/src/configure:
dnl     JS_STANDALONE=1 MOZ_BUILD_APP=js

dnl ========================================================
dnl = Find the right NSPR to use.
dnl ========================================================
MOZ_ARG_WITH_STRING(nspr-cflags,
[  --with-nspr-cflags=FLAGS
                          Pass FLAGS to CC when building code that uses NSPR.
                          Use this when there's no accurate nspr-config
                          script available.  This is the case when building
                          SpiderMonkey as part of the Mozilla tree: the
                          top-level configure script computes NSPR flags
                          that accomodate the quirks of that environment.],
    NSPR_CFLAGS=$withval)
MOZ_ARG_WITH_STRING(nspr-libs,
[  --with-nspr-libs=LIBS   Pass LIBS to LD when linking code that uses NSPR.
                          See --with-nspr-cflags for more details.],
    NSPR_LIBS=$withval)

if test "$MOZ_BUILD_APP" != js || test -n "$JS_STANDALONE"; then
  _IS_OUTER_CONFIGURE=1
fi

MOZ_ARG_WITH_BOOL(system-nspr,
[  --with-system-nspr      Use an NSPR that is already built and installed.
                          Use the 'nspr-config' script in the current path,
                          or look for the script in the directories given with
                          --with-nspr-exec-prefix or --with-nspr-prefix.
                          (Those flags are only checked if you specify
                          --with-system-nspr.)],
    _USE_SYSTEM_NSPR=1 )

dnl Pass at most one of
dnl   --with-system-nspr
dnl   --with-nspr-cflags/libs

AC_MSG_CHECKING([NSPR selection])
nspr_opts=
which_nspr=default
if test -n "$_USE_SYSTEM_NSPR"; then
    nspr_opts="x$nspr_opts"
    which_nspr="system"
fi
if test -n "$NSPR_CFLAGS" -o -n "$NSPR_LIBS"; then
    nspr_opts="x$nspr_opts"
    which_nspr="command-line"
fi

if test -z "$nspr_opts"; then
    if test "$MOZ_BUILD_APP" != js; then
      dnl Toplevel configure defaults to using nsprpub from the source tree
      which_nspr="source-tree"
    else
      dnl JS configure defaults to emulated NSPR if available, falling back
      dnl to nsprpub.
      which_nspr="source-tree"
   fi
fi

if test -z "$nspr_opts" || test "$nspr_opts" = x; then
    AC_MSG_RESULT($which_nspr)
else
    AC_MSG_ERROR([only one way of using NSPR may be selected. See 'configure --help'.])
fi

# A (sub)configure invoked by the toplevel configure will always receive
# --with-nspr-libs on the command line. It will never need to figure out
# anything itself.
if test -n "$_IS_OUTER_CONFIGURE"; then

if test -n "$_USE_SYSTEM_NSPR"; then
    AM_PATH_NSPR($NSPR_MINVER, [MOZ_SYSTEM_NSPR=1], [AC_MSG_ERROR([you do not have NSPR installed or your version is older than $NSPR_MINVER.])])
fi

if test -n "$MOZ_SYSTEM_NSPR" -o -n "$NSPR_CFLAGS" -o -n "$NSPR_LIBS"; then
    _SAVE_CFLAGS=$CFLAGS
    CFLAGS="$CFLAGS $NSPR_CFLAGS"
    AC_TRY_COMPILE([#include "prtypes.h"],
                [#ifndef PR_STATIC_ASSERT
                 #error PR_STATIC_ASSERT not defined or requires including prtypes.h
                 #endif],
                ,
                AC_MSG_ERROR([system NSPR does not support PR_STATIC_ASSERT or including prtypes.h does not provide it]))
    AC_TRY_COMPILE([#include "prtypes.h"],
                [#ifndef PR_UINT64
                 #error PR_UINT64 not defined or requires including prtypes.h
                 #endif],
                ,
                AC_MSG_ERROR([system NSPR does not support PR_UINT64 or including prtypes.h does not provide it]))
    CFLAGS=$_SAVE_CFLAGS
fi

AC_SUBST_LIST(NSPR_CFLAGS)

fi # _IS_OUTER_CONFIGURE

])
