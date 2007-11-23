/* Start thread.  L4 version.
   Copyright (C) 2007 Free Software Foundation, Inc.
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
#include <hurd/thread.h>
#include <hurd/exceptions.h>

#include <pt-internal.h>

int
__pthread_thread_start (struct __pthread *thread)
{
  error_t err;

  if (__pthread_num_threads == 1)
    /* The main thread is already running of course.  */
    {
      assert (__pthread_total == 1);
      assert (l4_is_thread_equal (l4_myself (), thread->threadid));
      l4_set_user_defined_handle_of (hurd_exception_thread (l4_myself ()),
				     (l4_word_t) thread);
      l4_set_user_defined_handle ((l4_word_t) thread);
    }
  else
    {
      struct cap_addr_trans addr_trans = CAP_ADDR_TRANS_VOID;

      /* First, start the exception thread.  */
      l4_word_t dummy;
      err = rm_thread_exregs (ADDR_VOID, thread->object.addr,
			      HURD_EXREGS_EXCEPTION_THREAD
			      | HURD_EXREGS_SET_SP_IP
			      | HURD_EXREGS_SET_USER_HANDLE
			      | HURD_EXREGS_START
			      | HURD_EXREGS_ABORT_IPC,
			      ADDR_VOID, 0, addr_trans, ADDR_VOID,
			      (l4_word_t) thread->exception_handler_sp,
			      (l4_word_t) exception_handler_loop, 0,
			      thread,
			      ADDR_VOID, ADDR_VOID,
			      &dummy, &dummy, &dummy, &dummy);
      assert (err == 0);

      err = rm_thread_exregs (ADDR_VOID, thread->object.addr,
			      HURD_EXREGS_SET_ASPACE
			      | HURD_EXREGS_SET_ACTIVITY
			      | HURD_EXREGS_SET_SP_IP
			      | HURD_EXREGS_SET_USER_HANDLE
			      | HURD_EXREGS_START
			      | HURD_EXREGS_ABORT_IPC,
			      ADDR (0, 0),
			      CAP_COPY_COPY_SOURCE_GUARD, addr_trans,
			      ADDR_VOID,
			      (l4_word_t) thread->mcontext.sp,
			      (l4_word_t) thread->mcontext.pc, 0,
			      thread,
			      ADDR_VOID, ADDR_VOID,
			      &dummy, &dummy, &dummy, &dummy);
      assert (err == 0);
    }
  return 0;
}
