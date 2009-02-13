/* System dependent pthreads code.  Hurd version.
   Copyright (C) 2000, 2008 Free Software Foundation, Inc.
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
#include <stddef.h>
#include <stdint.h>

#include <pt-internal.h>

int
sched_yield (void)
{
#ifdef USE_L4
  l4_yield ();
#else
# warning Not ported to this platform.
#endif
  return 0;
}

/* Forward.  */
static void init_routine (void (*) (void *), void *)
  __attribute__ ((noreturn));

/* OK, the name of this variable isn't really appropriate, but I don't
   want to change it yet.  */
void (*_pthread_init_routine)(void (*) (void *), void *) = &init_routine;

/* This function is called from the Hurd-specific startup code.  It
   should return a new stack pointer for the main thread.  The caller
   will switch to this new stack before doing anything serious.  */
static void
init_routine (void (*entry) (void *), void *arg)
{
  /* Initialize the library.  */
  __pthread_initialize ();

  struct __pthread *thread;
  int err;

  /* Create the pthread structure for the main thread (i.e. us).  */
  err = __pthread_create_internal (&thread, 0,
				   (void *(*)(void *)) entry, arg);
  assert_perror (err);

  /* Switch stacks and jump to THREAD->MCONTEXT.PC.  */
#ifdef __x86_64
  uintptr_t sp = (uintptr_t) thread->mcontext.sp - sizeof (uintptr_t);
  *(uintptr_t *) sp = thread->mcontext.pc;

  asm ("mov %0, %%rsp\n\t"
       "ret\n\t" :: "r" (sp));
#else
  /* There is nothing wrong with using vg_thread_start_sp_ip to
     implement the above: it just means an extra kernel entry.  */
  err = vg_thread_start_sp_ip (thread->object, thread->mcontext.sp,
			       thread->mcontext.pc);
#endif
  panic ("Failed to set new IP: %d.", err);
}
