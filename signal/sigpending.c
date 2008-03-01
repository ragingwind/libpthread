/* sigpending.c - Generic sigpending implementation.
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
sigpending (sigset_t *set)
{
  struct signal_state *ss = &_pthread_self ()->ss;

  pthread_mutex_lock (&ss->lock);

  /* There is no need to lock SIG_LOCK for process_pending since we
     just read it, which is atomic.  */
  *set = (ss->pending | process_pending) & ss->blocked;

  pthread_mutex_unlock (&ss->lock);

  return 0;
}
