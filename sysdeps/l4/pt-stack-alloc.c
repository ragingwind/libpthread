/* Allocate a new stack.  L4 Hurd version.
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

#include <l4/l4.h>
#include <errno.h>

#include <pt-internal.h>

#define __pthread_stacksize __pthread_default_attr.stacksize

#include <l4/sigma0.h>
#include <hurd/debug.h>

static void *
allocate_page (void)
{
  L4_Fpage_t p;
  /* The Kernel Interface page.  */
  static L4_KernelInterfacePage_t *kip;

  if (! kip)
    kip = L4_GetKernelInterface ();

#define sigma0_tid() (L4_GlobalId (kip->ThreadInfo.X.UserBase, 1))
  p =  L4_Sigma0_GetPage (sigma0_tid (),
			  L4_Fpage_Set_Attrs (L4_FpageLog2 (-1UL << 10,
							    PAGE_SHIFT),
					     L4_FullyAccessible));
  p.raw &= ~0x3ff;

  printf ("%s: Allocated page %x\n",
	  __FUNCTION__, p.raw);

  return (void *) p.raw;
}


/* Allocate a new stack of size STACKSIZE.  If successfull, store the
   address of the newly allocated stack in *STACKADDR and return 0.
   Otherwise return an error code (EINVAL for an invalid stack size,
   EAGAIN if the system lacked the necessary resources to allocate a
   new stack).  */
int
__pthread_stack_alloc (void **stackaddr, size_t stacksize)
{
  if (stacksize != __pthread_stacksize)
    return EINVAL;

  *stackaddr = allocate_page ();
  if (! *stackaddr)
    return EAGAIN;
  
  return 0;
}
