#!/bin/bash

MAKE()
{
make clean
rm build/.libs/libSDL-1.2.so.0.11.3
make
$STRIP build/.libs/libSDL-1.2.so.0.11.3
cp build/.libs/libSDL-1.2.so.0.11.3 libSDL-1.2.so.0
7z a libSDL-1.2_$VER\_$MOD.7z  libSDL-1.2.so.0 
rm libSDL-1.2.so.0
}

source ezxenv.sh
export VER=C1

$MOC src/video/magx/SDL_magx_win.h -o src/video/magx/moc_SDL_magx_win.cc

export QTDIR=/arm-eabi/lib/qt-ve66
export EZXDIR=/arm-eabi/lib/ezx-ve66
export MOD=VE66_EM35
MAKE

export QTDIR=/arm-eabi/lib/qt-zn5
export EZXDIR=/arm-eabi/lib/ezx-zn5
export MOD=ZN5_U9
MAKE

export QTDIR=/arm-eabi/lib/qt-e8
export EZXDIR=/arm-eabi/lib/ezx-e8
export MOD=E8
MAKE

export QTDIR=/arm-eabi/lib/qt-em30
export EZXDIR=/arm-eabi/lib/ezx-em30
export MOD=EM30
MAKE

export QTDIR=/arm-eabi/lib/qt-e8
export EZXDIR=/arm-eabi/lib/ezx-va76
export MOD=VA76
MAKE

export QTDIR=/arm-eabi/lib/qt-z6w
export EZXDIR=/arm-eabi/lib/ezx-z6w
export MOD=Z6W
MAKE

export QTDIR=/arm-eabi/lib/qt-z6
export EZXDIR=/arm-eabi/lib/ezx-z6
export MOD=Z6
MAKE

export QTDIR=/arm-eabi/lib/qt-v8
export EZXDIR=/arm-eabi/lib/ezx-v8
export MOD=V8
MAKE
