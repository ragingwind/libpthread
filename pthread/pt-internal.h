/* Internal defenitions for pthreads library.
   Copyright (C) 2000, 2004 Free Software Foundation, Inc.
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

#ifndef _PT_INTERNAL_H
#define _PT_INTERNAL_H	1

#include <pthread.h>
#if 0
#include <stddef.h>
#include <sched.h>
#include <signal.h>
#include <assert.h>
#endif
#include <atomic.h>

#include <pt-key.h>

#include <pt-sysdep.h>
#include <pt-machdep.h>

/* Thread state.  */
enum pthread_state
{
  PTHREAD_JOINABLE = 0,
  PTHREAD_DETACHED,
  PTHREAD_EXITED,
  PTHREAD_TERMINATED
};

#ifndef PTHREAD_KEY_MEMBERS
# define PTHREAD_KEY_MEMBERS
#endif

#ifndef PTHREAD_SYSDEP_MEMBERS
# define PTHREAD_SYSDEP_MEMBERS
#endif

/* This structure describes a POSIX thread.  */
struct __pthread
{
  /* Thread ID.  */
  pthread_t thread;

  /* Cancellation.  */
  int cancel_state;
  int cancel_type;
  int cancel_pending;
  struct __pthread_cancelation_handler *cancelation_handlers;

  /* Thread stack.  */
  void *stackaddr;
  size_t stacksize;
  size_t guardsize;		/* Included in STACKSIZE (i.e. total
				   stack memory is STACKSIZE, not
				   STACKSIZE + GUARDSIZE).  */
  int stack;			/* Nonzero if the stack was allocated.  */

  /* Exit status.  */
  void *status;

  /* Thread state.  */
  enum pthread_state state;
  pthread_mutex_t state_lock;	/* Locks the state.  */
  pthread_cond_t state_cond;	/* Signalled when the state changes.  */

  /* Thread context.  */
  struct pthread_mcontext mcontext;

  PTHREAD_KEY_MEMBERS

  PTHREAD_SYSDEP_MEMBERS

  struct __pthread *next, **prevp;
};

/* Enqueue an element THREAD on the queue *HEAD.  */
static inline void
__pthread_enqueue (struct __pthread **head, struct __pthread *thread)
{
  assert (thread->prevp == 0);

  thread->next = *head;
  thread->prevp = head;
  if (*head)
    (*head)->prevp = &thread->next;
  *head = thread;
}

/* Dequeue the element THREAD from the queue it is connected to.  */
static inline void
__pthread_dequeue (struct __pthread *thread)
{
  assert (thread);

  if (thread->next)
    thread->next->prevp = thread->prevp;
  *thread->prevp = thread->next;
  thread->prevp = 0;
}

/* Iterate over QUEUE storing each element in ELEMENT.  */
#define __pthread_queue_iterate(queue, element) \
	for (element = queue; element; element = element->next)

/* Iterate over QUEUE dequeuing each element, storing it in
   ELEMENT.  */
#define __pthread_dequeuing_iterate(queue, element) \
	for (element = queue; \
	     element && ((element->prevp = 0), 1); \
	     element = element->next)

/* The total number of threads currently active.  */
extern uatomic_max_t __pthread_total;

/* The total number of thread IDs currently in use, or on the list of
   available thread IDs.  */
extern int __pthread_num_threads;

/* Concurrency hint.  */
extern int __pthread_concurrency;

/* Array of __pthread structures and its lock.  */
extern struct __pthread **__pthread_threads;
extern pthread_rwlock_t __pthread_threads_lock;

#define __pthread_getid(thread) \
  ({ struct __pthread *__t;                                                  \
     pthread_rwlock_rdlock (&__pthread_threads_lock);                        \
     __t = __pthread_threads[thread];                                        \
     pthread_rwlock_unlock (&__pthread_threads_lock);                        \
     __t; })

