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
#include <hurd/addr.h>

#include <pt-internal.h>

extern struct hurd_startup_data *__hurd_startup_data;

extern addr_t meta_data_activity;

int
__pthread_thread_alloc (struct __pthread *thread)
{
  /* The main thread is already running of course.  */
  if (__pthread_num_threads == 1)
    {
      thread->object = __hurd_startup_data->thread;
      thread->threadid = l4_myself ();
      return 0;
    }
  else
    {
      addr_t exception_area = as_alloc (EXCEPTION_AREA_SIZE_LOG2, 1, true);

      thread->exception_area_va
	= ADDR_TO_PTR (addr_extend (exception_area,
				    0, EXCEPTION_AREA_SIZE_LOG2));

      int i;
      for (i = 0; i < EXCEPTION_AREA_SIZE / PAGESIZE; i ++)
	{
	  addr_t slot = addr_chop (PTR_TO_ADDR (thread->exception_area_va
						+ i * PAGESIZE),
				   PAGESIZE_LOG2);
	  as_ensure (slot);

	  struct storage storage = storage_alloc (ADDR_VOID, cap_page,
						  STORAGE_LONG_LIVED,
						  OBJECT_POLICY_DEFAULT,
						  slot);
	  if (ADDR_IS_VOID (storage.addr))
	    {
	      int j;
	      for (j = 0; j < i; j ++)
		storage_free (thread->exception_area[j], false);
	      as_free (exception_area, false);
	      return EAGAIN;
	    }

	  thread->exception_area[i] = storage.addr;
	}

      struct storage storage;
      storage = storage_alloc (meta_data_activity, cap_thread,
			       /* Threads are rarely shortly lived.  */
			       STORAGE_MEDIUM_LIVED, OBJECT_POLICY_DEFAULT,
			       ADDR_VOID);
      if (ADDR_IS_VOID (storage.addr))
	{
	  int j;
	  for (j = 0; j < EXCEPTION_AREA_SIZE / PAGESIZE; j ++)
	    storage_free (thread->exception_area[j], false);
	  as_free (exception_area, false);
	  return EAGAIN;
	}

      thread->object = storage.addr;
    }

  return 0;
}
