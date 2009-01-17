/* Cancel a thread.  Viengoos version.
   Copyright (C) 2002, 2007, 2008 Free Software Foundation, Inc.
   This file is part of the GNU Hurd.

   GNU Hurd is free software: you can redistribute it and/or modify it
   under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

   GNU Hurd is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with GNU Hurd.  If not, see
   <http://www.gnu.org/licenses/>.  */

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
  assert (p->cancel_pending == 1);
  assert (p->cancel_state == PTHREAD_CANCEL_ENABLE);

  if (hurd_myself () == p->threadid)
    call_exit ();
  else
    {
#ifdef USE_L4
      thread_start_sp_ip (p->object,
			  p->mcontext.sp, (uintptr_t) call_exit);
#else
# warning Unimplemented on this platform.
      assert (0);
#endif
    }

  return 0;
}
