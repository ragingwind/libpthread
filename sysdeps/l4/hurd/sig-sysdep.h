/* sig-sysdep.h - Hurd system specific header file.
   Copyright (C) 2008 Free Software Foundation, Inc.
   Written by Neal H. Walfield <neal@gnu.org>.

   This file is part of the GNU Hurd.

   The GNU Hurd is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public License
   as published by the Free Software Foundation; either version 3 of
   the License, or (at your option) any later version.

   The GNU Hurd is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this program.  If not, see
   <http://www.gnu.org/licenses/>.  */

#include <l4.h>
#include <string.h>

struct utcb
{
  l4_word_t saved_sender;
  l4_word_t saved_receiver;
  l4_word_t saved_timeout;
  l4_word_t saved_error_code;
  l4_word_t saved_flags;
  l4_word_t saved_br0;
  l4_msg_t saved_message;
};

static inline void
utcb_state_save (struct utcb *buffer)
{
  l4_word_t *utcb = _L4_utcb ();

  buffer->saved_sender = utcb[_L4_UTCB_SENDER];
  buffer->saved_receiver = utcb[_L4_UTCB_RECEIVER];
  buffer->saved_timeout = utcb[_L4_UTCB_TIMEOUT];
  buffer->saved_error_code = utcb[_L4_UTCB_ERROR_CODE];
  buffer->saved_flags = utcb[_L4_UTCB_FLAGS];
  buffer->saved_br0 = utcb[_L4_UTCB_BR0];
  memcpy (&buffer->saved_message,
	  utcb, L4_NUM_MRS * sizeof (l4_word_t));
}

static inline void
utcb_state_restore (struct utcb *buffer)
{
  l4_word_t *utcb = _L4_utcb ();

  utcb[_L4_UTCB_SENDER] = buffer->saved_sender;
  utcb[_L4_UTCB_RECEIVER] = buffer->saved_receiver;
  utcb[_L4_UTCB_TIMEOUT] = buffer->saved_timeout;
  utcb[_L4_UTCB_ERROR_CODE] = buffer->saved_error_code;
  utcb[_L4_UTCB_FLAGS] = buffer->saved_flags;
  utcb[_L4_UTCB_BR0] = buffer->saved_br0;
  memcpy (utcb, &buffer->saved_message,
	  L4_NUM_MRS * sizeof (l4_word_t));
}

#define SIGNAL_DISPATCH_ENTRY \
  struct utcb buffer; utcb_state_save (&buffer);

#define SIGNAL_DISPATCH_EXIT \
  utcb_state_restore (&buffer);
