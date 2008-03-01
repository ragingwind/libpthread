/* pthread-kill-siginfo-np.c - Generic pthread_kill_siginfo_np implementation.
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

#include "pt-internal.h"
#include "sig-internal.h"

int
pthread_kill_siginfo_np (pthread_t tid, siginfo_t si)
{
  int sig = si.si_signo;

  if (sig < 0 || sig >= NSIG)
    return EINVAL;

  if (sig == 0)
    return 0;

  struct signal_state *ss = &__pthread_getid (tid)->ss;

  pthread_mutex_lock (&sig_lock);
  pthread_mutex_lock (&ss->lock);

  if (ss->sigwaiter && (ss->sigwaiter->signals & sigmask (si.si_signo)))
    /* The thread is in a call to sigwait.  */
    {
      ss->sigwaiter->info = si;
      sigwaiter_unblock (ss->sigwaiter);
      return 0;
    }

  pthread_mutex_unlock (&sig_lock);

  if (ss->actions[sig - 1].sa_handler == (void *) SIG_IGN
      || (ss->actions[sig - 1].sa_handler == (void *) SIG_DFL
	  && default_action (sig) == sig_ignore))
    /* It is unclear (to me) what is supposed to happen when a signal
       is generated for a thread, which is blocking that signal and
       ignoring it.  POSIX does say that when the action associated
       with a pending, blocked signal is set to SIG_IGN, the pending
       signal is to be cleared.  Thus, it makes sense that any signal
       set to ignore is discarded at generation.  */
    {
      pthread_mutex_unlock (&ss->lock);
      return 0;
    }


  if ((sigmask (sig) & ss->blocked))
    /* The signal is blocked.  Mark it pending.  */
    {
      ss->pending |= sigmask (sig);
      pthread_mutex_unlock (&ss->lock);
      return 0;
    }

  if (pthread_self () == tid
      && (! (ss->actions[si.si_signo - 1].sa_flags & SA_ONSTACK)
	  || (ss->stack.ss_flags & SS_DISABLE)
	  || (ss->stack.ss_flags & SS_ONSTACK)))
    /* We are sending a signal to ourself and we don't use an
       alternate stack.  */
    signal_dispatch (ss, &si);
  else
    signal_dispatch_lowlevel (ss, tid, si);

  /* Don't unlock ss: signal_dispatch and signal_dispatch_lowlevel
     assume ownership of the lock.  */

  return 0;
}

