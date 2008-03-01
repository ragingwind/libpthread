/* sig-internal.h - Internal signal handling interface.
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

#ifndef SIG_INTERNAL_H
#define SIG_INTERNAL_H

#include <signal.h>

#include <sig-sysdep.h>

#define sigmask(sig) (1ULL << (sig - 1))
#define STOPSIGS (sigmask (SIGTTIN) | sigmask (SIGTTOU) | \
		  sigmask (SIGSTOP) | sigmask (SIGTSTP))

/* General lock.  Protects PROCESS_PENDING, PROCESS_PENDING_INFO,
   SIGWAITERS.  */
extern pthread_mutex_t sig_lock;

/* "Signals generated for the process shall be delivered to exactly
   one of those threads within the process which is in a call to a
   sigwait() function selecting that signal or has not blocked
   delivery of the signal.  If there are no threads in a call to a
   sigwait() function selecting that signal, and if all threads within
   the process block delivery of the signal, the signal shall remaing
   pending on the process"  (2.4.1).

   This variable is protected by SIG_LOCK.  */
extern sigset_t process_pending;
extern siginfo_t process_pending_info[NSIG];

struct sigwaiter;

/* The per-thread signal state.  */
struct signal_state
{
  /* Protects the following fields.  STACK.SA_FLAGS may be accessed
     using atomic operations.  */
  pthread_mutex_t lock;

  /* Pending signals.  */
  sigset_t pending;

  /* Blocked signals (i.e., the signal mask).  */
  sigset_t blocked;

  stack_t stack;
  struct sigaction actions[NSIG];
  siginfo_t info[NSIG];

  /* If the thread is blocked in a call to sigwait.  */
  struct sigwaiter *sigwaiter;
};

#define PTHREAD_SIGNAL_MEMBERS struct signal_state ss;

/* Arranges for thread TID to call signal_dispatch.  Must not be
   called if TID is the caller and an alternate stack is not required.
   In this case, the caller should call signal_dispatch directly.  */
extern void signal_dispatch_lowlevel (struct signal_state *ss,
				      pthread_t tid, siginfo_t si);

/* This is the signal handler entry point.  A thread is forced into
   this state when it receives a signal.  We need to save the thread's
   state and then invoke the high-level signal dispatcher.  SS->LOCK
   is locked by the caller.  */
extern void signal_dispatch (struct signal_state *ss, siginfo_t *si);

#ifndef SIGNAL_DISPATCH_ENTRY
#define SIGNAL_DISPATCH_ENTRY
#endif

#ifndef SIGNAL_DISPATCH_EXIT
#define SIGNAL_DISPATCH_EXIT
#endif

/* When a thread calls sigwait and a requested signal is not pending,
   it allocates the following structure, fills it in, adds it to
   sigwaiters and sleeps.  */
struct sigwaiter
{
  struct sigwaiter *next;
  struct sigwaiter *prev;

  /* Thread's signal state.  */
  struct signal_state *ss;

  /* Signals this thread is waiting for.  */
  sigset_t signals;

  /* The selected signal is returned here.  The waiter also
     futex_waits on this info.si_signo.  */
  siginfo_t info;
};

/* This variable is protected by SIG_LOCK.  */
extern struct sigwaiter *sigwaiters;

/* Block the caller waiting for a signal in set SET.  SIG_LOCK and
   SS->LOCK must be held and will be unlocked by this function before
   blocking.  */
extern siginfo_t sigwaiter_block (struct signal_state *ss,
				  const sigset_t *restrict set);

/* Unblock the waiter WAITER.  SIG_LOCK and WAITER->SS->LOCK must be
   held.  Both will be dropped on return.  */
extern void sigwaiter_unblock (struct sigwaiter *waiter);

enum sig_action { sig_core, sig_terminate, sig_ignore, sig_cont, sig_stop };

static inline enum sig_action
default_action (int signo)
{
  switch (signo)
    {
    case SIGABRT:
    case SIGBUS:
    case SIGFPE:
    case SIGILL:
    case SIGQUIT:
    case SIGSEGV:
    case SIGSTKFLT:
    case SIGSYS:
    case SIGTRAP:
    case SIGXCPU:
    case SIGXFSZ:
      return sig_core;

    case SIGALRM:
    case SIGHUP:
    case SIGINT:
    case SIGIO: /* Perhaps ignore?  */
    case SIGKILL:
    case SIGPIPE:
    case SIGPROF:
    case SIGTERM:
    case SIGUSR1:
    case SIGUSR2:
    case SIGVTALRM:
      return sig_terminate;

    case SIGCHLD:
    case SIGPWR:
    case SIGURG:
    case SIGWINCH:
      return sig_ignore;

    case SIGCONT:
      return sig_cont;

    case SIGSTOP:
    case SIGTSTP:
    case SIGTTIN:
    case SIGTTOU:
      return sig_stop;
    }

  panic ("Unknown signal number: %d", signo);
}

#endif
