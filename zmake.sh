#!/bin/bash
source ezxenv.sh

$MOC src/video/motoezx/SDL_QWin.h -o src/video/motoezx/mocSDL_QWin.cc

make
$STRIP build/.libs/libSDL-1.2.so.0.11.3

ncftpput -u root -p "" 192.168.16.2 /mmc/mmca1 build/.libs/libSDL-1.2.so.0.11.3
