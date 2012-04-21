/* Wait on a condition.  Generic version.
   Copyright (C) 2000, 2002, 2005 Free Software Foundation, Inc.
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

extern int __pthread_cond_timedwait_internal (pthread_cond_t *cond,
					      pthread_mutex_t *mutex,
					      const struct timespec *abstime);

int
__pthread_cond_timedwait (pthread_cond_t *cond,
			pthread_mutex_t *mutex,
			const struct timespec *abstime)
{
  return __pthread_cond_timedwait_internal (cond, mutex, abstime);
}

strong_alias (__pthread_cond_timedwait, pthread_cond_timedwait);

/* Block on condition variable COND until ABSTIME.  As a GNU
   extension, if ABSTIME is NULL, then wait forever.  MUTEX should be
   held by the calling thread.  On return, MUTEX will be held by the
   calling thread.  */
int
__pthread_cond_timedwait_internal (pthread_cond_t *cond,
				   pthread_mutex_t *mutex,
				   const struct timespec *abstime)
{
  error_t err;
  int canceltype;
  clockid_t clock_id = __pthread_default_condattr.clock;

  void cleanup (void *arg)
    {
      struct __pthread *self = _pthread_self ();

      __pthread_spin_lock (&cond->__lock);
      if (self->prevp)
	__pthread_dequeue (self);
      __pthread_spin_unlock (&cond->__lock);

      pthread_setcanceltype (canceltype, &canceltype);
      __pthread_mutex_lock (mutex);
    }

  if (abstime && (abstime->tv_nsec < 0 || abstime->tv_nsec >= 1000000000))
    return EINVAL;

  struct __pthread *self = _pthread_self ();

  /* Add ourselves to the list of waiters.  */
  __pthread_spin_lock (&cond->__lock);
  __pthread_enqueue (&cond->__queue, self);
  if (cond->__attr)
    clock_id = cond->__attr->clock;
  __pthread_spin_unlock (&cond->__lock);

  __pthread_mutex_unlock (mutex);

  /* Enter async cancelation mode.  If cancelation is disabled, then
     this does not change anything which is exactly what we want.  */
  pthread_cleanup_push (cleanup, 0);
  pthread_setcanceltype (PTHREAD_CANCEL_ASYNCHRONOUS, &canceltype);

  if (abstime)
    {
      err = __pthread_timedblock (self, abstime, clock_id);
      if (err)
	/* We timed out.  We may need to disconnect ourself from the
	   waiter queue.

	   FIXME: What do we do if we get a wakeup message before we
	   disconnect ourself?  It may remain until the next time we
	   block.  */
	{
	  assert (err == ETIMEDOUT);

	  __pthread_spin_lock (&mutex->__lock);
	  if (self->prevp)
	    __pthread_dequeue (self);
	  __pthread_spin_unlock (&mutex->__lock);
	}
    }
  else
    {
      err = 0;
      __pthread_block (self);
    }

  pthread_cleanup_pop (1);

  return err;
}
