/* Thread termination.
   Copyright (C) 2000, 2002, 2004 Free Software Foundation, Inc.
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

#include <pt-internal.h>

#include <atomic.h>


/* Terminate the current thread and make STATUS available to any
   thread that might join us.  */
void
pthread_exit (void *status)
{
  struct __pthread *self = _pthread_self ();
  struct __pthread_cancelation_handler **handlers;
  int oldstate;

  /* Run any cancelation handlers.  According to POSIX, the
     cancellation cleanup handlers should be called with cancellation
     disabled.  */
  pthread_setcancelstate (PTHREAD_CANCEL_DISABLE, &oldstate);

  for (handlers = __pthread_get_cleanup_stack ();
       *handlers;
       *handlers = (*handlers)->next)
    (*handlers)->handler ((*handlers)->arg);

  pthread_setcancelstate (oldstate, &oldstate);

  /* Destory any thread specific data.  */
  __pthread_destroy_specific (self);

  /* Destroy any signal state.  */
  __pthread_sigstate_destroy (self);

  /* Decrease the number of threads.  We use an atomic operation to
     make sure that only the last thread calls `exit'.  */
  if (atomic_decrement_and_test (&__pthread_total))
    /* We are the last thread.  */
    exit (0);

  /* Note that after this point the process can be terminated at any
     point if another thread calls `pthread_exit' and happens to be
     the last thread.  */

  __pthread_mutex_lock (&self->state_lock);

  if (self->cancel_state == PTHREAD_CANCEL_ENABLE && self->cancel_pending)
    status = PTHREAD_CANCELED;

  switch (self->state)
    {
    default:
      assert (! "This cannot happen!");
      break;

    case PTHREAD_DETACHED:
      /* Make sure that nobody can reference this thread anymore, and
         mark it as terminated.  Our thread ID will immediately become
         available for re-use.  For obvious reasons, we cannot
         deallocate our own stack.  However, it will eventually be
         reused when this thread structure is recycled.  */
      __pthread_mutex_unlock (&self->state_lock);
      __pthread_dealloc (self);

      break;

    case PTHREAD_JOINABLE:
      /* We need to stay around for a while since another thread
         might want to join us.  */
      self->state = PTHREAD_EXITED;

      /* We need to remember the exit status.  A thread joining us
         might ask for it.  */
      self->status = status;

      /* Broadcast the condition.  This will wake up threads that are
         waiting to join us.  */
      pthread_cond_broadcast (&self->state_cond);
      __pthread_mutex_unlock (&self->state_lock);

      break;
    }

  /* Note that after this point the resources used by this thread can
     be freed at any moment if another thread joins or detaches us.
     This means that before freeing any resources, such a thread
     should make sure that this thread is really halted.  */
  
  __pthread_thread_halt (self);

  /* NOTREACHED */
  abort ();
}
