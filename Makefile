# Makefile to build and install the SDL library

top_builddir = .
srcdir  = .
objects = build
depend	= build-deps
prefix = $(TOOLPREFIX)
exec_prefix = ${prefix}
bindir	= ${exec_prefix}/bin
libdir  = ${exec_prefix}/lib
includedir = ${prefix}/include
datarootdir = ${prefix}/share
datadir	= ${datarootdir}
mandir	= ${datarootdir}/man
auxdir	= build-scripts
distpath = $(srcdir)/..
distdir = SDL-1.2.14
distfile = $(distdir).tar.gz


SHELL	= /bin/bash
CC      = arm-linux-gnu-gcc
INCLUDE = -I./include
CFLAGS  = -g -O3 
EXTRA_CFLAGS = -I./include -D_GNU_SOURCE=1 -nostdinc -I$(TOOLPREFIX)/include -I$(TOOLPREFIX)/arm-linux-gnueabi/include -I$(TOOLPREFIX)/lib/gcc/arm-linux-gnueabi/3.4.3/include -DQWS -I$(QTDIR)/include -I$(EZXDIR)/include -DNO_DEBUG -fno-rtti -fno-exceptions -march=armv6j -mtune=arm1136jf-s -mfpu=vfp -nostdinc -I$(TOOLPREFIX)/include -I$(TOOLPREFIX)/arm-linux-gnueabi/include -I$(TOOLPREFIX)/lib/gcc/arm-linux-gnueabi/3.4.3/include -DQWS -I$(QTDIR)/include -I$(EZXDIR)/include -DNO_DEBUG -fno-rtti -fno-exceptions -D_REENTRANT -DHAVE_LINUX_VERSION_H
LDFLAGS = 
EXTRA_LDFLAGS =  -lm -ldl -L$(QTDIR)/lib -L$(EZXDIR)/lib -ldl -lpthread -lezxappbase -lqte-mt -L$(QTDIR)/lib -lezxsound -lpthread
LIBTOOL = $(SHELL) $(top_builddir)/libtool
INSTALL = /usr/bin/install -c
NASM	=  
AR	= arm-linux-gnu-ar
RANLIB	= arm-linux-gnu-ranlib
WINDRES	= 

