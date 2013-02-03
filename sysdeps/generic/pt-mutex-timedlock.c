/* Lock a mutex with a timeout.  Generic version.
   Copyright (C) 2000, 2002, 2005, 2008 Free Software Foundation, Inc.
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

#define LOSE do { * (int *) 0 = 0; } while (1)

/* Try to lock MUTEX, block until *ABSTIME if it is already held.  As
   a GNU extension, if TIMESPEC is NULL then wait forever.  */
int
__pthread_mutex_timedlock_internal (struct __pthread_mutex *mutex,
				    const struct timespec *abstime)
{
  error_t err;
  int drain;
  struct __pthread *self;
  const struct __pthread_mutexattr *attr = mutex->attr;

  if (attr == __PTHREAD_ERRORCHECK_MUTEXATTR)
    attr = &__pthread_errorcheck_mutexattr;
  if (attr == __PTHREAD_RECURSIVE_MUTEXATTR)
    attr = &__pthread_recursive_mutexattr;

  __pthread_spin_lock (&mutex->__lock);
  if (__pthread_spin_trylock (&mutex->__held) == 0)
    /* Successfully acquired the lock.  */
    {
#ifdef ALWAYS_TRACK_MUTEX_OWNER
#ifndef NDEBUG
      self = _pthread_self ();
      if (self)
	/* The main thread may take a lock before the library is fully
	   initialized, in particular, before the main thread has a
	   TCB.  */
	{
	  assert (! mutex->owner);
	  mutex->owner = _pthread_self ();
	}
#endif
#endif

      if (attr)
	switch (attr->mutex_type)
	  {
	  case PTHREAD_MUTEX_NORMAL:
	    break;

	  case PTHREAD_MUTEX_RECURSIVE:
	    mutex->locks = 1;
	  case PTHREAD_MUTEX_ERRORCHECK:
	    mutex->owner = _pthread_self ();
	    break;

	  default:
	    LOSE;
	  }

      __pthread_spin_unlock (&mutex->__lock);
      return 0;
    }

  /* The lock is busy.  */

  self = _pthread_self ();
  assert (self);

  if (! attr || attr->mutex_type == PTHREAD_MUTEX_NORMAL)
    {
#if defined(ALWAYS_TRACK_MUTEX_OWNER)
      assert (mutex->owner != self);
#endif
    }
  else
    {
      switch (attr->mutex_type)
	{
	case PTHREAD_MUTEX_ERRORCHECK:
	  if (mutex->owner == self)
	    {
	      __pthread_spin_unlock (&mutex->__lock);
	      return EDEADLK;
	    }
	  break;

	case PTHREAD_MUTEX_RECURSIVE:
	  if (mutex->owner == self)
	    {
	      mutex->locks ++;
	      __pthread_spin_unlock (&mutex->__lock);
	      return 0;
	    }
	  break;

	default:
	  LOSE;
	}
    }

#if !defined(ALWAYS_TRACK_MUTEX_OWNER)
  if (attr && attr->mutex_type != PTHREAD_MUTEX_NORMAL)
#endif
    assert (mutex->owner);

  if (abstime && (abstime->tv_nsec < 0 || abstime->tv_nsec >= 1000000000))
    return EINVAL;

  /* Add ourselves to the queue.  */
  __pthread_enqueue (&mutex->__queue, self);
  __pthread_spin_unlock (&mutex->__lock);

  /* Block the thread.  */
  if (abstime)
    err = __pthread_timedblock (self, abstime, CLOCK_REALTIME);
  else
    {
      err = 0;
      __pthread_block (self);
    }

  __pthread_spin_lock (&mutex->__lock);
  if (! self->prevp)
    /* Another thread removed us from the queue, which means a wakeup message
       has been sent.  It was either consumed while we were blocking, or
       queued after we timed out and before we acquired the mutex lock, in
       which case the message queue must be drained.  */
    drain = err ? 1 : 0;
  else
    {
      /* We're still in the queue.  Noone attempted to wake us up, i.e. we
	 timed out.  */
      __pthread_dequeue (self);
      drain = 0;
    }
  __pthread_spin_unlock (&mutex->__lock);

  if (drain)
    __pthread_block (self);

  if (err)
    {
      assert (err == ETIMEDOUT);
      return err;
    }

#if !defined(ALWAYS_TRACK_MUTEX_OWNER)
  if (attr && attr->mutex_type != PTHREAD_MUTEX_NORMAL)
#endif
    {
      assert (mutex->owner == self);
    }

  if (attr)
    switch (attr->mutex_type)
      {
      case PTHREAD_MUTEX_NORMAL:
	break;

      case PTHREAD_MUTEX_RECURSIVE:
	assert (mutex->locks == 0);
	mutex->locks = 1;
      case PTHREAD_MUTEX_ERRORCHECK:
	mutex->owner = self;
	break;

      default:
	LOSE;
      }

  return 0;
}

int
pthread_mutex_timedlock (struct __pthread_mutex *mutex,
			 const struct timespec *abstime)
{
  return __pthread_mutex_timedlock_internal (mutex, abstime);
}
