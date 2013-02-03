/* Start thread.  Mach version.
   Copyright (C) 2000, 2002, 2005, 2008 Free Software Foundation, Inc.
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
#include <string.h>

#include <mach.h>

#include <pt-internal.h>

/* Prepare a wakeup message.  */
static error_t
create_wakeupmsg (struct __pthread *thread)
{
  kern_return_t err;

  /* Build wakeup message.  */
  thread->wakeupmsg.msgh_bits = MACH_MSGH_BITS (MACH_MSG_TYPE_COPY_SEND, 0);
  thread->wakeupmsg.msgh_size = 0;

  err = __mach_port_allocate (__mach_task_self (), MACH_PORT_RIGHT_RECEIVE,
			      &thread->wakeupmsg.msgh_remote_port);
  if (err)
    return EAGAIN;

  thread->wakeupmsg.msgh_local_port = MACH_PORT_NULL;
  thread->wakeupmsg.msgh_seqno = 0;
  thread->wakeupmsg.msgh_id = 0;

  err = __mach_port_insert_right (__mach_task_self (),
				  thread->wakeupmsg.msgh_remote_port,
				  thread->wakeupmsg.msgh_remote_port,
				  MACH_MSG_TYPE_MAKE_SEND);
  if (err)
    {
      __mach_port_destroy (__mach_task_self (),
			   thread->wakeupmsg.msgh_remote_port);
      return EAGAIN;
    }

  /* No need to queue more than one wakeup message on this port.  */
  mach_port_set_qlimit (__mach_task_self (),
			thread->wakeupmsg.msgh_remote_port, 1);

  return 0;
}

/* Allocate any resouces for THREAD.  The new kernel thread should not
   be eligible to be scheduled.  */
int
__pthread_thread_alloc (struct __pthread *thread)
{
  if (thread->have_kernel_resources)
    return 0;

  error_t err;

  err = create_wakeupmsg (thread);
  if (err)
    return err;

  /* If there are no pthreads in the system then the pthread library
     is bootstrapping and the main thread must create initialize
     itself.  The thread itself is already running, it just has not
     pthread context.  We want to reuse what it already has (including
     the kernel thread), however, we must determine which thread is
     the main thread.

     We cannot test if __pthread_total is one as we later decrement
     before creating the signal thread.  Currently, we check if
     __pthread_num_threads--the number of allocated thread
     structures--is one.  __pthread_alloc has already been called in
     __pthread_create_internal for us.  This predicate could be improved,
     however, it is sufficient for now.  */
  if (__pthread_num_threads == 1)
    {
      assert (__pthread_total == 0);
      thread->kernel_thread = __mach_thread_self ();
      /* We implicitly hold a reference drop the one that we just
	 acquired.  */
      __mach_port_deallocate (__mach_task_self (), thread->kernel_thread);
    }
  else
    {
      err = __thread_create (__mach_task_self (), &thread->kernel_thread);
      if (err)
	return EAGAIN;
    }

  thread->have_kernel_resources = 1;

  return 0;
}
