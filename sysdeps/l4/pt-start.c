/* Start thread.  L4 Hurd version.
   Copyright (C) 2002 Free Software Foundation, Inc.
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

#include <pt-internal.h>

#include "task_client.h"

extern L4_ThreadId_t __system_pager;
extern L4_ThreadId_t __task_server;

#ifndef WORKING_EXREGS
static void
send_startup_ipc (L4_ThreadId_t id, L4_Word_t ip, L4_Word_t sp)
{
  L4_Msg_t msg;

  printf ("%s: Sending startup message to %x, "
	  "(ip=%x, sp=%x)\n",
	  __FUNCTION__, * (L4_Word_t *) &id, ip, sp);

  L4_Clear (&msg);
#ifdef HAVE_PROPAGATION
  L4_Set_VirtualSender (pager_tid);
  L4_Set_Propagation (&msg.tag);
#endif
  L4_Append_Word (&msg, ip);
  L4_Append_Word (&msg, sp);
#ifndef HAVE_PROPAGATION
  L4_Append_Word (&msg, *(L4_Word_t *) &id);
  id = __system_pager;
#if 0
  DODEBUG (2, printf ("%s: Redirecting start request to pager (%x).\n",
		      __FUNCTION__, * (L4_Word_t *) &id));
#endif
#endif
  L4_LoadMsg (&msg);
  L4_Send (id);
}
#endif

/* Start THREAD.  We allocate all system-specific resources, including
   a kernel thread, set it up, and get it running.  */
int
__pthread_start (struct __pthread *thread)
{
  error_t err;

  if (__pthread_num_threads == 1)
    /* The main thread is already running: do nothing.  */
    {
      assert (__pthread_total == 1);
      thread->threadid = L4_Myself ();
    }
  else
    {
      CORBA_Environment env;

      env = idl4_default_environment;
      err = thread_create (__task_server,
			   L4_Version (L4_Myself ()),
			   * (L4_Word_t *) &__system_pager,
			   (L4_Word_t *) &thread->threadid, &env);
      if (err)
	return EAGAIN;

      env = idl4_default_environment;
      err = thread_resume (__task_server,
			   * (L4_Word_t *) &thread->threadid,
			   &env);
      assert (! err);

#ifndef WORKING_EXREGS
      L4_AbortIpc_and_stop (thread->threadid);
      L4_Start_SpIp (thread->threadid, (L4_Word_t) thread->mcontext.sp,
		     (L4_Word_t) thread->mcontext.pc);
#endif
      send_startup_ipc (thread->threadid, (L4_Word_t) thread->mcontext.pc,
			(L4_Word_t) thread->mcontext.sp);

    }

  return 0;
}
