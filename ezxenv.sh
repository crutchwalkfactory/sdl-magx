#!/bin/bash

export TOOLPREFIX=/arm-eabi
export QTDIR=/arm-eabi/lib/qt-ve66
export EZXDIR=/arm-eabi/lib/ezx-ve66
#export QTDIR=/arm-eabi/lib/qt-zn5
#export EZXDIR=/arm-eabi/lib/ezx-zn5
#export QTDIR=/arm-eabi/lib/qt-e8
#export EZXDIR=/arm-eabi/lib/ezx-e8
#export QTDIR=/arm-eabi/lib/qt-em30
#export EZXDIR=/arm-eabi/lib/ezx-em30
export PATH=$TOOLPREFIX/bin:$TOOLPREFIX/arm-linux-gnueabi/bin:$PATH

export ARCH=iwmmxt 
export CC=arm-linux-gnu-gcc 
export CXX=arm-linux-gnu-g++ 
export LD=arm-linux-gnu-ld 
export AR=arm-linux-gnu-ar 
export AS=arm-linux-gnu-as 
export OC=arm-linux-gnu-objcopy 
export RANLIB=arm-linux-gnu-ranlib 
export STRIP=arm-linux-gnu-strip 

export MOC=$QTDIR/bin/moc
