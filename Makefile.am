# Makefile.am - Makefile template for libpthread.
# Copyright (C) 2003, 2008 Free Software Foundation, Inc.
#
# This file is part of the GNU Hurd.
# 
# The GNU Hurd is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
# 
# The GNU Hurd is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA

if ARCH_IA32
  arch=ia32
endif
if ARCH_POWERPC
  arch=powerpc
endif

# The source files are scattered over several directories.  Add 
# all these directories to the vpath.
SYSDEP_PATH = $(srcdir)/sysdeps/l4/hurd/${arch}	\
	 $(srcdir)/sysdeps/l4/${arch}		\
	 $(srcdir)/sysdeps/l4/hurd		\
	 $(srcdir)/sysdeps/l4			\
	 $(srcdir)/sysdeps/hurd			\
	 $(srcdir)/sysdeps/${arch}		\
	 $(srcdir)/sysdeps/generic		\
	 $(srcdir)/sysdeps/posix		\
	 $(srcdir)/pthread			\
	 $(srcdir)/signal			\
	 $(srcdir)/include
vpath %.c $(SYSDEP_PATH)

AM_CPPFLAGS = $(USER_CPPFLAGS) -I$(srcdir)/pthread \
  $(addprefix -I, $(SYSDEP_PATH)) -imacros $(srcdir)/include/libc-symbols.h
AM_CFLAGS = $(USER_CFLAGS)

# Sources.
SYSDEPS := lockfile.c

if ! ENABLE_TESTS
noinst_LIBRARIES = libpthread.a
endif

libpthread_a_SOURCES = pt-attr.c pt-attr-destroy.c pt-attr-getdetachstate.c \
	pt-attr-getguardsize.c pt-attr-getinheritsched.c		    \
	pt-attr-getschedparam.c pt-attr-getschedpolicy.c pt-attr-getscope.c \
	pt-attr-getstack.c pt-attr-getstackaddr.c pt-attr-getstacksize.c    \
	pt-attr-init.c pt-attr-setdetachstate.c pt-attr-setguardsize.c	    \
	pt-attr-setinheritsched.c pt-attr-setschedparam.c		    \
	pt-attr-setschedpolicy.c pt-attr-setscope.c pt-attr-setstack.c	    \
	pt-attr-setstackaddr.c pt-attr-setstacksize.c pt-attr.c		    \
	pt-barrier-destroy.c pt-barrier-init.c pt-barrier-wait.c	    \
	pt-barrier.c pt-barrierattr-destroy.c pt-barrierattr-init.c	    \
	pt-barrierattr-getpshared.c pt-barrierattr-setpshared.c		    \
	pt-destroy-specific.c pt-init-specific.c			    \
	pt-key-create.c pt-key-delete.c					    \
	pt-getspecific.c pt-setspecific.c				    \
	pt-once.c							    \
	pt-alloc.c							    \
	pt-create.c							    \
	pt-pool-np.c							    \
	pt-equal.c							    \
	pt-dealloc.c							    \
	pt-detach.c							    \
	pt-exit.c							    \
	pt-initialize.c							    \
	pt-join.c							    \
	pt-self.c							    \
	pt-sigmask.c							    \
	pt-spin-inlines.c						    \
	pt-cleanup.c							    \
	pt-setcancelstate.c						    \
	pt-setcanceltype.c						    \
	pt-testcancel.c							    \
	pt-cancel.c							    \
	pt-mutexattr.c							    \
	pt-mutexattr-destroy.c pt-mutexattr-init.c			    \
	pt-mutexattr-getprioceiling.c pt-mutexattr-getprotocol.c	    \
	pt-mutexattr-getpshared.c pt-mutexattr-gettype.c		    \
	pt-mutexattr-setprioceiling.c pt-mutexattr-setprotocol.c	    \
	pt-mutexattr-setpshared.c pt-mutexattr-settype.c		    \
	pt-mutex-init.c pt-mutex-destroy.c				    \
	pt-mutex-lock.c pt-mutex-trylock.c pt-mutex-timedlock.c		    \
	pt-mutex-unlock.c						    \
	pt-mutex-transfer-np.c						    \
	pt-mutex-getprioceiling.c pt-mutex-setprioceiling.c		    \
	pt-rwlock-attr.c						    \
	pt-rwlockattr-init.c pt-rwlockattr-destroy.c			    \
	pt-rwlockattr-getpshared.c pt-rwlockattr-setpshared.c		    \
	pt-rwlock-init.c pt-rwlock-destroy.c				    \
	pt-rwlock-rdlock.c pt-rwlock-tryrdlock.c			    \
	pt-rwlock-trywrlock.c pt-rwlock-wrlock.c			    \
	pt-rwlock-timedrdlock.c pt-rwlock-timedwrlock.c			    \
	pt-rwlock-unlock.c						    \
	pt-cond.c							    \
	pt-condattr-init.c pt-condattr-destroy.c			    \
	pt-condattr-getclock.c pt-condattr-getpshared.c			    \
	pt-condattr-setclock.c pt-condattr-setpshared.c			    \
	pt-cond-destroy.c pt-cond-init.c				    \
	pt-cond-brdcast.c						    \
	pt-cond-signal.c						    \
	pt-cond-wait.c							    \
	pt-cond-timedwait.c						    \
	pt-stack-alloc.c						    \
	pt-thread-alloc.c						    \
	pt-thread-dealloc.c						    \
	pt-thread-start.c						    \
	pt-thread-halt.c						    \
	pt-startup.c							    \
	pt-getconcurrency.c pt-setconcurrency.c				    \
	pt-block.c							    \
	pt-timedblock.c							    \
	pt-wakeup.c							    \
	pt-docancel.c							    \
	pt-sysdep.c							    \
	pt-setup.c							    \
	pt-machdep.c							    \
	pt-spin.c							    \
	pt-sigstate-init.c						    \
	pt-sigstate-destroy.c						    \
	pt-sigstate.c							    \
	pt-atfork.c							    \
	pt-kill.c							    \
	pt-getcpuclockid.c						    \
	pt-getschedparam.c pt-setschedparam.c pt-setschedprio.c		    \
	sem-close.c sem-init.c sem-timedwait.c sem-wait.c		    \
	sem-destroy.c sem-open.c sem-trywait.c sem-getvalue.c		    \
	sem-post.c sem-unlink.c						    \
									    \
	pt-setactivity-np.c						    \
									    \
	kill.c								    \
	killpg.c							    \
	pt-kill-siginfo-np.c						    \
	raise.c								    \
	sigaction.c							    \
	sigaddset.c							    \
	sigaltstack.c							    \
	sigdelset.c							    \
	sigemptyset.c							    \
	sigfillset.c							    \
	sig-internal.c							    \
	sig-internal.h							    \
	siginterrupt.c							    \
	sigismember.c							    \
	signal.c							    \
	signal-dispatch.c						    \
	signal.h							    \
	sigpending.c							    \
	sigprocmask.c							    \
	sigsuspend.c							    \
	sigtimedwait.c							    \
	sigwait.c							    \
	sigwaiter.c							    \
	sigwaitinfo.c							    \
	signal-dispatch-lowlevel.c					    \
	sigprocmask.c
