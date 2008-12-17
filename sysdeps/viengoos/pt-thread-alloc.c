/* Allocate kernel thread.  Viengoos version.
   Copyright (C) 2007, 2008 Free Software Foundation, Inc.
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

#include <hurd/startup.h>
#include <hurd/storage.h>
#include <hurd/as.h>
#include <viengoos/addr.h>
#include <hurd/message-buffer.h>

#include <pt-internal.h>

extern struct hurd_startup_data *__hurd_startup_data;

extern addr_t meta_data_activity;

int
__pthread_thread_alloc (struct __pthread *thread)
{
  if (thread->have_kernel_resources)
    return 0;

  thread->lock_message_buffer = hurd_message_buffer_alloc_long ();

  /* The main thread is already running of course.  */
  if (__pthread_num_threads == 1)
    {
      thread->object = __hurd_startup_data->thread;
      thread->threadid = l4_myself ();

      l4_set_user_defined_handle ((l4_word_t) thread);

      /* Get the thread's UTCB and stash it.  */
      thread->utcb = hurd_utcb ();
      /* Override the utcb fetch function.  */
      hurd_utcb = pthread_hurd_utcb_np;
      assert (thread->utcb == hurd_utcb ());
    }
  else
    {
      struct storage storage;
      storage = storage_alloc (meta_data_activity, cap_thread,
			       /* Threads are rarely shortly lived.  */
			       STORAGE_MEDIUM_LIVED, OBJECT_POLICY_DEFAULT,
			       ADDR_VOID);
      if (ADDR_IS_VOID (storage.addr))
	{
	  debug (0, DEBUG_BOLD ("Out of memory"));
	  return EAGAIN;
	}

      thread->object = storage.addr;

      error_t err;
      err = hurd_activation_state_alloc (thread->object, &thread->utcb);
      if (unlikely (err))
	panic ("Failed to initialize thread's activation state: %d", err);

      err = rm_cap_copy (ADDR_VOID,
			 thread->lock_message_buffer->receiver,
			 ADDR (VG_MESSENGER_THREAD_SLOT,
			       VG_MESSENGER_SLOTS_LOG2),
			 ADDR_VOID, thread->object,
			 0, CAP_PROPERTIES_DEFAULT);
      if (err)
	panic ("Failed to set lock messenger's thread");

      /* Unblock the lock messenger.  */
      err = vg_ipc (VG_IPC_RECEIVE | VG_IPC_RECEIVE_ACTIVATE
		    | VG_IPC_RETURN,
		    ADDR_VOID, thread->lock_message_buffer->receiver, ADDR_VOID,
		    ADDR_VOID, ADDR_VOID, ADDR_VOID, ADDR_VOID);
      if (err)
	panic ("Failed to unblock messenger's thread");
    }

  thread->have_kernel_resources = true;

  return 0;
}
