/* signal-dispatch.c - Signal dispatcher.
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

/* This is the signal handler entry point.  A thread is forced into
   this state when it receives a signal.  We need to save the thread's
   state and then invoke the high-level signal dispatcher.  SS->LOCK
   is locked by the caller.  */
void
signal_dispatch (struct signal_state *ss, siginfo_t *si)
{
  SIGNAL_DISPATCH_ENTRY;

  int signo = si->si_signo;

  assert (signo > 0 && signo < NSIG);
  assert (pthread_mutex_trylock (&ss->lock) == EBUSY);

  do
    {
      if ((sigmask (signo) & STOPSIGS))
	/* Stop signals clear a pending SIGCONT even if they
	   are handled or ignored (but not if preempted).  */
	{
	  sigdelset (&ss->pending, SIGCONT);
	  sigdelset (&process_pending, SIGCONT);
	}
      else if ((signo == SIGCONT))
	/* Even if handled or ignored (but not preempted), SIGCONT
	   clears stop signals and resumes the process.  */
	{
	  ss->pending &= ~STOPSIGS;
	  process_pending &= ~STOPSIGS;
	}

      void (*handler)(int, siginfo_t *, void *)
	= ss->actions[signo - 1].sa_sigaction;

      /* Reset to SIG_DFL if requested.  SIGILL and SIGTRAP cannot
	 be automatically reset when delivered; the system silently
	 enforces this restriction (sigaction).  */
      if (ss->actions[signo - 1].sa_flags & SA_RESETHAND
	  && signo != SIGILL && signo != SIGTRAP)
	ss->actions[signo - 1].sa_handler = SIG_DFL;

      sigset_t orig_blocked = ss->blocked;
      /* Block requested signals while running the handler.  */
      ss->blocked |= ss->actions[signo - 1].sa_mask;

      /* Block SIGNO unless we're asked not to.  */
      if (! (ss->actions[signo - 1].sa_flags & (SA_RESETHAND | SA_NODEFER)))
	sigaddset (&ss->blocked, signo);

      sigdelset (&ss->pending, signo);
      pthread_mutex_unlock (&ss->lock);

      pthread_mutex_lock (&sig_lock);
      sigdelset (&process_pending, signo);
      pthread_mutex_unlock (&sig_lock);

      if (handler == (void *) SIG_DFL)
	{
	  enum sig_action action = default_action (signo);

	  if (action == sig_terminate || action == sig_core)
	    _exit (128 + signo);

	  if (action == sig_stop)
	    /* XXX: Implement me.  */
	    panic ("Stopping process unimplemented.");

	  if (action == sig_cont)
	    /* XXX: Implement me.  */;
	    panic ("Continuing process unimplemented.");
	}
      else if (handler == (void *) SIG_IGN)
	;
      else
	handler (signo, si, NULL);

      pthread_mutex_lock (&ss->lock);

      /* "When a thread's signal mask is changed in a signal-catching
	 function that is installed by sigaction(), the restoration of
	 the signal mask on return from the signal-catching function
	 overrides that change (see sigaction())" (sigprocmask).  */
      ss->blocked = orig_blocked;

      sigset_t pending = ~ss->blocked & ss->pending;
      if (! pending)
	pending = ~ss->blocked & process_pending;
      signo = l4_lsb64 (pending);
    }
  while (signo);

  pthread_mutex_unlock (&ss->lock);

  SIGNAL_DISPATCH_EXIT;
}
