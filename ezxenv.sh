#!/bin/bash

export TOOLPREFIX=/arm-eabi
#export TOOLPREFIX=/arm-eabi
export QTDIR=/arm-eabi/lib/qt-ve66
export EZXDIR=/arm-eabi/lib/ezx-ve66
#export QTDIR=/arm-eabi/lib/qt-z6
#export EZXDIR=/arm-eabi/lib/ezx-z6
#export QTDIR=/arm-eabi/lib/qt-e8
#export EZXDIR=/arm-eabi/lib/ezx-va76
#export QTDIR=/arm-eabi/lib/qt-zn5
#export EZXDIR=/arm-eabi/lib/ezx-zn5
#export QTDIR=/arm-eabi/lib/qt-e8
#export EZXDIR=/arm-eabi/lib/ezx-e8
#export QTDIR=/arm-eabi/lib/qt-em30
#export EZXDIR=/arm-eabi/lib/ezx-em30
export PATH=$TOOLPREFIX/bin:$TOOLPREFIX/lib/SDL/bin:$TOOLPREFIX/arm-linux-gnueabi/bin:$PATH

export ARCH=iwmmxt 
export CC=arm-linux-gnueabi-gcc
export CXX=arm-linux-gnueabi-g++ 
export LD=arm-linux-gnueabi-ld 
export AR=arm-linux-gnueabi-ar 
export AS=arm-linux-gnueabi-as 
export OC=arm-linux-gnueabi-objcopy 
export RANLIB=arm-linux-gnueabi-ranlib 
export STRIP=arm-linux-gnueabi-strip 

export MOC=$QTDIR/bin/moc
