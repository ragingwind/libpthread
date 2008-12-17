/* Deallocate a thread.  Viengoos version.
   Copyright (C) 2008 Free Software Foundation, Inc.
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

#include <pt-internal.h>

#include <hurd/exceptions.h>
#include <hurd/mutex.h>
#include <hurd/as.h>
#include <viengoos/addr.h>
#include <hurd/message-buffer.h>

void
__pthread_thread_dealloc (struct __pthread *thread)
{
  assert (thread != _pthread_self ());

  __pthread_thread_halt (thread);

  /* Clean up the activation state.  */
  hurd_activation_state_free (thread->utcb);

  assert (thread->lock_message_buffer);
  hurd_message_buffer_free (thread->lock_message_buffer);

  thread->have_kernel_resources = 0;
}
