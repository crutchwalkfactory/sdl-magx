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

export QTDIR=/arm-eabi/lib/qt-zn5
export EZXDIR=/arm-eabi/lib/ezx-zn5
export MOD=ZN5_U9
MAKE
