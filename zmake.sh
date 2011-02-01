#!/bin/bash
source ezxenv.sh

$MOC src/video/magx/SDL_magx_win.h -o src/video/magx/moc_SDL_magx_win.cc

make
$STRIP build/.libs/libSDL-1.2.so.0.11.3

ncftpput -u root -p "" 192.168.16.2 /mmc/mmca1 build/.libs/libSDL-1.2.so.0.11.3
