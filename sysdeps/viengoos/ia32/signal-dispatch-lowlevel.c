/* signal-dispatch-lowlevel.c - ia32 specific signal handling functions.
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

#include <pt-internal.h>
#include <sig-internal.h>

#include <hurd/thread.h>
#include <pthread.h>
#include <stdint.h>
#include <atomic.h>

extern char _signal_dispatch_entry;
/* - 0(%esp) a pointer to the thread's struct signal_state.
   - 4(%esp) a pointer to a siginfo_t.
   - 8(%esp) is a pointer to the ss_flags field (or NULL).
   - 12(%esp)+4 is the value of the sp when the thread was interrupted (intr_sp)
     - 0(intr_sp) is the value of the ip when the thread was interrupted.
   - 16(%esp) - 16 byte register save area
*/
__asm__ ("\n\
	.globl _signal_dispatch_entry\n\
_signal_dispatch_entry:\n\
	/* Save caller saved registers (16 bytes).  */\n\
	mov %eax, 16(%esp)\n\
	mov %ecx, 16+4(%esp)\n\
	mov %edx, 16+8(%esp)\n\
	pushf\n\
	popl %eax\n\
	mov %eax, 16+12(%esp)\n\
\n\
	/* Reset EFLAGS.  */\n\
	cld\n\
	call signal_dispatch\n\
\n\
	/* Get the original stack and begin restoration.  */\n\
	mov 12(%esp), %edx\n\
\n\
	/* Move the saved registers to the user stack.  */\n\
	sub $16, %edx\n\
	/* eax.  */\n\
	mov 16+0(%esp), %ecx\n\
	mov %ecx, 0(%edx)\n\
	/* ecx.  */\n\
	mov 16+4(%esp), %ecx\n\
	mov %ecx, 4(%edx)\n\
	/* edx.  */\n\
	mov 16+8(%esp), %ecx\n\
	mov %ecx, 8(%edx)\n\
	/* eflags.  */\n\
	mov 16+12(%esp), %ecx\n\
	mov %ecx, 12(%edx)\n\
\n\
	/* Get the pointer to the sigaltstack flags.  */\n\
	mov 8(%esp), %ecx\n\
\n\
	/* Restore the user stack.  */\n\
	mov %edx, %esp\n\
\n\
	/* Clear the SA_ONSTACK flag.  */\n\
	and %ecx, %ecx\n\
	jz after_clear\n\
	lock; and $~1, 0(%ecx)\n\
after_clear:\n\
\n\
	/* Restore eflags, the scratch regs and the original sp and ip.  */\n\
	popl %eax\n\
	popl %ecx\n\
	popl %edx\n\
	popf\n\
	ret\n");

extern char _signal_dispatch_entry_self;
/* - 0(%esp) is the return address (we ignore it)
   - 4(%esp) is the sp to load

   Since we are returning to signal_dispatch_lowlevel's caller, we
   also need to restore its frame pointer.  */
__asm__ ("\n\
	.globl _signal_dispatch_entry_self\n\
_signal_dispatch_entry_self:\n\
	mov 0(%ebp), %ebp\n\
	mov 4(%esp), %esp\n\
	jmp _signal_dispatch_entry\n");

void
signal_dispatch_lowlevel (struct signal_state *ss, pthread_t tid,
			  siginfo_t si)
{
  assert (pthread_mutex_trylock (&ss->lock) == EBUSY);

  struct __pthread *thread = __pthread_getid (tid);

  bool self = tid == pthread_self ();

  uintptr_t intr_sp;

  if (self)
    {
      /* The return address is just before the first argument.  */
      intr_sp = (uintptr_t) &ss - 4;
      assert (* (void **) intr_sp == __builtin_return_address (0));
    }
  else
    {
      struct hurd_thread_exregs_in in;
      memset (&in, 0, sizeof (in));
      struct hurd_thread_exregs_out out;

      error_t err;
      err = rm_thread_exregs (ADDR_VOID, thread->object,
			      HURD_EXREGS_STOP | HURD_EXREGS_ABORT_IPC
			      | HURD_EXREGS_GET_REGS,
			      in, &out);
      if (err)
	panic ("Failed to modify thread " ADDR_FMT,
	       ADDR_PRINTF (thread->object));

      intr_sp = out.sp;

      /* Push the ip on the user stack.  */
      intr_sp -= 4;
      * (uintptr_t *) intr_sp = out.ip;
    }

  bool altstack = false;
  uintptr_t sp;
  if (! (ss->actions[si.si_signo - 1].sa_flags & SA_ONSTACK)
      || (ss->stack.ss_flags & SS_DISABLE)
      || (ss->stack.ss_flags & SS_ONSTACK))
    {
      assert (! self);
      sp = intr_sp;
    }
  else
    {
      /* The stack grows down.  */
      sp = (uintptr_t) ss->stack.ss_sp + ss->stack.ss_size;

      /* We know intimately that SS_ONSTACK is the least significant
	 bit.  */
      assert (SS_ONSTACK == 1);
      atomic_bit_set (&ss->stack.ss_flags, 0);

      altstack = true;
    }

  /* Set up the call frame for a call to signal_dispatch_entry.  */

  /* Allocate a siginfo structure on the stack.  */
  sp = sp - sizeof (siginfo_t);
  siginfo_t *sip = (void *) sp;
  /* Copy the user supplied values.  */
  *sip = si;

  /* Add space for the 4 caller saved registers.  */
  sp -= 4 * sizeof (uintptr_t);

  /* Save the interrupted sp.  */
  sp -= 4;
  * (uintptr_t *) sp = intr_sp;

  /* Address of the ss_flags.  */
  sp -= 4;
  if (altstack)
    * (uintptr_t *) sp = (uintptr_t) &ss->stack.ss_flags;
  else
    * (uintptr_t *) sp = 0;

  /* Push the parameters to signal_dispatch.  */

  /* signal info structure.  */
  sp -= 4;
  * (uintptr_t *) sp = (uintptr_t) sip;

  /* The ss.  */
  sp -= 4;
  * (uintptr_t *) sp = (uintptr_t) ss;

  pthread_mutex_transfer_np (&ss->lock, tid);

  if (self)
    ((void (*) (uintptr_t)) &_signal_dispatch_entry_self) ((uintptr_t) sp);
  else
    {
      struct hurd_thread_exregs_in in;
      struct hurd_thread_exregs_out out;

      in.sp = sp;
      in.ip = (uintptr_t) &_signal_dispatch_entry;

      rm_thread_exregs (ADDR_VOID, thread->object,
			HURD_EXREGS_SET_SP_IP
			| HURD_EXREGS_START | HURD_EXREGS_ABORT_IPC,
			in, &out);
    }
}
