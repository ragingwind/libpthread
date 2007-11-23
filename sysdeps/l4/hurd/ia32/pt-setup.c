/* Setup thread stack.  Hurd/i386 version.
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

#include <l4.h>

#include <pt-internal.h>

/* The stack layout used on the i386 is:

    -----------------
   |  ARG            |
    -----------------
   |  START_ROUTINE  |
    -----------------
   |  0              |
   -----------------  */

/* Set up the stack for THREAD, such that it appears as if
   START_ROUTINE and ARG were passed to the new thread's entry-point.
   Return the stack pointer for the new thread.  We also take the
   opportunity to install THREAD in our utcb.  */
static void *
stack_setup (struct __pthread *thread,
	     void *(*start_routine)(void *), void *arg)
{
  l4_word_t *top;

  /* Calculate top of the new stack.  */
  top = (l4_word_t *) ((l4_word_t) thread->stackaddr + thread->stacksize);

  if (start_routine)
    {
      /* Set up call frame.  */
      *--top = (l4_word_t) arg;	/* Argument to START_ROUTINE.  */
      *--top = (l4_word_t) start_routine;
      *--top = 0;		/* Fake return address.  */
    }

  return top;
}

int
__pthread_setup (struct __pthread *thread,
		 void (*entry_point)(void *(*)(void *), void *),
		 void *(*start_routine)(void *), void *arg)
{
  thread->mcontext.pc = entry_point;
  thread->mcontext.sp = stack_setup (thread, start_routine, arg);


  if (__pthread_num_threads != 1)
    {
      assert (! ADDR_IS_VOID (thread->exception_handler_stack.addr));
      thread->exception_handler_sp
	= ADDR_TO_PTR (addr_extend (thread->exception_handler_stack.addr,
				    0, PAGESIZE_LOG2));
    }

  return 0;
}
