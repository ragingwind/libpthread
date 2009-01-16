/* Setup thread stack.  Viengoos/x86_64 version.
   Copyright (C) 2000, 2002, 2008, 2009 Free Software Foundation, Inc.
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

#include <pt-internal.h>

/* Set up the stack for THREAD, such that it appears as if
   START_ROUTINE and ARG were passed to the new thread's entry-point.
   Return the stack pointer for the new thread.  We also take the
   opportunity to install THREAD in our utcb.  */
static void *
stack_setup (struct __pthread *thread,
	     void *(*start_routine)(void *), void *arg,
	     void (*entry_point)(void *(*)(void *), void *))
{
# warning Not ported to this architecture.
  assert (0);
  return 0;
}

int
__pthread_setup (struct __pthread *thread,
		 void (*entry_point)(void *(*)(void *), void *),
		 void *(*start_routine)(void *), void *arg)
{
  thread->mcontext.pc = (void *) 0;
  thread->mcontext.sp = (void *) stack_setup (thread, start_routine, arg,
					      entry_point);
  return 0;
}
