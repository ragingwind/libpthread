/* Unlock a rwlock.  Generic version.
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

#include <pthread.h>

#include <pt-internal.h>

/* Unlock *RWLOCK, rescheduling a waiting writer thread or, if there
   are no threads waiting for a write lock, rescheduling the reader
   threads.  */
int
pthread_rwlock_unlock (pthread_rwlock_t *rwlock)
{
  struct __pthread *wakeup;
  
  __pthread_spin_lock (&rwlock->__lock);

  assert (__pthread_spin_trylock (&rwlock->__held) == EBUSY);

  if (rwlock->readers > 1)
    /* There are other readers.  */
    {
      rwlock->readers --;
      __pthread_spin_unlock (&rwlock->__lock);
      return 0;
    }

  if (rwlock->readers == 1)
    /* Last reader.  */
    rwlock->readers = 0;
      

  /* Wake someone else up.  Try the writer queue first, then the
     reader queue if that is empty.  */

  if (rwlock->writerqueue)
    {
      wakeup = rwlock->writerqueue;
      __pthread_dequeue (wakeup);

      /* We do not unlock RWLOCK->held: we are transferring the ownership
	 to the thread that we are waking up.  */

      __pthread_spin_unlock (&rwlock->__lock);
      __pthread_wakeup (wakeup);

      return 0;
    }

  if (rwlock->readerqueue)
    {
      __pthread_queue_iterate (rwlock->readerqueue, wakeup)
	rwlock->readers ++;

      wakeup = rwlock->readerqueue;
      rwlock->readerqueue = 0;

      __pthread_spin_unlock (&rwlock->__lock);

      /* We can safely walk the list of waiting threads without holding
	 the lock since it is now decoupled from the rwlock.  */
      __pthread_dequeuing_iterate (wakeup, wakeup)
	__pthread_wakeup (wakeup);

      return 0;
    }


  /* Noone is waiting.  Just unlock it.  */

  __pthread_spin_unlock (&rwlock->__held);
  __pthread_spin_unlock (&rwlock->__lock);
  return 0;
}
