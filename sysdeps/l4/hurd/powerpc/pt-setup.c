/* Setup thread stack.  Hurd/PowerPC version.
   Copyright (C) 2003 Free Software Foundation, Inc.
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

/* Arguments is passed in registers on the PowerPC.  But the
   exchange registers syscall only allows us to set the PC and the
   stack pointer so we put the entry point and start function on
   the stack.  */
struct start_info
{
  void (*entry_point) (void *(*)(void *), void *);
  void *(*start_routine) (void *);
  void *arg;
};

void first_entry_1 (void);

/* Stage 1 entry function.  The start_info structure is inlined on the
   stack.  Put values into registers and call entry function.  */
asm ("				;\
first_entry_1:			;\
	lwz	0, 0(1)		;\
	lwz	3, 4(1)		;\
	lwz	4, 8(1)		;\
	mtctr	0		;\
	bctrl			;\
");

/* Set up the stack for THREAD, such that it appears as if
   START_ROUTINE and ARG were passed to the new thread's entry-point.
   Return the stack pointer for the new thread.  We also take the
   opportunity to install THREAD in our utcb.  */
static void *
stack_setup (struct __pthread *thread,
	     void (*entry_point)(void *(*)(void *), void *),
	     void *(*start_routine)(void *), void *arg)
{
  l4_word_t *top;

  /* Calculate top of the new stack.  */
  top = (l4_word_t *) ((l4_word_t) thread->stackaddr + thread->stacksize);

  /* Initial stack frame.  */
  top[-4] = 0;
  top = top - 4;

  if (start_routine)
    {
      struct start_info *info = ((struct start_info *) top) - 1;

      info->entry_point = entry_point;
      info->start_routine = start_routine;
      info->arg = arg;
      return (void *) info;
    }
  return top;
}

int
__pthread_setup (struct __pthread *thread,
		 void (*entry_point)(void *(*)(void *), void *),
		 void *(*start_routine)(void *), void *arg)
{
  thread->mcontext.pc = first_entry_1;
  thread->mcontext.sp = stack_setup (thread, entry_point,
				     start_routine, arg);

  if (l4_same_threads (thread->threadid, l4_myself ()))
    l4_set_user_defined_handle ((l4_word_t) thread);
  else
    l4_set_user_defined_handle_of (thread->threadid, 
				   (l4_word_t) thread);
  return 0;
}
