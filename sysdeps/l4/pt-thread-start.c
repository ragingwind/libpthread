/* Start thread.  L4 version.
   Copyright (C) 2003 Free Software Foundation, Inc.
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

/* Start THREAD.  Get the kernel thread scheduled and running.  */
int
__pthread_thread_start (struct __pthread *thread)
{
  /* The main thread is already running of course.  */
  if (__pthread_num_threads == 1)
    assert (__pthread_total == 1);
  else
    {
      l4_thread_id_t dest = thread->threadid;
      l4_word_t control = (L4_XCHG_REGS_SET_HALT | L4_XCHG_REGS_SET_SP
			   | L4_XCHG_REGS_SET_IP | L4_XCHG_REGS_SET_PAGER);
      l4_word_t sp = (l4_word_t) thread->mcontext.sp;
      l4_word_t ip = (l4_word_t) thread->mcontext.pc;
      l4_word_t dummy = 0;
      l4_thread_id_t pager = l4_pager ();

      l4_exchange_registers (&dest, &control, &sp, &ip, &dummy, &dummy, &pager);
    }
  return 0;
}
