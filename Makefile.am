# Makefile.am - Makefile template for libpthread.
#
# Copyright (C) 1994, 1995, 1996, 1997, 2000, 2002, 2003, 2004, 2005, 2006,
#   2007, 2008, 2009, 2011 Free Software Foundation, Inc.
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

# For as yet unknown reasons, in presence of libtool usage and target-specific
# flags (lib*_la_*FLAGS), Automake no longer uses `$<' for accessing the source
# files, thus losing the ability to use VPATH.  Restore the original behavior.
# TODO: talk to libtool people.
Makefile: Makefile.stamp
Makefile.stamp: Makefile.pre
	sed \
	  < $< > Makefile.new \
	  -e '/^#*	$$(LIBTOOL)/s%`test -f.*%$$<%'
	mv Makefile.new Makefile
	echo timestamp > $@

if ARCH_IA32
  arch=ia32
endif
if ARCH_POWERPC
  arch=powerpc
endif

if OS_GNU
  microkernel=mach
endif
if OS_L4
  microkernel=l4
endif

# The source files are scattered over several directories.  Add 
# all these directories to the VPATH.
SYSDEP_PATH = $(srcdir)/sysdeps/$(microkernel)/hurd/$(arch) \
	 $(srcdir)/sysdeps/$(microkernel)/$(arch) \
	 $(srcdir)/sysdeps/$(arch) \
	 $(srcdir)/sysdeps/$(microkernel)/hurd \
	 $(srcdir)/sysdeps/$(microkernel) \
	 $(srcdir)/sysdeps/hurd			\
	 $(srcdir)/sysdeps/generic		\
	 $(srcdir)/sysdeps/posix		\
	 $(srcdir)/pthread
if OS_L4
SYSDEP_PATH += \
	 $(srcdir)/signal
endif
SYSDEP_PATH += \
	 $(srcdir)/include

VPATH += $(SYSDEP_PATH)

if OS_GNU
libpthread_la_CPPFLAGS = $(CPPFLAGS) \
  $(addprefix -I, $(SYSDEP_PATH)) \
  -imacros config.h \
  -imacros $(srcdir)/include/libc-symbols.h \
  -imacros $(srcdir)/not-in-libc.h \
  -D_IO_MTSAFE_IO \
  -DENABLE_TLS
libpthread_la_CFLAGS = \
  -Wall \
  -std=gnu99
libpthread_la_LDFLAGS = \
  -version-info 0:0:0 \
  -lihash
endif
if OS_L4
AM_CPPFLAGS = $(USER_CPPFLAGS) -I$(srcdir)/pthread \
  $(addprefix -I, $(SYSDEP_PATH)) -imacros $(srcdir)/include/libc-symbols.h
AM_CFLAGS = $(USER_CFLAGS)
endif

# Sources.

if OS_GNU
lib_LTLIBRARIES = libpthread.la
libpthread_la_SOURCES = $(SRCS)
else
if OS_L4
if ! ENABLE_TESTS
noinst_LIBRARIES = libpthread.a
endif
libpthread_a_SOURCES = $(SRCS)
endif
endif

SRCS := pt-attr.c pt-attr-destroy.c pt-attr-getdetachstate.c		    \
	pt-attr-getguardsize.c pt-attr-getinheritsched.c		    \
	pt-attr-getschedparam.c pt-attr-getschedpolicy.c pt-attr-getscope.c \
	pt-attr-getstack.c pt-attr-getstackaddr.c pt-attr-getstacksize.c    \
	pt-attr-init.c pt-attr-setdetachstate.c pt-attr-setguardsize.c	    \
	pt-attr-setinheritsched.c pt-attr-setschedparam.c		    \
	pt-attr-setschedpolicy.c pt-attr-setscope.c pt-attr-setstack.c	    \
	pt-attr-setstackaddr.c pt-attr-setstacksize.c			    \
									    \
	pt-barrier-destroy.c pt-barrier-init.c pt-barrier-wait.c	    \
	pt-barrier.c pt-barrierattr-destroy.c pt-barrierattr-init.c	    \
	pt-barrierattr-getpshared.c pt-barrierattr-setpshared.c		    \
									    \
	pt-destroy-specific.c pt-init-specific.c			    \
	pt-key-create.c pt-key-delete.c					    \
	pt-getspecific.c pt-setspecific.c				    \
									    \
	pt-once.c							    \
									    \
	pt-alloc.c							    \
	pt-create.c							    \
	pt-getattr.c							    \
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
									    \
	pt-mutexattr.c							    \
	pt-mutexattr-destroy.c pt-mutexattr-init.c			    \
	pt-mutexattr-getprioceiling.c pt-mutexattr-getprotocol.c	    \
	pt-mutexattr-getpshared.c pt-mutexattr-gettype.c		    \
	pt-mutexattr-setprioceiling.c pt-mutexattr-setprotocol.c	    \
	pt-mutexattr-setpshared.c pt-mutexattr-settype.c		    \
									    \
	pt-mutex-init.c pt-mutex-destroy.c				    \
	pt-mutex-lock.c pt-mutex-trylock.c pt-mutex-timedlock.c		    \
	pt-mutex-unlock.c						    \
	pt-mutex-transfer-np.c						    \
	pt-mutex-getprioceiling.c pt-mutex-setprioceiling.c		    \
									    \
	pt-rwlock-attr.c						    \
	pt-rwlockattr-init.c pt-rwlockattr-destroy.c			    \
	pt-rwlockattr-getpshared.c pt-rwlockattr-setpshared.c		    \
									    \
	pt-rwlock-init.c pt-rwlock-destroy.c				    \
	pt-rwlock-rdlock.c pt-rwlock-tryrdlock.c			    \
	pt-rwlock-trywrlock.c pt-rwlock-wrlock.c			    \
	pt-rwlock-timedrdlock.c pt-rwlock-timedwrlock.c			    \
	pt-rwlock-unlock.c						    \
									    \
	pt-cond.c							    \
	pt-condattr-init.c pt-condattr-destroy.c			    \
	pt-condattr-getclock.c pt-condattr-getpshared.c			    \
	pt-condattr-setclock.c pt-condattr-setpshared.c			    \
									    \
	pt-cond-destroy.c pt-cond-init.c				    \
	pt-cond-brdcast.c						    \
	pt-cond-signal.c						    \
	pt-cond-wait.c							    \
	pt-cond-timedwait.c						    \
									    \
	pt-stack-alloc.c						    \
	pt-thread-alloc.c						    \
	pt-thread-dealloc.c						    \
	pt-thread-start.c						    \
	pt-thread-halt.c						    \
	pt-startup.c							    \
									    \
	pt-getconcurrency.c pt-setconcurrency.c				    \
									    \
	pt-block.c							    \
	pt-timedblock.c							    \
	pt-wakeup.c							    \
	pt-docancel.c							    \
	pt-sysdep.c							    \
	pt-setup.c							    \
	pt-machdep.c							    \
	pt-spin.c							    \
									    \
	pt-sigstate-init.c						    \
	pt-sigstate-destroy.c						    \
	pt-sigstate.c							    \
									    \
	pt-atfork.c							    \
	pt-kill.c							    \
	pt-getcpuclockid.c						    \
									    \
	pt-getschedparam.c pt-setschedparam.c pt-setschedprio.c		    \
	pt-yield.c							    \
									    \
	sem-close.c sem-destroy.c sem-getvalue.c sem-init.c sem-open.c	    \
	sem-post.c sem-timedwait.c sem-trywait.c sem-unlink.c		    \
	sem-wait.c

