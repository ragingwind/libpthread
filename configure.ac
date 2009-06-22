# Copyright (C) 2009 Free Software Foundation, Inc.
# 
# This file is part of the POSIX threading library.
#
# This program is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by the
# Free Software Foundation; either version 2, or (at your option) any later
# version.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
# or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
# for more details.
# 
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

m4_include([package.m4])
AC_INIT(
  [AC_PACKAGE_NAME],
  [AC_PACKAGE_VERSION],
  [AC_PACKAGE_BUGREPORT],
  [AC_PACKAGE_TARNAME]
)
AC_CONFIG_SRCDIR([include/pthread/pthread.h])
AC_CONFIG_AUX_DIR([build-aux])

AM_INIT_AUTOMAKE(
dnl Don't define `PACKAGE' and `VERSION'.
  [no-define]
dnl We require GNU make.
  [-Wall -Wno-portability]
)

AC_PROG_CC

AC_CHECK_FUNCS([uselocale])
AC_CHECK_LIB([ihash], [hurd_ihash_create], [:],
  [AC_MSG_ERROR([need libihash])]
)

AC_GNU_SOURCE

AC_CANONICAL_HOST
[arch_ia32=false
arch_powerpc=false
case $host_cpu in
  i386 | i486 | i586 | i686)
    arch_ia32=true
    ;;
  powerpc)
    arch_powerpc=true
    ;;
  *)]
    AC_MSG_ERROR([unsupported architecture])
    [;;
esac]
AM_CONDITIONAL([ARCH_IA32], [$arch_ia32])
AM_CONDITIONAL([ARCH_POWERPC], [$arch_powerpc])
[os_gnu=false
case $host_os in
  gnu*)
    os_gnu=true
    ;;
  *)]
    AC_MSG_ERROR([unsupported system])
    [;;
esac]
AM_CONDITIONAL([OS_GNU], [$os_gnu])
AM_CONDITIONAL([OS_L4], [false])

AM_CONDITIONAL([ENABLE_TESTS], [false])

LT_INIT

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES(
  [Makefile.pre:Makefile.in]
)
# See the comment in `Makefile.am'.
AC_CONFIG_COMMANDS([Makefile],
  [# This is OK to get us going, the Makefile will then do the rest.
  cp Makefile.pre Makefile &&
  rm -f Makefile.stamp]
)

AC_OUTPUT
