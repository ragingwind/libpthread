/* Deallocate the kernel thread resources.  Viengoos version.
   Copyright (C) 2007, 2008 Software Foundation, Inc.
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
#include <hurd/addr.h>

/* If we try to deallocate our self, we will end up causing a
   deadlock.  Thus, when a thread tries to free itself, we add it
   here.  The next thread to free a thread will free it.  */
ss_mutex_t saved_object_lock;
static addr_t saved_object;

void
__pthread_thread_halt (struct __pthread *thread, int need_dealloc)
{
  /* We may deallocate THREAD.  First save any data we need.  */

  addr_t exception_area[EXCEPTION_AREA_SIZE / PAGESIZE];
  memcpy (exception_area, thread->exception_area,
	  sizeof (thread->exception_area));
  memset (thread->exception_area, 0, sizeof (thread->exception_area));

  void *va = thread->exception_area_va;

  addr_t object = thread->object;
  l4_thread_id_t tid = thread->threadid;

  if (need_dealloc)
    __pthread_dealloc (thread);

  /* The THREAD data structure is no longer valid.  */
  thread = NULL;

  /* Deallocate any saved object.  */
  ss_mutex_lock (&saved_object_lock);
  if (! ADDR_IS_VOID (saved_object))
    {
      storage_free (saved_object, false);
      saved_object = ADDR_VOID;
    }
  ss_mutex_unlock (&saved_object_lock);

  /* Free the exception area.  */

  /* Clean up the exception page.  */
  exception_page_cleanup
    (ADDR_TO_PTR (addr_extend (exception_area[EXCEPTION_PAGE],
			       0, PAGESIZE_LOG2)));

  /* Free the storage.  */
  int i;
  for (i = 0; i < EXCEPTION_AREA_SIZE / PAGESIZE; i ++)
    {
      assert (! ADDR_IS_VOID (exception_area[i]));
      storage_free (exception_area[i], false);
    }

  /* And the address space.  */
  as_free (addr_chop (PTR_TO_ADDR (va), EXCEPTION_AREA_SIZE_LOG2), false);

  if (tid == l4_myself ())
    /* If we try to storage_free (storage.addr), we will freeze in the
       middle.  That's no good.  We set SAVED_OBJECT to our thread
       object and the next thread in will free us.  */
    {
      ss_mutex_lock (&saved_object_lock);
      saved_object = object;
      ss_mutex_unlock (&saved_object_lock);
    }
  else
    storage_free (object, false);

  if (tid == l4_myself ())
    {
      l4_send_timeout (l4_myself (), L4_NEVER);
      panic ("Failed to stop thread %x.%x!",
	     l4_thread_no (l4_myself ()), l4_version (l4_myself ()));
    }
  else
    thread_stop (object);
}
