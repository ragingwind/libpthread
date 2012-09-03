/* pthread_hurd_cond_wait.  Hurd-specific wait on a condition.
   Copyright (C) 2012 Free Software Foundation, Inc.
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
#include <hurd/signal.h>

#include <pt-internal.h>

int
pthread_hurd_cond_wait_np (pthread_cond_t *cond, pthread_mutex_t *mutex)
{
  /* This function will be called by hurd_thread_cancel while we are blocked
     We wake up all threads blocked on COND, so our thread will wake up and
     notice the cancellation flag.  */
  void cancel_me (void)
    {
      pthread_cond_broadcast (cond);
    }
  struct hurd_sigstate *ss = _hurd_self_sigstate ();
  struct __pthread *self = _pthread_self ();
  int cancel;

  assert (ss->intr_port == MACH_PORT_NULL); /* Sanity check for signal bugs. */

  /* Atomically enqueue our thread on the condition variable's queue of
     waiters, and mark our sigstate to indicate that `cancel_me' must be
     called to wake us up.  We must hold the sigstate lock while acquiring
     the condition variable's lock and tweaking it, so that
     hurd_thread_cancel can never suspend us and then deadlock in
     pthread_cond_broadcast waiting for the condition variable's lock.  */

  __spin_lock (&ss->lock);
  __pthread_spin_lock (&cond->__lock);
  cancel = ss->cancel;
  if (cancel)
    /* We were cancelled before doing anything.  Don't block at all.  */
    ss->cancel = 0;
  else
    {
      /* Put us on the queue so that pthread_cond_broadcast will know to wake
         us up.  */
      __pthread_enqueue (&cond->__queue, self);
      /* Tell hurd_thread_cancel how to unblock us.  */
      ss->cancel_hook = &cancel_me;
    }
  __pthread_spin_unlock (&cond->__lock);
  __spin_unlock (&ss->lock);


  if (cancel)
    /* Cancelled on entry.  Just leave the mutex locked.  */
    mutex = NULL;
  else
    {
      /* Now unlock the mutex and block until woken.  */
      __pthread_mutex_unlock (mutex);
      __pthread_block (self);
    }

  __spin_lock (&ss->lock);
  /* Clear the hook, now that we are done blocking.  */
  ss->cancel_hook = NULL;
  /* Check the cancellation flag; we might have unblocked due to
     cancellation rather than a normal pthread_cond_signal or
     pthread_cond_broadcast (or we might have just happened to get cancelled
     right after waking up).  */
  cancel |= ss->cancel;
  ss->cancel = 0;
  __spin_unlock (&ss->lock);

  if (mutex)
    /* Reacquire the mutex and return.  */
    __pthread_mutex_lock (mutex);

  return cancel;
}
