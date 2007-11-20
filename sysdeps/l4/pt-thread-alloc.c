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
    thread->threadid = l4_myself ();
  else
    {
      thread->threadid = pthread_pool_get_np ();
      if (thread->threadid != l4_nilthread)
	return 0;

      return EAGAIN;
    }
  return 0;
}
