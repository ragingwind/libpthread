/* Thread pool for L4 threads.
   Copyright (C) 2004, 2007 Free Software Foundation, Inc.
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

#include <pt-internal.h>
#include <l4/thread.h>

static pthread_mutex_t pool_lock = PTHREAD_MUTEX_INITIALIZER;

_L4_thread_id_t pool_list = l4_nilthread;

/* Add the thread TID to the pthread kernel thread pool.  */
int
pthread_pool_add_np (l4_thread_id_t tid)
{
  __pthread_mutex_lock (&pool_lock);
  /* FIXME: Do error checking.  */
  l4_set_user_defined_handle_of (tid, pool_list);
  pool_list = tid;
  __pthread_mutex_unlock (&pool_lock);

  return 0;
}


/* Get the first thread from the pool.  */
l4_thread_id_t
pthread_pool_get_np (void)
{
  _L4_thread_id_t tid;

  __pthread_mutex_lock (&pool_lock);
  /* FIXME: Do error checking.  */
  tid = pool_list;
  if (tid != l4_nilthread)
    pool_list = l4_user_defined_handle_of (tid);
  __pthread_mutex_unlock (&pool_lock);
  return tid;
}
