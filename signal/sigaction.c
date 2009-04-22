/* sigaction.c - Generic sigaction implementation.
   Copyright (C) 2008 Free Software Foundation, Inc.
   Written by Neal H. Walfield <neal@gnu.org>.

   This file is part of the GNU Hurd.

   The GNU Hurd is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public License
   as published by the Free Software Foundation; either version 3 of
   the License, or (at your option) any later version.

   The GNU Hurd is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this program.  If not, see
   <http://www.gnu.org/licenses/>.  */

#include "sig-internal.h"
#include "pt-internal.h"

int
sigaction (int sig, const struct sigaction *restrict sa,
	   struct sigaction *restrict osa)
{
  if (sig <= 0 || sig >= NSIG)
    {
      errno = EINVAL;
      return -1;
    }

  struct signal_state *ss = &_pthread_self ()->ss;

  pthread_mutex_lock (&ss->lock);

  if (osa)
    *osa = ss->actions[sig - 1];

  if (sa)
    {
      ss->actions[sig - 1] = *sa;

      /* "The SIGKILL and SIGSTOP signals shall not be added to the
	 signal mask using this mechanism; this restriction shall be
	 enforced by the system without causing an error to be
	 indicated" (sigaction).  */
      sigdelset (&ss->blocked, SIGKILL);
      sigdelset (&ss->blocked, SIGSTOP);

      /* A "signal shall remain pending on the process until it is
	 unblocked, it is accepted when ..., or the action associated
	 with it is set to ignore the signal"  (2.4.1).

         "Setting a signal action to SIG_DFL for a signal that is
         pending, and whose default action is to ignore the signal,
         ..., shall cause the pending signal to be discarded, whether
         or not it is blocked" (2.4.3).  */
      if (sa->sa_handler == SIG_IGN
	  || (sa->sa_handler == SIG_DFL && default_action (sig) == sig_ignore))
	{
	  sigdelset (&ss->pending, sig);
	  sigdelset (&process_pending, sig);
	}
    }

  pthread_mutex_unlock (&ss->lock);

  return 0;
}

