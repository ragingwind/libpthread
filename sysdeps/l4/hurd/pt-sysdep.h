/* Internal defenitions for pthreads library.
   Copyright (C) 2000, 2002, 2005, 2007 Free Software Foundation, Inc.
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

#ifndef _PT_SYSDEP_H
#define _PT_SYSDEP_H	1

#include <l4.h>
#include <hurd/storage.h>
#include <sys/mman.h>

/* XXX */
#define _POSIX_THREAD_THREADS_MAX	64

/* The default stack size: 2MB.  */
#define PTHREAD_STACK_DEFAULT	(2 * 1024 * 1024)

#define PTHREAD_SYSDEP_MEMBERS \
  struct storage object; \
  l4_thread_id_t threadid; \
  struct storage exception_page; \
  l4_word_t my_errno;

extern inline struct __pthread *
__attribute__((__always_inline__))
_pthread_self (void)
{
  return (struct __pthread *) l4_user_defined_handle ();
}

extern inline void
__attribute__((__always_inline__))
__pthread_stack_dealloc (void *stackaddr, size_t stacksize)
{
  munmap (stackaddr, stacksize);
}

#endif /* pt-sysdep.h */
