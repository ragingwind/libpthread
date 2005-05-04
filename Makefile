#
#   Copyright (C) 1994,95,96,97,2000,02, 2004, 2005 Free Software Foundation, Inc.
#
#   This program is free software; you can redistribute it and/or
#   modify it under the terms of the GNU General Public License as
#   published by the Free Software Foundation; either version 2, or (at
#   your option) any later version.
#
#   This program is distributed in the hope that it will be useful, but
#   WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#   General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program; if not, write to the Free Software
#   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

dir := libpthread
makemode := library

MICROKERNEL := mach
SYSDEPS := lockfile.c

SRCS := 

LCLHDRS := 

SRCS := pt-attr.c pt-attr-destroy.c pt-attr-getdetachstate.c		    \
	pt-attr-getguardsize.c pt-attr-getinheritsched.c		    \
	pt-attr-getschedparam.c pt-attr-getschedpolicy.c pt-attr-getscope.c \
	pt-attr-getstack.c pt-attr-getstackaddr.c pt-attr-getstacksize.c    \
	pt-attr-init.c pt-attr-setdetachstate.c pt-attr-setguardsize.c	    \
	pt-attr-setinheritsched.c pt-attr-setschedparam.c		    \
	pt-attr-setschedpolicy.c pt-attr-setscope.c pt-attr-setstack.c	    \
	pt-attr-setstackaddr.c pt-attr-setstacksize.c pt-attr.c		    \
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
									    \
	cthreads-compat.c						    \
	$(SYSDEPS)

OBJS = $(addsuffix .o,$(basename $(notdir $(SRCS))))

OTHERTAGS = 

libname = libpthread

sysdeps_headers =				\
              pthread.h				\
              pthread/pthread.h			\
						\
              bits/pthread.h			\
              bits/mutex.h			\
              bits/condition.h			\
              bits/condition-attr.h		\
              bits/spin-lock.h			\
              bits/cancelation.h		\
              bits/thread-attr.h		\
              bits/barrier-attr.h		\
              bits/barrier.h			\
              bits/thread-specific.h		\
              bits/once.h			\
              bits/mutex-attr.h			\
              bits/rwlock.h			\
              bits/rwlock-attr.h

SYSDEP_PATH = $(srcdir)/sysdeps/$(MICROKERNEL)/hurd/i386	\
	 $(srcdir)/sysdeps/$(MICROKERNEL)/i386			\
	 $(srcdir)/sysdeps/$(MICROKERNEL)/hurd			\
	 $(srcdir)/sysdeps/$(MICROKERNEL)			\
	 $(srcdir)/sysdeps/hurd					\
	 $(srcdir)/sysdeps/i386					\
	 $(srcdir)/sysdeps/generic				\
	 $(srcdir)/sysdeps/posix				\
	 $(srcdir)/pthread					\
	 $(srcdir)/include

VPATH += $(SYSDEP_PATH)

HURDLIBS = ihash

CFLAGS := -D_IO_MTSAFE_IO				\
	   $(addprefix -I, $(SYSDEP_PATH))		\
	  -imacros $(srcdir)/include/libc-symbols.h	\
	  -imacros $(srcdir)/not-in-libc.h

installhdrs :=
installhdrsubdir = .

include ../Makeconf

install: install-headers $(libdir)/libpthread2.a $(libdir)/libpthread2_pic.a
install-headers: $(addprefix $(includedir)/, $(sysdeps_headers))

# XXX: If $(libdir)/libpthread2.a is installed and
# $(libdir)/libpthread is not, we can have some issues.
.PHONY: $(libdir)/libpthread.a $(libdir)/libpthread_pic.a

# XXX: These rules are a hack.  But it is better than messing with
# ../Makeconf at the moment.  Note that the linker scripts
# $(srcdir)/libpthread.a and $(srcdir)/libpthread_pic.a get overwritten
# when building in $(srcdir) and not a seperate build directory.
$(libdir)/libpthread2.a: $(libdir)/libpthread.a
	mv $< $@
	$(INSTALL_DATA) $(srcdir)/libpthread.a $<

$(libdir)/libpthread2_pic.a: $(libdir)/libpthread_pic.a
	mv $< $@
	$(INSTALL_DATA) $(srcdir)/libpthread_pic.a $<

.PHONY: $(addprefix $(includedir)/, $(sysdeps_headers))

$(addprefix $(includedir)/, $(sysdeps_headers)):
	@set -e;							\
	t="$@";								\
	t=$${t#$(includedir)/};						\
	header_ok=0;							\
	for dir in $(SYSDEP_PATH);					\
	do								\
	  if test -e "$$dir/$$t";					\
	  then								\
	    tdir=`dirname "$@"`;					\
	    if test ! -e $$tdir;					\
	    then							\
	      mkdir $$tdir;						\
	    fi;								\
	    echo $(INSTALL_DATA) "$$dir/$$t" "$@";			\
	    $(INSTALL_DATA) "$$dir/$$t" "$@";				\
	    header_ok=1;						\
	    break;							\
	  fi;								\
	done;								\
	if test "$${header_ok}" -ne 1;					\
	then								\
	  echo;								\
	  echo '*** 'The header file \`$@\' is required, but not	\
provided, by;								\
	  echo '*** 'this configuration.  Please report this to the	\
maintainer.;								\
	  echo;								\
	  false;							\
	fi

#  ifeq ($(VERSIONING),yes)
#  
#  # Adding this dependency gets it included in the command line,
#  # where ld will read it as a linker script.
#  $(libname).so.$(hurd-version): $(srcdir)/$(libname).map
#  
#  endif
