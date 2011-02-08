#!/bin/bash

MAKE()
{
rm build/.libs/libSDL-1.2.so.0.11.3
make
$STRIP build/.libs/libSDL-1.2.so.0.11.3
ncftpput -u root -p "" 192.168.16.2 /mmc/mmca1 build/.libs/libSDL-1.2.so.0.11.3
}

source ezxenv.sh

makefrom=$1

$MOC src/video/magx/SDL_magx_win.h -o src/video/magx/moc_SDL_magx_win.cc

if ["$makefrom" == ""]
then
makefrom=ve66
fi

case "$makefrom" in
	ve66)
		export QTDIR=/arm-eabi/lib/qt-ve66
		export EZXDIR=/arm-eabi/lib/ezx-ve66
		export MOD=VE66_EM35
		MAKE
		;;
	zn5)
		export QTDIR=/arm-eabi/lib/qt-zn5
		export EZXDIR=/arm-eabi/lib/ezx-zn5
		export MOD=ZN5_U9
		MAKE
		;;
	e8)
		export QTDIR=/arm-eabi/lib/qt-e8
		export EZXDIR=/arm-eabi/lib/ezx-e8
		export MOD=E8
		MAKE
		;;
	em30)
		export QTDIR=/arm-eabi/lib/qt-em30
		export EZXDIR=/arm-eabi/lib/ezx-em30
		export MOD=EM30
		MAKE
		;;
	va76)
		export QTDIR=/arm-eabi/lib/qt-e8
		export EZXDIR=/arm-eabi/lib/ezx-va76
		export MOD=VA76
		MAKE
		;;
	z6w)
		export QTDIR=/arm-eabi/lib/qt-z6w
		export EZXDIR=/arm-eabi/lib/ezx-z6w
		export MOD=Z6W
		MAKE
		;;
	z6)
		export QTDIR=/arm-eabi/lib/qt-z6
		export EZXDIR=/arm-eabi/lib/ezx-z6
		export MOD=Z6
		MAKE
		;;
	v8)
		export QTDIR=/arm-eabi/lib/qt-v8
		export EZXDIR=/arm-eabi/lib/ezx-v8
		export MOD=V8
		MAKE
		;;
esac