TARGET  = libSDL.la
SOURCES =  ./src/*.c ./src/audio/*.c ./src/cdrom/*.c ./src/cpuinfo/*.c ./src/events/*.c ./src/file/*.c ./src/stdlib/*.c ./src/thread/*.c ./src/timer/*.c ./src/video/*.c ./src/joystick/*.c ./src/video/dummy/*.c ./src/audio/disk/*.c ./src/audio/dummy/*.c ./src/loadso/dlopen/*.c ./src/video/magx/*.cc ./src/audio/magx/*.cc ./src/thread/pthread/SDL_systhread.c ./src/thread/pthread/SDL_syssem.c ./src/thread/pthread/SDL_sysmutex.c ./src/thread/pthread/SDL_syscond.c ./src/joystick/linux/*.c ./src/timer/unix/*.c ./src/cdrom/dummy/*.c
OBJECTS = $(objects)/SDL.lo $(objects)/SDL_error.lo $(objects)/SDL_fatal.lo $(objects)/SDL_audio.lo $(objects)/SDL_audiocvt.lo $(objects)/SDL_audiodev.lo $(objects)/SDL_mixer.lo $(objects)/SDL_mixer_MMX.lo $(objects)/SDL_mixer_MMX_VC.lo $(objects)/SDL_mixer_m68k.lo $(objects)/SDL_wave.lo $(objects)/SDL_cdrom.lo $(objects)/SDL_cpuinfo.lo $(objects)/SDL_active.lo $(objects)/SDL_events.lo $(objects)/SDL_expose.lo $(objects)/SDL_keyboard.lo $(objects)/SDL_mouse.lo $(objects)/SDL_quit.lo $(objects)/SDL_resize.lo $(objects)/SDL_rwops.lo $(objects)/SDL_getenv.lo $(objects)/SDL_iconv.lo $(objects)/SDL_malloc.lo $(objects)/SDL_qsort.lo $(objects)/SDL_stdlib.lo $(objects)/SDL_string.lo $(objects)/SDL_thread.lo $(objects)/SDL_timer.lo $(objects)/SDL_RLEaccel.lo $(objects)/SDL_blit.lo $(objects)/SDL_blit_0.lo $(objects)/SDL_blit_1.lo $(objects)/SDL_blit_A.lo $(objects)/SDL_blit_N.lo $(objects)/SDL_bmp.lo $(objects)/SDL_cursor.lo $(objects)/SDL_gamma.lo $(objects)/SDL_pixels.lo $(objects)/SDL_stretch.lo $(objects)/SDL_surface.lo $(objects)/SDL_video.lo $(objects)/SDL_yuv.lo $(objects)/SDL_yuv_mmx.lo $(objects)/SDL_yuv_sw.lo $(objects)/SDL_joystick.lo $(objects)/SDL_nullevents.lo $(objects)/SDL_nullmouse.lo $(objects)/SDL_nullvideo.lo $(objects)/SDL_diskaudio.lo $(objects)/SDL_dummyaudio.lo $(objects)/SDL_sysloadso.lo $(objects)/SDL_magx_kernel.lo $(objects)/SDL_magx_sys.lo $(objects)/SDL_magx_video.lo $(objects)/SDL_magx_win.lo $(objects)/ipu_alloc.lo $(objects)/moc_SDL_magx_win.lo $(objects)/SDL_magxaudio.lo $(objects)/SDL_systhread.lo $(objects)/SDL_syssem.lo $(objects)/SDL_sysmutex.lo $(objects)/SDL_syscond.lo $(objects)/SDL_sysjoystick.lo $(objects)/SDL_systimer.lo $(objects)/SDL_syscdrom.lo

SDLMAIN_TARGET = libSDLmain.a
SDLMAIN_SOURCES = ./src/main/magx/*.cc
SDLMAIN_OBJECTS = $(objects)/SDL_motoezx_main.o

DIST = acinclude autogen.sh Borland.html Borland.zip BUGS build-scripts configure configure.in COPYING CREDITS CWprojects.sea.bin docs docs.html include INSTALL Makefile.dc Makefile.minimal Makefile.in MPWmake.sea.bin README* sdl-config.in sdl.m4 sdl.pc.in SDL.qpg.in SDL.spec SDL.spec.in src test TODO VisualCE.zip VisualC.html VisualC.zip Watcom-OS2.zip Watcom-Win32.zip symbian.zip WhatsNew Xcode.tar.gz

HDRS = SDL.h SDL_active.h SDL_audio.h SDL_byteorder.h SDL_cdrom.h SDL_cpuinfo.h SDL_endian.h SDL_error.h SDL_events.h SDL_getenv.h SDL_joystick.h SDL_keyboard.h SDL_keysym.h SDL_loadso.h SDL_main.h SDL_mouse.h SDL_mutex.h SDL_name.h SDL_opengl.h SDL_platform.h SDL_quit.h SDL_rwops.h SDL_stdinc.h SDL_syswm.h SDL_thread.h SDL_timer.h SDL_types.h SDL_version.h SDL_video.h begin_code.h close_code.h

LT_AGE      = 11
LT_CURRENT  = 11
LT_RELEASE  = 1.2
LT_REVISION = 3
LT_LDFLAGS  = -no-undefined -rpath $(DESTDIR)$(libdir) -release $(LT_RELEASE) -version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE)

all: $(srcdir)/configure Makefile $(objects) $(objects)/$(TARGET) $(objects)/$(SDLMAIN_TARGET)

$(srcdir)/configure: $(srcdir)/configure.in
	@echo "Warning, configure.in is out of date"
	#(cd $(srcdir) && sh autogen.sh && sh configure)
	@sleep 3

Makefile: $(srcdir)/Makefile.in
	$(SHELL) config.status $@

$(objects):
	$(SHELL) $(auxdir)/mkinstalldirs $@

.PHONY: all depend install install-bin install-hdrs install-lib install-data install-man uninstall uninstall-bin uninstall-hdrs uninstall-lib uninstall-data uninstall-man clean distclean dist
depend:
	@SOURCES="$(SOURCES)" INCLUDE="$(INCLUDE)" output="$(depend)" \
	$(SHELL) $(auxdir)/makedep.sh
	@for src in $(SDLMAIN_SOURCES); do \
	    obj=`echo $$src | sed -e 's|.*/||' -e 's|\.[^\.]*$$|.o|'`; \
	    echo "\$$(objects)/$$obj: $$src" >>$(depend); \
	    echo "	\$$(CC) \$$(CFLAGS) \$$(EXTRA_CFLAGS) -c $$src -o \$$@" >>$(depend); \
	done

include $(depend)

$(objects)/$(TARGET): $(OBJECTS)
	$(LIBTOOL) --mode=link $(CC) -o $@ $(OBJECTS) $(LDFLAGS) $(EXTRA_LDFLAGS) $(LT_LDFLAGS)

$(objects)/$(SDLMAIN_TARGET): $(SDLMAIN_OBJECTS)
	$(AR) cru $@ $(SDLMAIN_OBJECTS)
	$(RANLIB) $@

install: all install-bin install-hdrs install-lib install-data install-man
install-bin:
	$(SHELL) $(auxdir)/mkinstalldirs $(DESTDIR)$(bindir)
	$(INSTALL) -m 755 sdl-config $(DESTDIR)$(bindir)/sdl-config
