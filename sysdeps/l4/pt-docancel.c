/* Cancel a thread.
   Copyright (C) 2002 Free Software Foundation, Inc.
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

#include <pthread.h>

#include <pt-internal.h>

static void
call_exit (void)
{
  pthread_exit (0);
}

int
__pthread_do_cancel (struct __pthread *p)
{
#if 0
  assert (p->cancel_pending = 1);
  assert (p->cancel_state == PTHREAD_CANCEL_ENABLE);

  if (L4_SameThreads (L4_Myself (), p->threadid))
    call_exit ();
  else
    {
      L4_Word_t dummy;
      L4_ThreadId_t dummy_id;

      /* Change the ip of the target thread to make it exit.  */
      L4_ExchangeRegisters (p->threadid, (1 << 4), 0, call_exit,
			    0, 0, L4_nilthread,
			    &dummy, &dummy, &dummy, &dummy, &dummy,
			    &dummy_id);
    }
#endif
  return 0;
}
