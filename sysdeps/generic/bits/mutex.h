/* Mutex type.  Generic version.
   Copyright (C) 2000,02 Free Software Foundation, Inc.
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

#ifndef _BITS_MUTEX_H

#ifndef __need_pthread_mutex
# define _BITS_MUTEX_H	1
#endif

#ifndef __pthread_mutex_defined
# if defined __need_pthread_mutex || defined _BITS_MUTEX_H
#  undef __need_pthread_mutex
#  define __pthread_mutex_defined

#  include <bits/spin-lock.h>
#  include <bits/mutex-attr.h>

/* User visible part of a mutex.  */
struct __pthread_mutex
  {
    __pthread_spinlock_t __held;
    __pthread_spinlock_t __lock;
    /* In cthreads, mutex_init does not initialized thre third
       pointer, as such, we cannot rely on its value for anything.  */
    char *cthreadscompat1;
    struct __pthread *__queue;
    struct __pthread_mutexattr *attr;
    void *data;
    /*  Up to this point, we are completely compatible with cthreads
	and what libc expects.  */
    void *owner;
    unsigned locks;
    /* If NULL then the default attributes apply.  */
  };

/* Initializer for a mutex.  N.B.  this also happens to be compatible
   with the cthread mutex initializer.  */
#  define __PTHREAD_MUTEX_INITIALIZER \
      { __SPIN_LOCK_INITIALIZER, __SPIN_LOCK_INITIALIZER, NULL, NULL, NULL, \
	NULL, 0, 0 }

# endif
#endif /* Not __pthread_mutex_defined.  */

#ifdef _BITS_MUTEX_H

#include <errno.h>
#include <stddef.h>

#ifdef __USE_EXTERN_INLINES

# ifndef _EXTERN_INLINE
#  define _EXTERN_INLINE extern __inline
# endif

_EXTERN_INLINE int
pthread_mutex_init (struct __pthread_mutex *__mutex,
		    const pthread_mutexattr_t *attr)
{
  extern int _pthread_mutex_init (struct __pthread_mutex *,
				  const pthread_mutexattr_t *);

  if (attr)
    return _pthread_mutex_init (__mutex, attr);

  *__mutex = (struct __pthread_mutex) __PTHREAD_MUTEX_INITIALIZER;
  return 0;
}

_EXTERN_INLINE int
pthread_mutex_destroy (struct __pthread_mutex *__mutex)
{
  extern int _pthread_mutex_destroy (struct __pthread_mutex *);

  if (__mutex->attr || __mutex->data)
    return _pthread_mutex_destroy (__mutex);

  return 0;
}

_EXTERN_INLINE int
__pthread_mutex_lock (struct __pthread_mutex *__mutex)
{
  extern int _pthread_mutex_lock (struct __pthread_mutex *);
  
  if (__mutex->attr == NULL
      && __mutex->data == NULL
      && __pthread_spin_trylock (&__mutex->__held) == 0)
    return 0;

  return _pthread_mutex_lock (__mutex);
}

extern inline int
__pthread_mutex_trylock (struct __pthread_mutex *__mutex)
{
  extern int _pthread_mutex_trylock (struct __pthread_mutex *);
  
  if (__mutex->attr == NULL
      && __mutex->data == NULL)
    return __pthread_spin_trylock (&__mutex->__held);

  return _pthread_mutex_trylock (__mutex);
}

extern inline int
pthread_mutex_lock (struct __pthread_mutex *__mutex)
{
  return __pthread_mutex_lock (__mutex);
}

extern inline int
pthread_mutex_trylock (struct __pthread_mutex *__mutex)
{
  return __pthread_mutex_trylock (__mutex);
}

#endif /* Use extern inlines.  */

#endif

#endif /* bits/mutex.h */