install-hdrs:
	$(SHELL) $(auxdir)/mkinstalldirs $(DESTDIR)$(includedir)/SDL
	for file in $(HDRS); do \
	    $(INSTALL) -m 644 $(srcdir)/include/$$file $(DESTDIR)$(includedir)/SDL/$$file; \
	done
	$(INSTALL) -m 644 include/SDL_config.h $(DESTDIR)$(includedir)/SDL/SDL_config.h
install-lib: $(objects) $(objects)/$(TARGET) $(objects)/$(SDLMAIN_TARGET)
	$(SHELL) $(auxdir)/mkinstalldirs $(DESTDIR)$(libdir)
	$(LIBTOOL) --mode=install $(INSTALL) $(objects)/$(TARGET) $(DESTDIR)$(libdir)/$(TARGET)
	$(INSTALL) -m 644 $(objects)/$(SDLMAIN_TARGET) $(DESTDIR)$(libdir)/$(SDLMAIN_TARGET)
	$(RANLIB) $(DESTDIR)$(libdir)/$(SDLMAIN_TARGET)
install-data:
	$(SHELL) $(auxdir)/mkinstalldirs $(DESTDIR)$(datadir)/aclocal
	$(INSTALL) -m 644 $(srcdir)/sdl.m4 $(DESTDIR)$(datadir)/aclocal/sdl.m4
	$(SHELL) $(auxdir)/mkinstalldirs $(DESTDIR)$(libdir)/pkgconfig
	$(INSTALL) -m 644 sdl.pc $(DESTDIR)$(libdir)/pkgconfig
install-man:
	$(SHELL) $(auxdir)/mkinstalldirs $(DESTDIR)$(mandir)/man3
	for src in $(srcdir)/docs/man3/*.3; do \
	    file=`echo $$src | sed -e 's|^.*/||'`; \
	    $(INSTALL) -m 644 $$src $(DESTDIR)$(mandir)/man3/$$file; \
	done

uninstall: uninstall-bin uninstall-hdrs uninstall-lib uninstall-data uninstall-man
uninstall-bin:
	rm -f $(DESTDIR)$(bindir)/sdl-config
uninstall-hdrs:
	for file in $(HDRS); do \
	    rm -f $(DESTDIR)$(includedir)/SDL/$$file; \
	done
	rm -f $(DESTDIR)$(includedir)/SDL/SDL_config.h
	-rmdir $(DESTDIR)$(includedir)/SDL
uninstall-lib:
	$(LIBTOOL) --mode=uninstall rm -f $(DESTDIR)$(libdir)/$(TARGET)
	rm -f $(DESTDIR)$(libdir)/$(SDLMAIN_TARGET)
uninstall-data:
	rm -f $(DESTDIR)$(datadir)/aclocal/sdl.m4
	rm -f $(DESTDIR)$(libdir)/pkgconfig/sdl.pc
uninstall-man:
	for src in $(srcdir)/docs/man3/*.3; do \
	    file=`echo $$src | sed -e 's|^.*/||'`; \
	    rm -f $(DESTDIR)$(mandir)/man3/$$file; \
	done

clean:
	rm -rf $(objects)
	if test -f test/Makefile; then (cd test; $(MAKE) $@); fi

distclean: clean
	rm -f Makefile include/SDL_config.h sdl-config
	rm -f SDL.qpg
	rm -f config.status config.cache config.log libtool $(depend)
	rm -rf $(srcdir)/autom4te*
	rm -rf $(srcdir)/test/autom4te*
	find $(srcdir) \( \
	    -name '*~' -o \
	    -name '*.bak' -o \
	    -name '*.old' -o \
	    -name '*.rej' -o \
	    -name '*.orig' -o \
	    -name '.#*' \) \
	    -exec rm -f {} \;
	cp $(srcdir)/include/SDL_config.h.default $(srcdir)/include/SDL_config.h
	if test -f test/Makefile; then (cd test; $(MAKE) $@); fi

dist $(distfile):
	$(SHELL) $(auxdir)/mkinstalldirs $(distdir)
	tar cf - $(DIST) | (cd $(distdir); tar xf -)
	cp $(distdir)/include/SDL_config.h.default $(distdir)/include/SDL_config.h
	rm -rf `find $(distdir) -name .svn`
	rm -rf $(distdir)/test/autom4te*
	find $(distdir) \( \
	    -name '*~' -o \
	    -name '*.bak' -o \
	    -name '*.old' -o \
	    -name '*.rej' -o \
	    -name '*.orig' -o \
	    -name '.#*' \) \
	    -exec rm -f {} \;
	if test -f $(distdir)/test/Makefile; then (cd $(distdir)/test && make distclean); fi
	tar cvf - $(distdir) | gzip --best >$(distfile)
	rm -rf $(distdir)
