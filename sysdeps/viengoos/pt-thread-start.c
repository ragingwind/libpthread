/* Start thread.  Viengoos version.
   Copyright (C) 2007, 2008 Free Software Foundation, Inc.
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

#include <assert.h>
#include <errno.h>
#include <string.h>
#include <hurd/thread.h>

#include <pt-internal.h>

int
__pthread_thread_start (struct __pthread *thread)
{
  error_t err;

  if (__pthread_num_threads == 1)
    /* The main thread is already running of course.  */
    {
      assert (__pthread_total == 1);
      assert (hurd_myself () == thread->threadid);
    }
  else
    {
      struct vg_thread_exregs_in in;
      struct vg_thread_exregs_out out;

      vg_addr_t aspace = VG_ADDR (0, 0);
      in.aspace_cap_properties = VG_CAP_PROPERTIES_VOID;
      in.aspace_cap_properties_flags = VG_CAP_COPY_COPY_SOURCE_GUARD;

      vg_addr_t activity = VG_ADDR_VOID;

      in.sp = (uintptr_t) thread->mcontext.sp;
      in.ip = (uintptr_t) thread->mcontext.pc;

      in.user_handle = (uintptr_t) thread;
      err = vg_thread_exregs (VG_ADDR_VOID, thread->object,
			      VG_EXREGS_SET_ASPACE
			      | VG_EXREGS_SET_ACTIVITY
			      | VG_EXREGS_SET_SP_IP
			      | VG_EXREGS_SET_USER_HANDLE
			      | VG_EXREGS_START
			      | VG_EXREGS_ABORT_IPC,
			      in, aspace, activity, VG_ADDR_VOID, VG_ADDR_VOID,
			      &out, NULL, NULL, NULL, NULL);
      assert (err == 0);
    }
  return 0;
}
