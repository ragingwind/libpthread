/* Broadcast a condition.  Generic version.
   Copyright (C) 2000, 2002 Free Software Foundation, Inc.
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

#include <pthread.h>

#include <pt-internal.h>

/* Unblock all threads that are blocked on condition variable COND.  */
int
__pthread_cond_broadcast (pthread_cond_t *cond)
{
  struct __pthread *wakeup;

  __pthread_spin_lock (&cond->__lock);

  wakeup = cond->__queue;
  cond->__queue = NULL;
  __pthread_spin_unlock (&cond->__lock);

  /* We can safely walk the list of waiting threads without holding
     the lock since it is now decoupled from the condition.  */
  __pthread_dequeuing_iterate (wakeup, wakeup)
    __pthread_wakeup (wakeup);

  return 0;
}

strong_alias (__pthread_cond_broadcast, pthread_cond_broadcast);
