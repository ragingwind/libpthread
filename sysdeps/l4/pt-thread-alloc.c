/* Allocate kernel thread.  L4 version.
   Copyright (C) 2003, 2005 Free Software Foundation, Inc.
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

#include <pt-internal.h>

int
__pthread_thread_alloc (struct __pthread *thread)
{
  error_t err;

  /* The main thread is already running of course.  */
  if (__pthread_num_threads == 1)
    {
      /* XXX: The initialization code needs to make this consistent.
	 Right now, we have none and this whole library is a hack
	 anyways.  */
#warning __pthread_total is inconsistent.
#if 0
      assert (__pthread_total == 1);
#endif
      thread->threadid = l4_myself ();
    }
  else
    {
      thread->threadid = pthread_pool_get_np ();
      if (thread->threadid != l4_nilthread)
	return 0;

#if 0
      CORBA_Environment env;

      env = idl4_default_environment;
      err = thread_create (__task_server,
			   L4_Version (L4_Myself ()),
			   * (L4_Word_t *) &__system_pager,
			   (L4_Word_t *) &thread->threadid, &env);
      if (err)
#endif
	return EAGAIN;
    }
  return 0;
}
