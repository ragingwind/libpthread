/* Thread creation.
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
#include <signal.h>

#include <bits/atomic.h>

#include <pt-internal.h>

#ifdef HAVE_USELOCALE
# include <locale.h>
#endif

/* The total number of pthreads currently active.  This is defined
   here since it would be really stupid to have a threads-using
   program that doesn't call `pthread_create'.  */
__atomic_t __pthread_total;


/* The entry-point for new threads.  */
static void
entry_point (void *(*start_routine)(void *), void *arg)
{
#ifdef HAVE_USELOCALE
  /* A fresh thread needs to be bound to the global locale.  */
  uselocale (LC_GLOBAL_LOCALE);
#endif

  pthread_exit (start_routine (arg));
}

/* Create a thread with attributes given by ATTR, executing
   START_ROUTINE with argument ARG.  */
int
pthread_create (pthread_t *thread, const pthread_attr_t *attr,
		void *(*start_routine)(void *), void *arg)
{
  int err;
  struct __pthread *pthread;

  err = __pthread_create_internal (&pthread, attr, 0, start_routine, arg);
  if (! err)
    *thread = pthread->thread;

  return err;
}

/* Internal version of pthread_create.  See comment in
   pt-internal.h.  */
int
__pthread_create_internal (struct __pthread **thread,
			   const pthread_attr_t *attr,
			   void *provided_thread,
			   void *(*start_routine)(void *), void *arg)
{
  int err;
  struct __pthread *pthread;
  const struct __pthread_attr *setup;
  sigset_t sigset;

  /* Allocate a new thread structure.  */
  err = __pthread_alloc (&pthread);
  if (err)
    goto failed;

  /* Use the default attributes if ATTR is NULL.  */
  setup = attr ? attr : &__pthread_default_attr;

  /* Initialize the thread state.  */
  pthread->state = (setup->detachstate == PTHREAD_CREATE_DETACHED
		    ? PTHREAD_DETACHED : PTHREAD_JOINABLE);

  /* If the user supplied a stack, it is not our responsibility to
     setup a stack guard.  */
  if (setup->stackaddr)
    pthread->guardsize = 0;
  else
    pthread->guardsize = (setup->guardsize <= setup->stacksize
			  ? setup->guardsize : setup->stacksize);

  /* Find a stack.  There are several scenarios: if a detached thread
     kills itself, it has no way to deallocate its stack, thus it
     leaves PTHREAD->stack set to true.  We try to reuse it here,
     however, if the user supplied a stack, we cannot use the old one.
     Right now, we simply deallocate it.  */
  if (pthread->stack)
    {
      if (setup->stackaddr != __pthread_default_attr.stackaddr)
	{
	  __pthread_stack_dealloc (pthread->stackaddr,
				   pthread->stacksize);
	  pthread->stackaddr = setup->stackaddr;
	  pthread->stacksize = setup->stacksize;
	}
    }
  else
    {
      err = __pthread_stack_alloc (&pthread->stackaddr,
				   setup->stacksize);
      if (err)
	goto failed_stack_alloc;

      pthread->stacksize = setup->stacksize;
      pthread->stack = 1;
    }

  /* Allocate the kernel thread and other required resources
     if they were not provided with this call.  */
  if (!provided_thread)
    {
      err = __pthread_thread_alloc (pthread);
      if (err)
	goto failed_thread_alloc;
    }
  else
    {
      err = __pthread_init_provided_thread (pthread, provided_thread);
      if (err)
	goto failed_thread_alloc;
    }

  /* And initialize the rest of the machine context.  This may include
     additional machine- and system-specific initializations that
     prove convenient.  */
  err = __pthread_setup (pthread, entry_point, start_routine, arg);
  if (err)
    goto failed_setup;

  /* Initialize the system-specific signal state for the new
     thread.  */
  err = __pthread_sigstate_init (pthread);
  if (err)
    goto failed_sigstate;

  /* Set the new thread's signal mask and set the pending signals to
     empty.  POSIX says: "The signal mask shall be inherited from the
     creating thread.  The set of signals pending for the new thread
     shall be empty."  If the currnet thread is not a pthread then we
     just inherit the process' sigmask.  */
  if (__pthread_num_threads == 1)
    /* FIXME no sigprocmask yet */
    err = 0; /* sigprocmask (0, 0, &sigset); */
  else
    err = __pthread_sigstate (_pthread_self (), 0, 0, &sigset, 0);
  assert_perror (err);
   
  err = __pthread_sigstate (pthread, SIG_SETMASK, &sigset, 0, 1);
  assert_perror (err);

  /* Increase the total number of threads.  We do this before actually
     starting the new thread, since the new thread might immediately
     call `pthread_exit' which decreases the number of threads and
     calls `exit' if the number of threads reaches zero.  Increasing
     the number of threads from within the new thread isn't an option
     since this thread might return and call `pthread_exit' before the
     new thread runs.  */
  __atomic_inc (&__pthread_total);

  /* Store a pointer to this thread in the thread ID lookup table.  We
     could use __thread_setid, however, we only lock for reading as no
     other thread should be using this entry (we also assume that the
     store is atomic).  */
  pthread_rwlock_rdlock (&__pthread_threads_lock);
  __pthread_threads[pthread->thread] = pthread;
  pthread_rwlock_unlock (&__pthread_threads_lock);

  /* At this point it is possible to guess our pthread ID.  We have to
     make sure that all functions taking a pthread_t argument can
     handle the fact that this thread isn't really running yet.  */

  /* Schedule the new thread.  */
  err = __pthread_thread_start (pthread);
  if (err)
    goto failed_starting;

  /* At this point the new thread is up and running.  */

  *thread = pthread;

  return 0;

 failed_starting:
  __pthread_setid (pthread->thread, NULL);
  __atomic_dec (&__pthread_total);
 failed_sigstate:
  __pthread_sigstate_destroy (pthread);
 failed_setup:
  __pthread_thread_halt (pthread);
 failed_thread_alloc:
  __pthread_stack_dealloc (pthread->stackaddr, pthread->stacksize);
  pthread->stack = 0;
 failed_stack_alloc:
  __pthread_dealloc (pthread);
 failed:
  return err;
}
