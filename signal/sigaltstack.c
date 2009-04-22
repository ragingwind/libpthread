/* sigaltstack.c - Generic sigaltstack implementation.
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
#include "pt-internal.h"

int
sigaltstack (const stack_t *restrict stack, stack_t *restrict old)
{
  int err = 0;
  struct signal_state *ss = &_pthread_self ()->ss;

  pthread_mutex_lock (&ss->lock);

  if (old)
    *old = ss->stack;

  if (stack)
    {
      if (stack->ss_size < MINSIGSTKSZ)
	{
	  err = ENOMEM;
	  goto out;
	}

      if ((stack->ss_flags & ~(SS_DISABLE)))
	/* Flags contains a value other than SS_DISABLE.  */
	{
	  err = EINVAL;
	  goto out;
	}

      if ((ss->stack.ss_flags & SS_ONSTACK))
	/* Stack in use.  */
	{
	  err = EPERM;
	  goto out;
	}

      ss->stack = *stack;
    }

 out:
  pthread_mutex_unlock (&ss->lock);

  if (err)
    {
      errno = err;
      return -1;
    }
  return 0;
}
