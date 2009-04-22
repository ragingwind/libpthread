/* kill.c - Generic kill implementation.
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

int
kill (pid_t pid, int signo)
{
  if (pid != getpid ())
    {
      errno = EOPNOTSUPP;
      return -1;
    }

  /* "Signals generated for the process shall be delivered to exactly
     one of those threads within the process which is in a call to a
     sigwait() function selecting that signal or has not blocked
     delivery of the signal.  If there are no threads in a call to a
     sigwait() function selecting that signal, and if all threads
     within the process block delivery of the signal, the signal shall
     remaing pending on the process" (2.4.1).  */

  /* First, see if there is a waiter, which is interested in this
     signal.  */
  pthread_mutex_lock (&sig_lock);

  struct sigwaiter *waiter;
  for (waiter = sigwaiters; waiter; waiter = waiter->next)
    if ((waiter->signals & sigmask (signo)))
      /* Got a winner.  */
      {
	sigdelset (&process_pending, signo);

	pthread_mutex_lock (&waiter->ss->lock);
	sigdelset (&waiter->ss->pending, signo);

	memset (&waiter->info, 0, sizeof (waiter->info));
	waiter->info.si_signo = signo;

	sigwaiter_unblock (waiter);

	return 0;
      }

  pthread_mutex_unlock (&sig_lock);

  /* XXX: We just generate the signal for the current thread.  If the
     current thread has blocked the signal, the correct thing to do is
     to iterate over all the other threads and find on that hasn't
     blocked it.  */
  return pthread_kill (pthread_self (), signo);
}

