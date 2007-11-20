/* Deallocate the kernel thread resources.  Mach version.
   Copyright (C) 2005 Free Software Foundation, Inc.
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
#include <mach.h>

#include <pt-internal.h>

/* Deallocate any kernel resources associated with THREAD except don't
   halt the thread itself.  On return, the thread will be marked as
   dead and __pthread_halt will be called.  */
void
__pthread_thread_dealloc (struct __pthread *thread)
{
  /* Why no assert?  Easy.  When Mach kills a task, it starts by
     invalidating the task port and then terminating the threads one
     by one.  But while it is terminating them, they are still
     eligible to be scheduled.  Imagine we have two threads, one calls
     exit, one calls pthread_exit.  The second one may run this after
     the mask port can been destroyed thus gratuitously triggering the
     assert.  */
  __mach_port_destroy (__mach_task_self (),
		       thread->wakeupmsg.msgh_remote_port);
}
