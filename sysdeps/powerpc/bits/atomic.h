/* Atomic operations.  PowerPC version.
   Copyright (C) 2003 Free Software Foundation, Inc.
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

#ifndef _BITS_ATOMIC_H
#define _BITS_ATOMIC_H	1

typedef __volatile int __atomic_t;

static inline void
__atomic_inc (__atomic_t *__var)
{
  int tmp;
  __asm__ ("\n\
0:      lwarx   %0,0,%1 \n\
        add%I2  %0,%0,%2        \n\
        stwcx.  %0,0,%1 \n\
        bne-    0b      \n\
" : "=&b"(tmp) : "r" (__var), "Ir"(1) : "cr0", "memory");
}

static inline void
__atomic_dec (__atomic_t *__var)
{
  int tmp;
  __asm__ ("\n\
0:      lwarx   %0,0,%1 \n\
        add%I2  %0,%0,%2        \n\
        stwcx.  %0,0,%1 \n\
        bne-    0b      \n\
" : "=&b"(tmp) : "r" (__var), "Ir"(-1) : "cr0", "memory");
}

static inline int
__atomic_dec_and_test (__atomic_t *__var)
{
  unsigned char __ret;

#if 0
  __asm__ __volatile ("lock; decl %0; sete %1"
		      : "=m" (*__var), "=qm" (__ret) : "m" (*__var));
#endif
  return __ret != 0;
}

/* We assume that an __atomicptr_t is only used for pointers to
   word-aligned objects, and use the lowest bit for a simple lock.  */
typedef __volatile int * __atomicptr_t;

/* Actually we don't implement that yet, and assume that we run on
   something that has the i486 instruction set.  */
static inline int
__atomicptr_compare_and_swap (__atomicptr_t *__ptr, void *__oldval,
			      void * __newval)
{
  int __ret;
  __asm__ ("\n\
0:      lwarx   %0,0,%1 \n\
        sub%I2c.        %0,%0,%2        \n\
        cntlzw  %0,%0   \n\
        bne-    1f      \n\
        stwcx.  %3,0,%1 \n\
        bne-    0b      \n\
1:      \n\
" : "=&b"(__ret) : "r"(__ptr), "Ir"(__oldval), "r"(__newval) : "cr0", "memory");
  return __ret >> 5;
}

#endif