#define __pthread_setid(thread, pthread) \
  pthread_rwlock_wrlock (&__pthread_threads_lock);                           \
  __pthread_threads[thread] = pthread;                                       \
  pthread_rwlock_unlock (&__pthread_threads_lock);

/* Similar to pthread_self, but returns the thread descriptor instead
   of the thread ID.  */
#ifndef _pthread_self
extern struct __pthread *_pthread_self (void);
#endif


/* Initialize the pthreads library.  */
extern void __pthread_initialize (void);

/* Internal version of pthread_create.  Rather than return the new
   tid, we return the whole __pthread structure in *PTHREAD.  */
extern int __pthread_create_internal (struct __pthread **pthread,
				      const pthread_attr_t *attr,
				      void *provided_thread,
				      void *(*start_routine)(void *),
				      void *arg);

/* Allocate a new thread structure and a pthread thread ID (but not a
   kernel thread or a stack).  */
extern int __pthread_alloc (struct __pthread **thread);

/* Deallocate the thread structure.  This is the dual of
   __pthread_alloc (N.B. it does not call __pthread_stack_alloc nor
   __pthread_thread_halt).  */
extern void __pthread_dealloc (struct __pthread *thread);


/* Allocate a stack of size STACKSIZE.  The stack base shall be
   returned in *STACKADDR.  */
extern int __pthread_stack_alloc (void **stackaddr, size_t stacksize);

/* Deallocate the stack STACKADDR of size STACKSIZE.  */
extern void __pthread_stack_dealloc (void *stackaddr, size_t stacksize);


/* Setup thread THREAD's context.  */
extern int __pthread_setup (struct __pthread *thread,
				  void (*entry_point)(void *(*)(void *),
						      void *),
				  void *(*start_routine)(void *), void *arg);


/* Allocate a kernel thread for THREAD; it must not be placed on the
   run queue.  */
extern int __pthread_thread_alloc (struct __pthread *thread);

/* Start THREAD making it eligible to run.  */
extern int __pthread_thread_start (struct __pthread *thread);

/* Stop thread thread and deallocate any kernel resources associated
   with THREAD.  */
extern void __pthread_thread_halt (struct __pthread *thread);

/* Initialize provided kernel thread.  */
extern int __pthread_init_provided_thread (struct __pthread *thread,
					   void *p);

/* Block THREAD.  */
extern void __pthread_block (struct __pthread *thread);

/* Block THREAD until *ABSTIME is reached.  */
extern error_t __pthread_timedblock (struct __pthread *thread,
				     const struct timespec *abstime);

/* Wakeup THREAD.  */
extern void __pthread_wakeup (struct __pthread *thread);


/* Perform a cancelation.  */
extern int __pthread_do_cancel (struct __pthread *thread);


/* Initialize the thread specific data structures.  THREAD must be the
   calling thread.  */
extern error_t __pthread_init_specific (struct __pthread *thread);

/* Call the destructors on all of the thread specific data in THREAD.
   THREAD must be the calling thread.  */
extern void __pthread_destroy_specific (struct __pthread *thread);


/* Initialize newly create thread *THREAD's signal state data
   structures.  */
extern error_t __pthread_sigstate_init (struct __pthread *thread);

/* Destroy the signal state data structures associcated with thread
   *THREAD.  */
extern void __pthread_sigstate_destroy (struct __pthread *thread);

/* Modify thread *THREAD's signal state.  */
extern error_t __pthread_sigstate (struct __pthread *thread, int how,
				   const sigset_t *set, sigset_t *oset,
				   int clear_pending);


/* Default thread attributes.  */
extern const struct __pthread_attr __pthread_default_attr;

/* Default barrier attributes.  */
extern const struct __pthread_barrierattr __pthread_default_barrierattr;

/* Default mutex attributes.  */
extern const struct __pthread_mutexattr __pthread_default_mutexattr;

/* Default rdlock attributes.  */
const struct __pthread_rwlockattr __pthread_default_rwlockattr;

/* Default condition attributes.  */
const struct __pthread_condattr __pthread_default_condattr;

#endif /* pt-internal.h */
