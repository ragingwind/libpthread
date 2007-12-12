/* Deallocate the kernel thread resources.  L4/Hurd version.
   Copyright (C) 2007 Software Foundation, Inc.
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

/* If we try to deallocate our self, we will end up causing a
   deadlock.  Thus, when a thread tries to free itself, we add it
   here.  The next thread to free a thread will free it.  */
static struct storage saved_object;

void
__pthread_thread_halt (struct __pthread *thread, int need_dealloc)
{
  struct storage exception_page = thread->exception_page;
  thread->exception_page.addr = ADDR_VOID;

  struct storage object = thread->object;
  l4_thread_id_t tid = thread->threadid;

  if (need_dealloc)
    __pthread_dealloc (thread);

  if (! ADDR_IS_VOID (saved_object.addr))
    {
      storage_free (saved_object.addr, false);
      saved_object.cap->type = cap_void;
      saved_object.addr = ADDR_VOID;
    }

  /* Free the exception page.  */
  assert (! ADDR_IS_VOID (exception_page.addr));
  exception_page_cleanup (ADDR_TO_PTR (addr_extend (exception_page.addr,
						    0, PAGESIZE_LOG2)));
  storage_free (exception_page.addr, false);

  if (tid == l4_myself ())
    /* If we try to storage_free (storage.addr), we will freeze in the
       middle.  That's no good.  Thus, we add ourself to the pool of
       available objects.  */
    saved_object = object;
  else
    {
      storage_free (object.addr, false);
      object.cap->type = cap_void;
    }

  if (tid == l4_myself ())
    {
      l4_send_timeout (l4_myself (), L4_NEVER);
      panic ("Failed to stop thread %x.%x!",
	     l4_thread_no (l4_myself ()), l4_version (l4_myself ()));
    }
  else
    thread_stop (object.addr);
}
