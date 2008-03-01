# headers.m4 - Autoconf snippets to install links for header files.
# Copyright 2003 Free Software Foundation, Inc.
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
  include/pthread.h:libpthread/include/pthread.h
  include/pthread/pthread.h:libpthread/include/pthread/pthread.h
  include/bits/memory.h:libpthread/sysdeps/${arch}/bits/memory.h
  include/bits/spin-lock.h:libpthread/sysdeps/${arch}/bits/spin-lock.h
  include/bits/pthreadtypes.h:libpthread/sysdeps/generic/bits/pthreadtypes.h
  include/bits/barrier-attr.h:libpthread/sysdeps/generic/bits/barrier-attr.h
  include/bits/barrier.h:libpthread/sysdeps/generic/bits/barrier.h
  include/bits/cancelation.h:libpthread/sysdeps/generic/bits/cancelation.h
  include/bits/condition-attr.h:libpthread/sysdeps/generic/bits/condition-attr.h
  include/bits/condition.h:libpthread/sysdeps/generic/bits/condition.h
  include/bits/mutex-attr.h:libpthread/sysdeps/generic/bits/mutex-attr.h
  include/bits/mutex.h:libpthread/sysdeps/generic/bits/mutex.h
  include/bits/once.h:libpthread/sysdeps/generic/bits/once.h
  include/bits/pthread.h:libpthread/sysdeps/generic/bits/pthread.h
  include/bits/rwlock-attr.h:libpthread/sysdeps/generic/bits/rwlock-attr.h
  include/bits/rwlock.h:libpthread/sysdeps/generic/bits/rwlock.h
  include/bits/thread-attr.h:libpthread/sysdeps/generic/bits/thread-attr.h
  include/bits/thread-barrier.h:libpthread/sysdeps/generic/bits/thread-barrier.h
  include/bits/thread-specific.h:libpthread/sysdeps/generic/bits/thread-specific.h
  include/bits/pthread-np.h:libpthread/sysdeps/l4/bits/pthread-np.h
  include/semaphore.h:libpthread/include/semaphore.h
  include/bits/semaphore.h:libpthread/sysdeps/generic/bits/semaphore.h
  include/signal.h:libpthread/signal/signal.h
])
