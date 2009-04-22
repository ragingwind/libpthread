/* Initialize the signal state.  Hurd on L4 version.
   Copyright (C) 2003, 2008 Free Software Foundation, Inc.
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
#include <sig-internal.h>

error_t
__pthread_sigstate_init (struct __pthread *thread)
{
  struct signal_state *ss = &thread->ss;

  memset (ss, 0, sizeof (*ss));

  ss->stack.ss_flags = SS_DISABLE;

  int signo;
  for (signo = 1; signo < NSIG; ++signo)
    {
      sigemptyset (&ss->actions[signo - 1].sa_mask);
      ss->actions[signo - 1].sa_flags = SA_RESTART;
      ss->actions[signo - 1].sa_handler = SIG_DFL;
      ss->lock = (pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER;
    }

  return 0;
}
