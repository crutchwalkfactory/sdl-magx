#! /bin/bash
source ezxenv.sh
./configure --prefix=/arm-eabi --host=arm-linux-gnueabi \
	--disable-cdrom --disable-alsa --disable-oss \
	--disable-esd --disable-pulseaudio --disable-arts \
	--disable-nas --disable-ipod --disable-nasm \
	--disable-video-fbcon --disable-video-directfb \
	--disable-video-x11 --disable-video-opengl \
	--enable-video --enable-video-motoezx --enable-audio-ezx
