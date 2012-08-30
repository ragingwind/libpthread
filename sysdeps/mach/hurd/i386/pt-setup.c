/* Setup thread stack.  Hurd/i386 version.
   Copyright (C) 2000, 2002, 2005, 2007, 2008 Free Software Foundation, Inc.
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

#include <stdint.h>
#include <assert.h>
#include <mach.h>

#include <pt-internal.h>

/* The stack layout used on the i386 is:

    -----------------
   |  ARG            |
    -----------------
   |  START_ROUTINE  |
    -----------------
   |  0              |
    -----------------
   |                 |
   |  Fast TSD       |
   |                 |
    -----------------

    We need to reserve __hurd_threadvar_max `unsigned long int's' of
    (fast) thread-specific data (TSD) for the Hurd.  */

/* Set up the stack for THREAD, such that it appears as if
   START_ROUTINE and ARG were passed to the new thread's entry-point.
   Return the stack pointer for the new thread.  */
static void *
stack_setup (struct __pthread *thread,
	     void *(*start_routine)(void *), void *arg)
{
  error_t err;
  uintptr_t *bottom, *top;

  /* Calculate the top of the new stack.  */
  bottom = thread->stackaddr;
  top = (uintptr_t *) ((uintptr_t) bottom + thread->stacksize);

  /* Next, make room for the TSDs.  */
  top -= __hurd_threadvar_max;

  /* Save the self pointer.  */
  top[_HURD_THREADVAR_THREAD] = (uintptr_t) thread;

  if (start_routine)
    {
      /* And then the call frame.  */
      top -= 2;
      top = (uintptr_t *) ((uintptr_t) top & ~0xf);
      top[1] = (uintptr_t) arg;	/* Argument to START_ROUTINE.  */
      top[0] = (uintptr_t) start_routine;
      *--top = 0;		/* Fake return address.  */
    }

  if (thread->guardsize)
    {
      err = __vm_protect (__mach_task_self (), (vm_address_t) bottom,
			  thread->guardsize, 0, 0);
      assert_perror (err);
    }

  return top;
}

int
__pthread_setup (struct __pthread *thread,
		 void (*entry_point)(void *(*)(void *), void *),
		 void *(*start_routine)(void *), void *arg)
{
  error_t err;
  mach_port_t ktid;

  thread->mcontext.pc = entry_point;
  thread->mcontext.sp = stack_setup (thread, start_routine, arg);

  thread->tcb->self = thread->kernel_thread;

  ktid = __mach_thread_self ();
  if (thread->kernel_thread != ktid)
    {
      err = __thread_set_pcsptp (thread->kernel_thread,
			       1, thread->mcontext.pc,
			       1, thread->mcontext.sp,
			       1, thread->tcb);
      assert_perror (err);
    }
  __mach_port_deallocate (__mach_task_self (), ktid);

  return 0;
}
