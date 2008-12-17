/* Memory barrier operations.  Generic version.
   Copyright (C) 2008 Free Software Foundation, Inc.
   This file is part of the GNU Hurd.

   The GNU Hurd is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 3 of the
   License, or (at your option) any later version.

   The GNU Hurd is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see
   <http://www.gnu.org/licenses/>.  */

#ifndef _BITS_MEMORY_H
#define _BITS_MEMORY_H	1

/* Prevent read and write reordering across this function.  */
static inline void
__memory_barrier (void)
{
  /* Any lock'ed instruction will do.  */
  __sync_synchronize ();
}

/* Prevent read reordering across this function.  */
#define __memory_read_barrier __memory_barrier

/* Prevent write reordering across this function.  */
#define __memory_write_barrier __memory_barrier

#endif
