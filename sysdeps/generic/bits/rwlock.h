/* rwlock type.  Generic version.
   Copyright (C) 2002 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with the GNU C Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

#ifndef _BITS_RWLOCK_H
#define _BITS_RWLOCK_H

#include <bits/spin-lock.h>

/* User visible part of a rwlock.  If __held is not held and readers
   is 0, then the lock is unlocked.  If __held is held and readers is
   0, then the lock is held by a writer.  If __held is held and
   readers is greater than 0, then the lock is held by READERS
   readers.  */
struct __pthread_rwlock
  {
    __pthread_spinlock_t __held;
    __pthread_spinlock_t __lock;
    int readers;
    struct __pthread *readerqueue;
    struct __pthread *writerqueue;
    struct __pthread_rwlockattr *__attr;
    void *__data;
  };

/* Initializer for a rwlock.  */
#define __PTHREAD_RWLOCK_INITIALIZER \
    ((struct __pthread_rwlock) \
      { __SPIN_LOCK_INITIALIZER, __SPIN_LOCK_INITIALIZER, 0, 0, 0, 0, 0 })


_EXTERN_INLINE int
pthread_rwlock_init (struct __pthread_rwlock *__rwlock,
		     const struct __pthread_rwlockattr *__attr)
{
  extern int _pthread_rwlock_init (struct __pthread_rwlock *,
				   const struct __pthread_rwlockattr *);

  if (__attr)
    return _pthread_rwlock_init (__rwlock, __attr);

  *__rwlock = __PTHREAD_RWLOCK_INITIALIZER;
  return 0;
}

_EXTERN_INLINE int
pthread_rwlock_destroy (struct __pthread_rwlock *__rwlock)
{
  extern int _pthread_rwlock_destroy (struct __pthread_rwlock *);

  if (__rwlock->__attr
      || __rwlock->__data)
    return _pthread_rwlock_destroy (__rwlock);

  return 0;
}

#endif /* bits/rwlock.h */
