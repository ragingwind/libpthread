/* Machine-specific definitions for spin locks.  PowerPC version.
   Copyright (C) 2003 Free Software Foundation, Inc.
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

/*
 * Never include this file directly; use <pthread.h> or <cthreads.h> instead.
 */

#ifndef _BITS_SPIN_LOCK_H
#define _BITS_SPIN_LOCK_H	1

#include <features.h>

__BEGIN_DECLS

/* The type of a spin lock object.  */
typedef __volatile int __pthread_spinlock_t;

/* Initializer for a spin lock object.  */
# define __SPIN_LOCK_INITIALIZER ((__pthread_spinlock_t) 0)

#if defined __USE_EXTERN_INLINES || defined _FORCE_INLINES

# ifndef __EBUSY
#  include <errno.h>
#  define __EBUSY EBUSY
# endif

# ifndef __PT_SPIN_INLINE
#  define __PT_SPIN_INLINE extern __inline
# endif

__PT_SPIN_INLINE int __pthread_spin_destroy (__pthread_spinlock_t *__lock);

__PT_SPIN_INLINE int
__pthread_spin_destroy (__pthread_spinlock_t *__lock)
{
  return 0;
}

__PT_SPIN_INLINE int __pthread_spin_init (__pthread_spinlock_t *__lock,
					  int __pshared);

__PT_SPIN_INLINE int
__pthread_spin_init (__pthread_spinlock_t *__lock, int __pshared)
{
  *__lock = __SPIN_LOCK_INITIALIZER;
  return 0;
}

__PT_SPIN_INLINE int __pthread_spin_trylock (__pthread_spinlock_t *__lock);

__PT_SPIN_INLINE int
__pthread_spin_trylock (__pthread_spinlock_t *__lock)
{
  long int __rtn;
  __asm__ __volatile__ ("\
0:	lwarx	%0,0,%1\n\
	stwcx.	%2,0,%1\n\
	bne-	0b\n\
" : "=&r" (__rtn) : "r" (__lock), "r" (1) : "cr0");
  return __rtn ? __EBUSY : 0;
}

extern inline int __pthread_spin_lock (__pthread_spinlock_t *__lock);
extern int _pthread_spin_lock (__pthread_spinlock_t *__lock);

extern inline int
__pthread_spin_lock (__pthread_spinlock_t *__lock)
{
  if (__pthread_spin_trylock (__lock))
    return _pthread_spin_lock (__lock);
  return 0;
}

__PT_SPIN_INLINE int __pthread_spin_unlock (__pthread_spinlock_t *__lock);

__PT_SPIN_INLINE int
__pthread_spin_unlock (__pthread_spinlock_t *__lock)
{
  long int __locked;
  __asm__ __volatile__ ("\
0:	lwarx	%0,0,%1\n\
	stwcx.	%2,0,%1\n\
	bne-	0b\n\
" : "=&r" (__locked) : "r" (__lock), "r" (0) : "cr0");
}

#endif /* Use extern inlines or force inlines.  */

__END_DECLS

#endif /* bits/spin-lock.h */
