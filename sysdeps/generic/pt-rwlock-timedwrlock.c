/* Acquire a rwlock for writing.  Generic version.
   Copyright (C) 2002, 2005 Free Software Foundation, Inc.
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

/* Acquire RWLOCK for writing blocking until *ABSTIME if we cannot get
   it.  As a special GNU extension, if ABSTIME is NULL then the wait
   shall not time out.  */
int
__pthread_rwlock_timedwrlock_internal (struct __pthread_rwlock *rwlock,
				       const struct timespec *abstime)
{
  struct __pthread *self;

  __pthread_spin_lock (&rwlock->__lock);
  if (__pthread_spin_trylock (&rwlock->__held) == 0)
    /* Successfully acquired the lock.  */
    {
      assert (rwlock->readerqueue == 0);
      assert (rwlock->writerqueue == 0);
      assert (rwlock->readers == 0);

      __pthread_spin_unlock (&rwlock->__lock);
      return 0;
    }

  /* The lock is busy.  */

  if (abstime && (abstime->tv_nsec < 0 || abstime->tv_nsec >= 1000000000))
    return EINVAL;

  self = _pthread_self ();

  /* Add ourselves to the queue.  */
  __pthread_enqueue (&rwlock->writerqueue, self);
  __pthread_spin_unlock (&rwlock->__lock);

  /* Block the thread.  */
  if (abstime)
    {
      error_t err;

      err = __pthread_timedblock (self, abstime, CLOCK_REALTIME);
      if (err)
	/* We timed out.  We may need to disconnect ourself from the
	   waiter queue.

	   FIXME: What do we do if we get a wakeup message before we
	   disconnect ourself?  It may remain until the next time we
	   block.  */
	{
	  assert (err == ETIMEDOUT);

	  __pthread_spin_lock (&rwlock->__lock);
	  if (self->prevp)
	    /* Disconnect ourself.  */
	    __pthread_dequeue (self);
	  __pthread_spin_unlock (&rwlock->__lock);

	  return err;
	}
    }
  else
    __pthread_block (self);

  assert (rwlock->readers == 0);

  return 0;
}

int
pthread_rwlock_timedwrlock (struct __pthread_rwlock *rwlock,
			    const struct timespec *abstime)
{
  return __pthread_rwlock_timedwrlock_internal (rwlock, abstime);
}
