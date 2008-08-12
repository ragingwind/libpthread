# headers.m4 - Autoconf snippets to install links for header files.
# Copyright 2003, 2008 Free Software Foundation, Inc.
# Written by Marcus Brinkmann <marcus@gnu.org>.
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This file is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

AC_CONFIG_LINKS([
  sysroot/include/pthread.h:libpthread/include/pthread.h
  sysroot/include/pthread/pthread.h:libpthread/include/pthread/pthread.h
  sysroot/include/pthread/pthreadtypes.h:libpthread/include/pthread/pthreadtypes.h
  sysroot/include/bits/memory.h:libpthread/sysdeps/${arch}/bits/memory.h
  sysroot/include/bits/spin-lock.h:libpthread/sysdeps/${arch}/bits/spin-lock.h
  sysroot/include/bits/spin-lock-inline.h:libpthread/sysdeps/${arch}/bits/spin-lock-inline.h
  sysroot/include/bits/pthreadtypes.h:libpthread/sysdeps/generic/bits/pthreadtypes.h
  sysroot/include/bits/barrier-attr.h:libpthread/sysdeps/generic/bits/barrier-attr.h
  sysroot/include/bits/barrier.h:libpthread/sysdeps/generic/bits/barrier.h
  sysroot/include/bits/cancelation.h:libpthread/sysdeps/generic/bits/cancelation.h
  sysroot/include/bits/condition-attr.h:libpthread/sysdeps/generic/bits/condition-attr.h
  sysroot/include/bits/condition.h:libpthread/sysdeps/generic/bits/condition.h
  sysroot/include/bits/mutex-attr.h:libpthread/sysdeps/generic/bits/mutex-attr.h
  sysroot/include/bits/mutex.h:libpthread/sysdeps/generic/bits/mutex.h
  sysroot/include/bits/once.h:libpthread/sysdeps/generic/bits/once.h
  sysroot/include/bits/pthread.h:libpthread/sysdeps/generic/bits/pthread.h
  sysroot/include/bits/rwlock-attr.h:libpthread/sysdeps/generic/bits/rwlock-attr.h
  sysroot/include/bits/rwlock.h:libpthread/sysdeps/generic/bits/rwlock.h
  sysroot/include/bits/thread-attr.h:libpthread/sysdeps/generic/bits/thread-attr.h
  sysroot/include/bits/thread-barrier.h:libpthread/sysdeps/generic/bits/thread-barrier.h
  sysroot/include/bits/thread-specific.h:libpthread/sysdeps/generic/bits/thread-specific.h
  sysroot/include/bits/pthread-np.h:libpthread/sysdeps/l4/hurd/bits/pthread-np.h
  sysroot/include/semaphore.h:libpthread/include/semaphore.h
  sysroot/include/bits/semaphore.h:libpthread/sysdeps/generic/bits/semaphore.h
  sysroot/include/signal.h:libpthread/signal/signal.h
])

AC_CONFIG_COMMANDS_POST([
  mkdir -p sysroot/lib libpthread &&
  ln -sf ../../libpthread/libpthread.a sysroot/lib/ &&
  touch libpthread/libpthread.a
])
