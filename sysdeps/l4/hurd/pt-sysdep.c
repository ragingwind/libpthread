/* System dependent pthreads code.  Hurd version.
   Copyright (C) 2000 Free Software Foundation, Inc.
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

/* Forward.  */
static void *init_routine (void);

/* OK, the name of this variable isn't really appropriate, but I don't
   want to change it yet.  */
void *(*_pthread_init_routine)(void) = &init_routine;

/* This function is called from the Hurd-specific startup code.  It
   should return a new stack pointer for the main thread.  The caller
   will switch to this new stack before doing anything serious.  */
static void * 
init_routine (void)
{
  struct __pthread *thread;
  int err;

  /* Initialize the library.  */
  __pthread_initialize ();

  /* Create the pthread structure for the main thread (i.e. us).  */
  err = __pthread_create_internal (&thread, 0, 0, 0, 0);
  assert_perror (err);

  __pthread_initialize ();

  /* Decrease the number of threads, to take into account that the
     signal thread (which will be created by the startup code when we
     return from here) shouldn't be seen as a user thread.  */
#warning Need to implement the signal thread.
  // __pthread_total--;

  return (void *) thread->mcontext.sp;
}
