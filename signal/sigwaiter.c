/* sigwaiter.c - Signal handling functions.
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

#include <hurd/futex.h>

struct sigwaiter *sigwaiters;

siginfo_t
sigwaiter_block (struct signal_state *ss, const sigset_t *restrict set)
{
  assert (pthread_mutex_trylock (&sig_lock) == EBUSY);
  assert (pthread_mutex_trylock (&ss->lock) == EBUSY);

  assert (! ss->sigwaiter);

  struct sigwaiter waiter;

  waiter.next = sigwaiters;
  if (waiter.next)
    {
      assert (! waiter.next->prev);
      waiter.next->prev = &waiter;
    }
  waiter.prev = 0;
  sigwaiters = &waiter;

  waiter.ss = ss;
  waiter.info.si_signo = 0;
  waiter.signals = *set;

  ss->sigwaiter = &waiter;

  pthread_mutex_unlock (&ss->lock);
  pthread_mutex_unlock (&sig_lock);

  futex_wait (&waiter.info.si_signo, 0);

#ifndef NDEBUG
  pthread_mutex_lock (&ss->lock);
  ss->sigwaiter = 0;
  pthread_mutex_unlock (&ss->lock);
#endif

  assert (waiter.info.si_signo);
  return waiter.info;
}

void
sigwaiter_unblock (struct sigwaiter *waiter)
{
  assert (pthread_mutex_trylock (&sig_lock) == EBUSY);
  assert (pthread_mutex_trylock (&waiter->ss->lock) == EBUSY);

  struct sigwaiter *prev = waiter->prev;
  struct sigwaiter *next = waiter->next;

  if (next)
    next->prev = prev;

  if (prev)
    prev->next = next;
  else
    sigwaiters = next;

  sigdelset (&process_pending, waiter->info.si_signo);
  sigdelset (&waiter->ss->pending, waiter->info.si_signo);

  pthread_mutex_unlock (&waiter->ss->lock);
  pthread_mutex_unlock (&sig_lock);

  futex_wake (&waiter->info.si_signo, 1);
}
