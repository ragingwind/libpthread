/* sigprocmask.c - Generic sigprocmask implementation.
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

#include <pt-internal.h>
#include <sig-internal.h>

int
sigprocmask (int how, const sigset_t *restrict set, sigset_t *restrict old)
{
  struct __pthread *thread = _pthread_self ();
  if (! thread)
    /* Library is initializing.  */
    {
      assert (__pthread_num_threads == 1);

      /* We should get the default mask from the startup data structure.  */
      if (old)
	*old = 0;

      return 0;
    }

  return pthread_sigmask (how, set, old);
}
