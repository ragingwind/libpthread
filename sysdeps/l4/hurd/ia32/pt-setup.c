/* Setup thread stack.  Hurd/i386 version.
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

#include <l4.h>

#include <pt-internal.h>
#include <hurd/thread.h>
#include <hurd/exceptions.h>

/* The stack layout used on the i386 is:

    -----------------
   |  ARG            |
    -----------------
   |  START_ROUTINE  |
    -----------------
   |  Return address |
    -----------------  <- %ebp
   |  Frame pointer  |
    -----------------

  We do the following: setup the stack to return to the entry routine.

  
*/

/* The stack contains:

    arg
    start_routine
    0 <- fake return address
    C entry_point
*/
extern uintptr_t _pthread_entry_point;
__asm__ ("\n\
	.globl	_pthread_entry_point, __pthread_entry_point\n\
_pthread_entry_point:\n\
__pthread_entry_point:\n\
	pushl	$0\n\
	popf\n\
\n\
	xor %ebp, %ebp\n\
	ret\n");

/* Set up the stack for THREAD, such that it appears as if
   START_ROUTINE and ARG were passed to the new thread's entry-point.
   Return the stack pointer for the new thread.  We also take the
   opportunity to install THREAD in our utcb.  */
static void *
stack_setup (struct __pthread *thread,
	     void *(*start_routine)(void *), void *arg,
	     void (*entry_point)(void *(*)(void *), void *))
{
  uintptr_t *top;

  /* Calculate top of the new stack.  */
  top = (uintptr_t *) ((uintptr_t) thread->stackaddr + thread->stacksize);

  if (start_routine)
    {
      /* Set up call frame.  */
      *--top = (uintptr_t) arg;	/* Argument to START_ROUTINE.  */
      *--top = (uintptr_t) start_routine;
      *--top = 0;		/* Fake return address.  */
      *--top = (uintptr_t) entry_point;
    }

  return top;
}

int
__pthread_setup (struct __pthread *thread,
		 void (*entry_point)(void *(*)(void *), void *),
		 void *(*start_routine)(void *), void *arg)
{
  thread->mcontext.pc = (void *) &_pthread_entry_point;
  thread->mcontext.sp = (void *) stack_setup (thread, start_routine, arg,
					      entry_point);

  if (__pthread_num_threads == 1)
    return 0;

  assert (! ADDR_IS_VOID (thread->exception_page.addr));

  struct exception_page *exception_page 
    = ADDR_TO_PTR (addr_extend (thread->exception_page.addr,
				0, PAGESIZE_LOG2));

  /* SP is set to the end of the exception page.  */
  exception_page->exception_handler_sp = (uintptr_t) exception_page + PAGESIZE;

  exception_page->exception_handler_ip = (uintptr_t) &exception_handler_entry;
  exception_page->exception_handler_end = (uintptr_t) &exception_handler_end;

  return 0;
}
