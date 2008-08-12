/* Deallocate a thread structure.
   Copyright (C) 2000 Free Software Foundation, Inc.
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
#include <pthread.h>
#include <stdlib.h>

#include <pt-internal.h>

#include <bits/atomic.h>

/* List of thread structures corresponding to free thread IDs.  */
extern __atomicptr_t __pthread_free_threads;

/* Deallocate the thread structure for PTHREAD and the resources
   associated with it.  */
void
__pthread_dealloc (struct __pthread *pthread)
{
  assert (pthread->state != PTHREAD_TERMINATED);

  /* Withdraw this thread from the thread ID lookup table.  */
  __pthread_setid (pthread->thread, NULL);

  /* Mark the thread as terminated.  We broadcast the condition
     here to prevent pthread_join from waiting for this thread to
     exit where it was never really started.  Such a call to
     pthread_join is completely bogus, but unfortunately allowed
     by the standards.  */
  __pthread_mutex_lock (&pthread->state_lock);
  pthread->state = PTHREAD_TERMINATED;
  if (pthread->state != PTHREAD_EXITED)
    pthread_cond_broadcast (&pthread->state_cond);
  __pthread_mutex_unlock (&pthread->state_lock);

  /* We do not actually deallocate the thread structure, but add it to
     a list of re-usable thread structures.  */
  while (1)
    {
      pthread->next = (struct __pthread *)__pthread_free_threads;
      if (__atomicptr_compare_and_swap (&__pthread_free_threads,
					pthread->next, pthread))
	return;
    }

  /* NOTREACHED */
}
