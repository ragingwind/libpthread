/* Unlock a mutex.  Generic version.
   Copyright (C) 2000, 2002, 2008 Free Software Foundation, Inc.
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

#define LOSE do { * (int *) 0 = 0; } while (1)

/* Unlock MUTEX, rescheduling a waiting thread.  */
int
__pthread_mutex_unlock (pthread_mutex_t *mutex)
{
  struct __pthread *wakeup;
  
  __pthread_spin_lock (&mutex->__lock);

  if (! mutex->attr || mutex->attr->mutex_type == PTHREAD_MUTEX_NORMAL)
    {
#ifndef NDEBUG
      if (_pthread_self ())
	{
	  assert (mutex->owner);
	  assertx (mutex->owner == _pthread_self (),
		   "%p(%x) != %p(%x)",
		   mutex->owner,
		   ((struct __pthread *) mutex->owner)->threadid,
		   _pthread_self (),
		   _pthread_self ()->threadid);
	  mutex->owner = NULL;
	}
#endif
    }
  else
    switch (mutex->attr->mutex_type)
      {
      case PTHREAD_MUTEX_ERRORCHECK:
      case PTHREAD_MUTEX_RECURSIVE:
	if (mutex->owner != _pthread_self ())
	  {
	    __pthread_spin_unlock (&mutex->__lock);
	    return EPERM;
	  }

	if (mutex->attr->mutex_type == PTHREAD_MUTEX_RECURSIVE)
	  if (--mutex->locks > 0)
	    {
	      __pthread_spin_unlock (&mutex->__lock);
	      return 0;
	    }

	mutex->owner = 0;
	break;

      default:
	LOSE;
      }


  if (mutex->__queue == NULL)
    {
      __pthread_spin_unlock (&mutex->__held);
      __pthread_spin_unlock (&mutex->__lock);
      return 0;
    }

  wakeup = mutex->__queue;
  __pthread_dequeue (wakeup);

#ifndef NDEBUG
  mutex->owner = wakeup;
#endif

  /* We do not unlock MUTEX->held: we are transferring the ownership
     to the thread that we are waking up.  */

  __pthread_spin_unlock (&mutex->__lock);
  __pthread_wakeup (wakeup);

  return 0;
}

strong_alias (__pthread_mutex_unlock, _pthread_mutex_unlock);
strong_alias (__pthread_mutex_unlock, pthread_mutex_unlock);
