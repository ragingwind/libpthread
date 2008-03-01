/* sigwaitinfo.c - Generic sigwaitinfo implementation.
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

#include <sig-internal.h>
#include <pt-internal.h>

int
sigwaitinfo (const sigset_t *restrict set, siginfo_t *restrict info)
{
  pthread_mutex_lock (&sig_lock);

  struct signal_state *ss = &_pthread_self ()->ss;

  pthread_mutex_lock (&ss->lock);

  if ((process_pending & *set) || (ss->pending & *set))
    /* There is at least one signal pending.  */
    {
      bool local = true;
      sigset_t extant = process_pending & *set;
      if (! extant)
	{
	  local = false;
	  extant = ss->pending & *set;
	}

      assert (extant);

      int signo = l4_msb64 (extant);

      if (info)
	{
	  if (local)
	    *info = ss->info[signo - 1];
	  else
	    *info = process_pending_info[signo - 1];
	  info->si_signo = signo;
	}

      sigdelset (&process_pending, signo);
      sigdelset (&ss->pending, signo);

      pthread_mutex_unlock (&ss->lock);
      pthread_mutex_unlock (&sig_lock);
      return 0;
    }

  siginfo_t i = sigwaiter_block (ss, set);
  assert (i.si_signo);
  assert ((sigmask (i.si_signo) & *set));

  if (info)
    *info = i;

  return 0;
}