if OS_GNU
SRCS += cthreads-compat.c						    \
									    \
	lockfile.c
endif
if OS_L4
SRCS += pt-pool-np.c							    \
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
endif

HEADERS_INSTALL :=				\
              pthread.h				\
              pthread/pthread.h			\
              pthread/pthreadtypes.h		\
	      semaphore.h			\
						\
              bits/pthread.h			\
              bits/pthread-np.h			\
              bits/mutex.h			\
              bits/condition.h			\
              bits/condition-attr.h		\
              bits/spin-lock.h			\
              bits/spin-lock-inline.h		\
              bits/cancelation.h		\
              bits/thread-attr.h		\
              bits/barrier-attr.h		\
              bits/barrier.h			\
              bits/thread-specific.h		\
              bits/once.h			\
              bits/mutex-attr.h			\
              bits/rwlock.h			\
              bits/rwlock-attr.h		\
	      bits/semaphore.h

if OS_GNU
.PHONY: install-data-local
install-data-local: install-data-local-headers \
  instal-data-local-fix-libpthread_a \
  instal-data-local-add-libpthread_so_symlink

.PHONY: install-data-local-headers
install-data-local-headers: Makefile
	for f in $(HEADERS_INSTALL); do \
	  header_ok=n; \
	  for d in $(SYSDEP_PATH); do \
	    if test -f "$$d/$$f"; then \
	      fdir=$(DESTDIR)$(includedir)/$$(dirname "$$f") && \
	      { test -d $$fdir/ \
	        || $(MKDIR_P) "$$fdir"; } && \
	      $(INSTALL_DATA) \
	        "$$d/$$f" \
	        "$(DESTDIR)$(includedir)/$$f" && \
	      header_ok=y && \
	      break; \
	    fi; \
	  done; \
	  if [ "$$header_ok" != y ]; then \
	    echo '*** The header file `'"$$f"\'' is missing.' && \
	    echo '*** Please report this to <$(PACKAGE_BUGREPORT)>.' && \
	    exit 1; \
	  fi; \
	done

# TODO: it is not guaranteed that `install-data' will always be done after
# `install-exec'.  And, we're not allowed to make `install-data-local' depend
# on `install-exec'.  TODO: what is the libtool way of doing this?
.PHONY: instal-data-local-fix-libpthread_a
instal-data-local-fix-libpthread_a:
	if test -f $(DESTDIR)$(libdir)/libpthread.a; then \
	  rm -f $(DESTDIR)$(libdir)/libpthread2.a && \
	  mv \
	    $(DESTDIR)$(libdir)/libpthread.a \
	    $(DESTDIR)$(libdir)/libpthread2.a; \
	fi
	$(INSTALL_DATA) \
	  $(srcdir)/libpthread.a \
	  $(DESTDIR)$(libdir)/libpthread.a

# TODO: once in a time, when everything has been re-linked...
.PHONY: instal-data-local-add-libpthread_so_symlink
instal-data-local-add-libpthread_so_symlink:
	rm -f $(DESTDIR)$(libdir)/libpthread.so.0.3
	t=$(shell readlink $(DESTDIR)$(libdir)/libpthread.so) && \
	$(LN_S) "$$t" $(DESTDIR)$(libdir)/libpthread.so.0.3
endif
