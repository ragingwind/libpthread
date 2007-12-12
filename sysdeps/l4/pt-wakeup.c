/* Wakeup a thread.  L4 version.
   Copyright (C) 2002 Free Software Foundation, Inc.
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

#include <hurd/stddef.h>

/* Wakeup THREAD.  */
void
__pthread_wakeup (struct __pthread *thread)
{
  debug (5, "%x.%x/%x waking %x.%x/%x",
	 l4_thread_no (l4_myself ()), l4_version (l4_myself ()), l4_myself (),
	 l4_thread_no (thread->threadid), l4_version (thread->threadid),
	 thread->threadid);

  /* Signal the waiter.  */
  l4_msg_t msg;
  l4_msg_clear (msg);
  l4_msg_set_untyped_words (msg, 0);
  l4_msg_load (msg);

  l4_msg_tag_t tag = l4_send (thread->threadid);
  if (l4_ipc_failed (tag))
    {
      int err = l4_error_code ();
      debug (1, "%x.%x failed to wake %x.%x: %s (%d)",
	     l4_thread_no (l4_myself ()), l4_version (l4_myself ()),
	     l4_thread_no (thread->threadid), l4_version (thread->threadid),
	     l4_strerror (err), err);
    }
  else
    debug (5, "%x.%x woke %x.%x",
	   l4_thread_no (l4_myself ()), l4_version (l4_myself ()),
	   l4_thread_no (thread->threadid), l4_version (thread->threadid));
}
