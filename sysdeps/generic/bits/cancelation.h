/* Cancelation.  Generic version.
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

#ifndef _BITS_CANCELATION_H
#define _BITS_CANCELATION_H	1

#include <assert.h>

struct __pthread_cancelation_handler
{
  void (*handler)(void *);
  void *arg;
  struct __pthread_cancelation_handler *next;
};

/* Returns the thread local location of the cleanup handler stack.  */
struct __pthread_cancelation_handler **__pthread_get_cleanup_stack (void);

#define pthread_cleanup_push(rt, rtarg) \
	{ \
	  struct __pthread_cancelation_handler **__handlers \
	    = __pthread_get_cleanup_stack (); \
	  struct __pthread_cancelation_handler __handler = \
	    { \
	      handler: (rt), \
	      arg: (rtarg), \
	      next: *__handlers \
	    }; \
	  *__handlers = &__handler;
	  
#define pthread_cleanup_pop(execute) \
	  if (execute) \
	    __handler.handler (__handler.arg); \
	  assert (*__handlers == &__handler); \
	  *__handlers = __handler.next; \
	}

#endif /* _BITS_CANCELATION_H */
