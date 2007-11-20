/* Deallocate the kernel thread resources.  L4version.
   Copyright (C) 2000, 2002, 2004 Free Software Foundation, Inc.
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

/* Deallocate the kernel thread resources associated with THREAD.  */
void
__pthread_thread_halt (struct __pthread *thread, int need_dealloc)
{
  l4_thread_id_t tid = thread->threadid;

  if (need_dealloc)
    __pthread_dealloc (thread);

  /* There is potential race here: once if TID is the current thread,
     then once we add TID to the pool, someone can reallocate it
     before we call stop.  However, to start the thread, the caller
     atomically starts and sets the sp and ip, thus, if the stop has
     not yet executed at that point, it won't.  */

  if (tid != l4_myself ())
    l4_stop (tid);
  pthread_pool_add_np (tid);
  if (tid == l4_myself ())
    l4_stop (tid);
}
