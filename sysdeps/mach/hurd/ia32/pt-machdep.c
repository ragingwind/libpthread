/* Machine dependent pthreads code.  Hurd/i386 version.
   Copyright (C) 2000,02 Free Software Foundation, Inc.
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

#include <errno.h>

#include <mach.h>
#include <mach/i386/thread_status.h>
#include <mach/thread_status.h>

int
__thread_set_pcsp (thread_t thread,
		   int set_ip, void *ip,
		   int set_sp, void *sp)
{
  error_t err;
  struct i386_thread_state state;
  mach_msg_type_number_t state_count;

  state_count = i386_THREAD_STATE_COUNT;

  err = __thread_get_state (thread, i386_THREAD_STATE,
			    (thread_state_t) &state, &state_count);
  if (err)
    return err;

  if (set_sp)
    state.uesp = (unsigned int) sp;
  if (set_ip)
    state.eip = (unsigned int) ip;

  err = __thread_set_state (thread, i386_THREAD_STATE,
			    (thread_state_t) &state,
			    i386_THREAD_STATE_COUNT);
  if (err)
    return err;

  return 0;
}
