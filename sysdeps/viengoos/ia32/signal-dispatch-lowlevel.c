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

#include <viengoos/thread.h>
#include <pthread.h>
#include <stdint.h>
#include <atomic.h>
#include <string.h>

#ifdef __x86_64
# define WS "8"
#else
# define WS "4"
#endif

#ifdef __x86_64
# define R(reg) "%r" #reg
#else
# define R(reg) "%e" #reg
#endif

extern char _signal_dispatch_entry;
/* Stack layout:

 x86 x86-64
  7   13    rflags
  6   12    dx
  5   11    cx
  4   10    ax
  3    9    interrupted context's sp - WS  (Note: *(intr_sp - WS) = intr_ip)
  2    8    &ss->stack.ss_flags (or NULL, if not on an alternative stack)
       7    r11  -.
       6    r10    \
       5    r9       x86-64
       4    r8       only
       3    rdi    /
       2    rsi  -'
  1    1    &si
  0    0    ss ._
               |\
                  entry sp

 */
__asm__ (".globl _signal_dispatch_entry\n\t"
	 "_signal_dispatch_entry:\n\t"

	 /* Save the callee saved registers that we use (when we
	    return, we need to fully restore the register state to its
	    interrupted context--we may have forced a remote thread
	    into this function).  */
#ifdef __x86_64
	 "mov %rsi, 2*"WS"(%rsp)\n\t"
	 "mov %rdi, 3*"WS"(%rsp)\n\t"
	 "mov %r8, 4*"WS"(%rsp)\n\t"
	 "mov %r9, 5*"WS"(%rsp)\n\t"
	 "mov %r10, 6*"WS"(%rsp)\n\t"
	 "mov %r11, 7*"WS"(%rsp)\n\t"
	 /* Skip 8 and 9.  */
	 "mov "R(ax)", 10*"WS"("R(sp)")\n\t"
	 "mov "R(cx)", 11*"WS"("R(sp)")\n\t"
	 "mov "R(dx)", 12*"WS"("R(sp)")\n\t"
	 "pushf\n\t"
	 "pop %rcx\n\t"
	 "mov %rcx, 13*"WS"("R(sp)")\n\t"
#else
	 "mov "R(ax)", 4*"WS"("R(sp)")\n\t"
	 "mov "R(cx)", 5*"WS"("R(sp)")\n\t"
	 "mov "R(dx)", 6*"WS"("R(sp)")\n\t"
	 "pushf\n\t"
	 "pop %ecx\n\t"
	 "mov %ecx, 7*"WS"("R(sp)")\n\t"
#endif

	 /* On x86-64, the first two arguments are passed in
	    registers.  On x86, on the stack.  */
#ifdef __x86_64
	 "pop %rdi\n\t"
	 "pop %rsi\n\t"
#endif

	 "cld\n\t"
	 "call signal_dispatch\n\t"

#ifndef __x86_64
	 /* Fix up the stack.  */
	 "add $(2*"WS"), %esp"
#endif

	 /* Restore the registers in the first save area.  */
#ifdef __x86_64
	 "pop %rsi\n\t"
	 "pop %rdi\n\t"
	 "pop %r8\n\t"
	 "pop %r9\n\t"
	 "pop %r10\n\t"
	 "pop %r11\n\t"
#endif

	 /* Set cx to the pointer to the sigaltstack flags.  */
	 "pop "R(cx)"\n\t"
	 /* Set dx to the interrupted context's sp.  */
	 "pop "R(dx)"\n\t"

	 "and "R(cx)", "R(cx)"\n\t"
	 "jz after_move\n\t"

	 /* We need to move the remaining state to the interrupted
	    stack.  */

	 /* Make some space on that stack (which may be this stack!).  */
	 "sub $(4*"WS"), "R(dx)"\n\t"

	 /* Move the saved registers to the user stack.  */
	 /* ax.  */
	 "pop 0*"WS"("R(dx)")\n\t"
	 /* cx.  */
	 "pop 1*"WS"("R(dx)")\n\t"
	 /* dx.  */
	 "pop 2*"WS"("R(dx)")\n\t"
	 /* Flags.  */
	 "pop 3*"WS"("R(dx)")\n\t"


	 /* Restore the interrupted context's sp - 5 * WS (the four
	    saved registers above and the interrupted context's
	    IP).  */
	 "mov "R(dx)", "R(sp)"\n\t"

	 /* Clear the SA_ONSTACK flag.  */
	 "lock; and $~1, 0("R(cx)")\n\t"

	 "after_move:\n\t"
	 /* Restore the flag register, the scratch regs and the
	    original sp and ip.  */
	 "pop "R(ax)"\n\t"
	 "pop "R(cx)"\n\t"
	 "pop "R(dx)"\n\t"
	 "popf\n\t"
	 "ret\n\t");

