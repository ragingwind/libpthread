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
#include <hurd/addr.h>

void
__pthread_thread_dealloc (struct __pthread *thread)
{
  assert (thread != _pthread_self ());

  /* Clean up the exception page.  */
  exception_page_cleanup
    (ADDR_TO_PTR (addr_extend (thread->exception_area[EXCEPTION_PAGE],
			       0, PAGESIZE_LOG2)));

  /* Free the storage.  */
  int i;
  for (i = 0; i < EXCEPTION_AREA_SIZE / PAGESIZE; i ++)
    {
      assert (! ADDR_IS_VOID (thread->exception_area[i]));
      storage_free (thread->exception_area[i], false);
    }

  /* And the address space.  */
  as_free (addr_chop (PTR_TO_ADDR (thread->exception_area_va),
		      EXCEPTION_AREA_SIZE_LOG2), false);

  storage_free (thread->object, false);

  thread->have_kernel_resources = 0;
}
