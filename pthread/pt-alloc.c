/* Allocate a new thread structure.
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

#include <assert.h>
#include <errno.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#include <pt-internal.h>

#include <bits/atomic.h>

/* This braindamage is necessary because the standard says that some
   of the threads functions "shall fail" if "No thread could be found
   corresponding to that specified by the given thread ID."  */
   
/* Thread ID lookup table.  */
struct __pthread **__pthread_threads;

/* The size of the thread ID lookup table.  */
int __pthread_max_threads;

/* The total number of thread IDs currently in use, or on the list of
   available thread IDs.  */
int __pthread_num_threads;

/* A lock for the table, and the other variables above.  */
pthread_rwlock_t __pthread_threads_lock;


/* List of thread structures corresponding to free thread IDs.  */
__atomicptr_t __pthread_free_threads;

static inline error_t
initialize_pthread (struct __pthread *new, int recycling)
{
  error_t err;

  err = __pthread_init_specific (new);
  if (err)
    return err;

  new->cancel_state = PTHREAD_CANCEL_ENABLE;
  new->cancel_type = PTHREAD_CANCEL_DEFERRED;
  new->cancel_pending = 0;

  if (recycling)
    /* Since we are recycling PTHREAD, we can assume certains things
       about PTHREAD's current state and save some cycles by not
       rewriting the memory.  */
    return 0;

  new->stack = 0;

  new->state_lock = (struct __pthread_mutex) PTHREAD_MUTEX_INITIALIZER;
  new->state_cond = PTHREAD_COND_INITIALIZER;

  new->cancelation_handlers = 0;

  new->next = 0;
  new->prevp = 0;

  return 0;
}


/* Allocate a new thread structure and its pthread thread ID (but not
   a kernel thread).  */
int
__pthread_alloc (struct __pthread **pthread)
{
  error_t err;

  struct __pthread *new;
  struct __pthread **threads;
  struct __pthread **old_threads;
  int max_threads;
  int new_max_threads;

  /* Try to re-use a thread structure before creating a new one.  */
  while ((new = (struct __pthread *)__pthread_free_threads))
    {
      if (__atomicptr_compare_and_swap (&__pthread_free_threads,
					new, new->next))
	{
	  /* Yes, we managed to get one.  The thread number in the
             thread structure still refers to the correct slot.  */
	  err = initialize_pthread (new, 1);
	  if (err)
	    /* An error occured, however, we cannot just free NEW as
	       there may be resources attached to it.  We must return
	       it to the free list.  */
	    while (1)
	      {
		new->next = (struct __pthread *)__pthread_free_threads;
		if (__atomicptr_compare_and_swap (&__pthread_free_threads,
						  new->next, new))
		  break;
	      }

	  if (! err)
	    *pthread = new;

	  return err;
	}
    }

  /* Allocate a new thread structure.  */
  new = malloc (sizeof (struct __pthread));
  if (new == NULL)
    return ENOMEM;

  err = initialize_pthread (new, 0);
  if (err)
    {
      free (new);
      return err;
    }

 retry:
  pthread_rwlock_wrlock (&__pthread_threads_lock);

  if (__pthread_num_threads < __pthread_max_threads)
    {
      /* We have a free slot.  Use the slot number as
         the thread ID for the new thread.  */
      new->thread = __pthread_num_threads++;
      __pthread_threads[new->thread] = NULL;

      pthread_rwlock_unlock (&__pthread_threads_lock);

      *pthread = new;
      return 0;
    }
#ifdef PTHREAD_THREADS_MAX
  else if (__pthread_num_threads >= PTHREAD_THREADS_MAX)
    {
      /* We have reached the limit on the number of threads per process.  */
      pthread_rwlock_unlock (&__pthread_threads_lock);

      free (new);
      return EAGAIN;
    }
#endif

  /* We are going to enlarge the threads table.  Save its current
     size.  We're going to release the lock before doing the necessary
     memory allocation, since that's a potentially blocking operation.  */
  max_threads = __pthread_max_threads;

  pthread_rwlock_unlock (&__pthread_threads_lock);

  /* Allocate a new lookup table that's twice as large.  */
  new_max_threads
    = max_threads > 0 ? max_threads * 2 : _POSIX_THREAD_THREADS_MAX;
  threads = malloc (new_max_threads * sizeof (struct __pthread *));
  if (threads == NULL)
    {
      free (new);
      return ENOMEM;
    }

  pthread_rwlock_wrlock (&__pthread_threads_lock);

  /* Check if nobody else has already enlarged the table.  */
  if (max_threads != __pthread_max_threads)
    {
      /* Yep, they did.  */
      pthread_rwlock_unlock (&__pthread_threads_lock);

      /* Free the newly allocated table and try again to allocate a slot.  */
      free (threads);
      goto retry;
    }

  /* Copy over the contents of the old table.  */
  memcpy (threads, __pthread_threads,
	  __pthread_max_threads * sizeof (struct __pthread *));

  /* Save the location of the old table.  We want to deallocate its
     storage after we released the lock.  */
  old_threads = __pthread_threads;

  /* Replace the table with the new one.  */
  __pthread_max_threads = new_max_threads;
  __pthread_threads = threads;

  /* And allocate ourselves one of the newly created slots.  */
  new->thread = __pthread_num_threads++;
  __pthread_threads[new->thread] = NULL;

  pthread_rwlock_unlock (&__pthread_threads_lock);

  free (old_threads);

  *pthread = new;
  return 0;
}