extern char _signal_dispatch_entry_self;
/* x86-64:

     - 0(%rsp) is the return address
     - %rdi is the sp to load

   ia32:

     - 0(%esp) is the return address
     - 4(%esp) is the sp to load

   We need to move the return IP to *(original sp).  */
__asm__ (".globl _signal_dispatch_entry_self\n\t"
	 "_signal_dispatch_entry_self:\n\t"

#ifdef __x86_64
	 /* Save the current SP.  */
	 "mov %rsp, 9*"WS"(%rdi)\n\t"
	 /* sp = new sp.  */
	 "mov %rdi, %rsp\n\t"
#else
	 /* eax, ecx and edx are caller saved.  */

	 /* Save the current SP.  */
	 "mov 4(%esp), %eax\n\t"
	 "mov %esp, 3*"WS"(%eax)\n\t"

	 /* sp = new sp.  */
	 "mov %eax, %esp\n\t"
#endif

	 "jmp _signal_dispatch_entry\n\t");


void
signal_dispatch_lowlevel (struct signal_state *ss, pthread_t tid,
			  siginfo_t si)
{
  assert (pthread_mutex_trylock (&ss->lock) == EBUSY);

  struct __pthread *thread = __pthread_getid (tid);

  bool self = tid == pthread_self ();

  bool altstack = (ss->actions[si.si_signo - 1].sa_flags & SA_ONSTACK)
    && !(ss->stack.ss_flags & SS_DISABLE)
    && !(ss->stack.ss_flags & SS_ONSTACK);

  if (self && ! altstack)
    return signal_dispatch (ss, &si);

  uintptr_t intr_sp = 0;
  uintptr_t intr_ip = 0;
  if (! self)
    /* Suspend the thread and get its current stack.  */
    {
      struct vg_thread_exregs_in in;
      memset (&in, 0, sizeof (in));
      struct vg_thread_exregs_out out;

      error_t err;
      err = vg_thread_exregs (VG_ADDR_VOID, thread->object,
			      VG_EXREGS_STOP
			      | VG_EXREGS_GET_REGS,
			      in, VG_ADDR_VOID, VG_ADDR_VOID,
			      VG_ADDR_VOID, VG_ADDR_VOID,
			      &out, NULL, NULL, NULL, NULL);
      if (err)
	panic ("Failed to modify thread " VG_ADDR_FMT,
	       VG_ADDR_PRINTF (thread->object));

      intr_sp = out.sp;
      intr_ip = out.ip;
    }

  uintptr_t *sp;
  if (altstack)
    {
      /* The stack grows down.  */
      sp = (uintptr_t *) ((void *) ss->stack.ss_sp + ss->stack.ss_size);

      /* We know intimately that SS_ONSTACK is the least significant
	 bit.  */
      build_assert (SS_ONSTACK == 1);
      atomic_bit_set (&ss->stack.ss_flags, 0);
    }
  else
    {
      assert (! self);

      sp = (uintptr_t *) intr_sp;
      /* Allocate space for the interrupted context's IP.  */
      sp --;
    }

  /* Set up the call frame for a call to signal_dispatch_entry.  */

  /* Allocate save area 1.  */
  sp -= 4;

  /* The interrupted context's sp - WS.  (In the case where INTR_SP is
     NULL, i.e., when we are dealing with a self signal on an
     alternate stack, we will fix it signal_dispatch_entry_self.)  */
  * -- sp = intr_sp - sizeof (uintptr_t);

  /* The address of the ss_flags.  */
  if (altstack)
    * -- sp = (uintptr_t) &ss->stack.ss_flags;
  else
    * -- sp = 0;

  /* Allocate save area 2.  */
#ifdef __x86_64
  sp -= 6;
#endif

  /* A pointer to the siginfo structure.  */
  * -- sp = (uintptr_t) &si;

  /* The ss.  */
  * -- sp = (uintptr_t) ss;

  pthread_mutex_transfer_np (&ss->lock, tid);

  if (self)
    ((void (*) (uintptr_t *)) &_signal_dispatch_entry_self) (sp);
  else
    {
      /* Set INTR_SP[-1] to the interrupted context's ip.  */
      ((uintptr_t *) intr_sp)[-1] = intr_ip;

      struct vg_thread_exregs_in in;
      struct vg_thread_exregs_out out;

      in.sp = (uintptr_t) sp;
      in.ip = (uintptr_t) &_signal_dispatch_entry;

      vg_thread_exregs (VG_ADDR_VOID, thread->object,
			VG_EXREGS_SET_SP_IP
			| VG_EXREGS_START,
			in, VG_ADDR_VOID, VG_ADDR_VOID,
			VG_ADDR_VOID, VG_ADDR_VOID,
			&out, NULL, NULL, NULL, NULL);
    }
}
