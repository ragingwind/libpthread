/* Set a thread's signal state.  Hurd on L4 version.
   Copyright (C) 2002, 2005, 2008 Free Software Foundation, Inc.
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
#include <assert.h>
#include <signal.h>

#include <pt-internal.h>

error_t
__pthread_sigstate (struct __pthread *thread, int how,
		    const sigset_t *set, sigset_t *oset,
		    int clear_pending)
{
  struct signal_state *ss = &thread->ss;
  pthread_mutex_lock (&ss->lock);

  if (oset)
    *oset = ss->blocked;

  if (set)
    {
      /* Mask out SIGKILL and SIGSTOP.  */
      sigset_t s = *set;
      sigdelset (&s, SIGKILL);
      sigdelset (&s, SIGSTOP);

      switch (how)
	{
	case SIG_BLOCK:
	  ss->blocked |= s;
	  break;
	case SIG_UNBLOCK:
	  ss->blocked &= ~s;
	  break;
	case SIG_SETMASK:
	  ss->blocked = s;
	  break;
	default:
	  errno = EINVAL;
	  pthread_mutex_unlock (&ss->lock);
	  return -1;
	}
    }

  if (clear_pending)
    sigemptyset (&ss->pending);

  /* A "signal shall remain pending until it is unblocked" (2.4.1).

     "If there are any pending unblocked signals after the call to
     sigprocmask(), at least one of those signals shall be delivered
     before the call to sigprocmask() returns."
     (pthread_sigmask).  */
  sigset_t extant = ~ss->blocked & ss->pending;
  if (! extant)
    extant = ~ss->blocked & process_pending;

  pthread_mutex_unlock (&ss->lock);

  if (extant)
    raise (l4_lsb64 (extant));

  return 0;
}
