/* signal.h - Signal handling interface.
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

#ifndef _SIGNAL_H
#define _SIGNAL_H 1

#include <stdint.h>
#include <sys/types.h>

typedef volatile int sig_atomic_t;

typedef uint64_t sigset_t;

int sigaddset (sigset_t *, int);
int sigdelset (sigset_t *, int);
int sigemptyset (sigset_t *);
int sigfillset (sigset_t *);
int sigismember (const sigset_t *, int);

/* These values are consistent with Linux.  */
#define SIGRTMIN 34
#define SIGRTMAX 64

enum
  {
    SIGHUP = 1,
#define SIGHUP SIGHUP
    SIGINT,
#define SIGINT SIGINT
    SIGQUIT,
#define SIGQUIT SIGQUIT
    SIGILL,
#define SIGILL SIGILL
    SIGTRAP,
#define SIGTRAP SIGTRAP
    SIGABRT,
#define SIGABRT SIGABRT
    SIGBUS,
#define SIGBUS SIGBUS
    SIGFPE,
#define SIGFPE SIGFPE
    SIGKILL,
#define SIGKILL SIGKILL
    SIGUSR1,
#define SIGUSR1 SIGUSR1
    SIGSEGV,
#define SIGSEGV SIGSEGV
    SIGUSR2,
#define SIGUSR2 SIGUSR2
    SIGPIPE,
#define SIGPIPE SIGPIPE
    SIGALRM,
#define SIGALRM SIGALRM
    SIGTERM,
#define SIGTERM SIGTERM
    SIGSTKFLT,
#define SIGSTKFLT SIGSTKFLT
    SIGCHLD,
#define SIGCHLD SIGCHLD
    SIGCONT,
#define SIGCONT SIGCONT
    SIGSTOP,
#define SIGSTOP SIGSTOP
    SIGTSTP,
#define SIGTSTP SIGTSTP
    SIGTTIN,
#define SIGTTIN SIGTTIN
    SIGTTOU,
#define SIGTTOU SIGTTOU
    SIGURG,
#define SIGURG SIGURG
    SIGXCPU,
#define SIGXCPU SIGXCPU
    SIGXFSZ,
#define SIGXFSZ SIGXFSZ
    SIGVTALRM,
#define SIGVTALRM SIGVTALRM
    SIGPROF,
#define SIGPROF SIGPROF
    SIGWINCH,
#define SIGWINCH SIGWINCH
    SIGIO,
#define SIGIO SIGIO
    SIGPWR,
#define SIGPWR SIGPWR
    SIGSYS,
#define SIGSYS SIGSYS
    NSIG
  };

/* The resulting set is the union of the current set and the signal
   set pointed to by the argument set.  */
#define SIG_BLOCK 1
/* The resulting set is the intersection of the current set and the
   complement of the signal set pointed to by the argument set.  */
#define SIG_UNBLOCK 2
/* The resulting set is the signal set pointed to by the argument
   set.  */
#define SIG_SETMASK 3

int pthread_sigmask (int how, const sigset_t *mask, sigset_t *old);
int sigprocmask (int how, const sigset_t *restrict mask,
		 sigset_t *restrict old);

/* Return set of pending signals.  */
int sigpending(sigset_t *set);

union sigval
{
  int sival_int;
  void *sival_ptr;
};

#define SIG_DFL ((void (*)(int)) (0))
#define SIG_ERR ((void (*)(int)) (-1))
#define SIG_IGN ((void (*)(int)) (1))

/* Causes signal delivery to occur on an alternate stack.  */
#define SA_ONSTACK (1 << 0)
/* Do not generate SIGCHLD when children stop or stopped children
   continue.  */
#define SA_NOCLDSTOP (1 << 1)
/* Causes signal dispositions to be set to SIG_DFL on entry to signal
   handlers.  */
#define SA_RESETHAND (1 << 2)
/* Causes certain functions to become restartable.  */
#define SA_RESTART (1 << 3)
/* Causes extra information to be passed to signal handlers at the
   time of receipt of a signal.  */
#define SA_SIGINFO (1 << 4)
/* Causes implementations not to create zombie processes on child
   death.  */
#define SA_NOCLDWAIT (1 << 5)
/* Causes signal not to be automatically blocked on entry to
   signal handler.  */
#define SA_NODEFER (1 << 6)

typedef struct
{
  int si_signo;
  int si_code;
  int si_errno;
  pid_t si_pid;
  uid_t si_uid;
  void *si_addr;
  int si_status;
  long si_band;
  union sigval si_value;
} siginfo_t;

struct sigaction
{
  union
  {
    /* Pointer to a signal-catching function or one of the macros
       SIG_IGN or SIG_DFL.  */
    void (*sa_handler)(int);
                   
    /* Pointer to a signal-catching function.  */
    void (*sa_sigaction)(int, siginfo_t *, void *);
  };

  /* Set of signals to be blocked during execution of the signal
     handling function.  */
  sigset_t sa_mask;

  /* Special flags.  */
  int sa_flags;
};

int sigaction (int signo, const struct sigaction *restrict newaction,
	       struct sigaction *restrict oldaction);

void (*signal (int signo, void (*handler)(int)))(int);
void (*bsd_signal (int signo, void (*handler)(int)))(int);

/* Process is executing on an alternate signal stack.  */
#define SS_ONSTACK (1 << 0)
/* Alternate signal stack is disabled.  */
#define SS_DISABLE (1 << 1)

/* Minimum stack size for a signal handler.  */
#define MINSIGSTKSZ PAGESIZE
/* Default size in bytes for the alternate signal stack. */
#define SIGSTKSZ (16 * PAGESIZE)

typedef struct
{
  void *ss_sp;
  size_t ss_size;
  int ss_flags;
} stack_t;

int sigaltstack(const stack_t *restrict stack, stack_t *restrict old);

#include <pthread.h>

/* Send SIGNO to the process PID.  */
int kill(pid_t pid, int signo);

/* Send SIGNO to the process group PG.  */
int killpg(pid_t pg, int signo);

/* Send SIGNO to thread TID.  */
int pthread_kill(pthread_t tid, int signo);

/* Send a signal to thread TID using SIGINFO.  */
int pthread_kill_siginfo_np (pthread_t tid, siginfo_t siginfo);

/* Send SIGNO to the calling thread.  */
int raise(int signo);

typedef struct sigevent
{
  /* Notification type.  */
  int sigev_notify;

  /* Signal number.  */
  int sigev_signo;

  /* Signal value.  */
  union sigval sigev_value;

  /* Notification function.  */
  void (*sigev_notify_function) (union sigval);

  /* Notification attributes.  */
  pthread_attr_t *sigev_notify_attributes;
} sigevent_t;

enum
  {
    SIGEV_NONE = 0,
#define SIGEV_NONE SIGEV_NONE
    SIGEV_SIGNAL,
#define SIGEV_SIGNAL SIGEV_SIGNAL
    SIGEV_THREAD
#define SIGEV_THREAD SIGEV_THREAD
  };

#define SIG_HOLD

int sighold (int);
int sigignore (int);
int siginterrupt (int, int);
int sigpause (int);
int sigqueue (pid_t, int, const union sigval);
int sigrelse (int);
void (*sigset (int, void (*)(int)))(int);
int sigsuspend (const sigset_t *);

/* Wait for a signal.  */
int sigwait (const sigset_t *restrict set, int *restrict signo);
int sigwaitinfo (const sigset_t *restrict set, siginfo_t *restrict info);
int sigtimedwait (const sigset_t *restrict set, siginfo_t *restrict info,
		  const struct timespec *restrict timespec);

#endif
