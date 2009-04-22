/* Allocate a new stack.  L4 Hurd version.
   Copyright (C) 2000, 2007 Free Software Foundation, Inc.
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

#include <l4.h>
#include <errno.h>

#include <pt-internal.h>

#include <sys/mman.h>

/* Allocate a new stack of size STACKSIZE.  If successful, store the
   address of the newly allocated stack in *STACKADDR and return 0.
   Otherwise return an error code (EINVAL for an invalid stack size,
   EAGAIN if the system lacked the necessary resources to allocate a
   new stack).  */
int
__pthread_stack_alloc (void **stackaddr, size_t stacksize)
{
  void *buffer = mmap (0, stacksize, PROT_READ | PROT_WRITE,
		       MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  if (buffer == MAP_FAILED)
    return EAGAIN;

  *stackaddr = buffer;

  return 0;
}
