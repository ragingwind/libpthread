/* pthread_barrier_wait.  Generic version.
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

#include <pthread.h>
#include <assert.h>

#include <pt-internal.h>

int
pthread_barrier_wait (pthread_barrier_t *barrier)
{
  __pthread_spin_lock (&barrier->lock);
  if (-- barrier->pending == 0)
    {
      barrier->pending = barrier->count;

      if (barrier->count > 1)
	{
	  struct __pthread *wakeup;

	  wakeup = barrier->queue;
	  barrier->queue = NULL;
	  __pthread_spin_unlock (&barrier->lock);

	  /* We can safely walk the list of waiting threads without
	     holding the lock since it is decoupled from the barrier
	     variable now.  */
	  __pthread_dequeuing_iterate (wakeup, wakeup)
	    __pthread_wakeup (wakeup);
	}

      return PTHREAD_BARRIER_SERIAL_THREAD;
    }
  else
    {
      struct __pthread *self = _pthread_self ();

      /* Add ourselves to the list of waiters.  */
      __pthread_enqueue (&barrier->queue, self);
      __pthread_spin_unlock (&barrier->lock);

      __pthread_block (self);
      return 0;
    }
}
