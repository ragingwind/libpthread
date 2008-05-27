/* Wakeup a thread.  Viengoos version.
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

#include <pt-internal.h>

#include <hurd/stddef.h>
#include <hurd/futex.h>
#include <stdint.h>

/* Wakeup THREAD.  */
void
__pthread_wakeup (struct __pthread *thread)
{
  /* We need to loop here as the blocked thread may not yet be
     blocked!  Here's what happens when a thread blocks: it registers
     itself as blocked, drops the relevant lock and then actually
     blocks (via __pthread_block).  This means that after dropping the
     lock and before blocking, it may be interrupted and another
     thread may try to wake it.  */
  long ret;
  do
    {
      ret = futex_wake (&thread->threadid, INT_MAX);
      assertx (ret <= 1, "tid: %x, ret: %d", thread->threadid, ret);

      if (ret == 0)
	l4_thread_switch (thread->threadid);
    }
  while (ret == 0);
}
