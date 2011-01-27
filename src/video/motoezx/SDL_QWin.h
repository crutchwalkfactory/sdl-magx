/*
    SDL - Simple DirectMedia Layer
    Copyright (C) 1997-2006 Sam Lantinga

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

    Sam Lantinga
    slouken@libsdl.org
*/
#include "SDL_config.h"

#ifndef _SDL_QWin_h
#define _SDL_QWin_h

#include <stdio.h>

#include <qimage.h>
#include <qpixmap.h>
#include <qwidget.h>
#include <linux/fb.h>

#include <ZKbMainWidget.h>

#include "SDL_events.h"

extern "C" 
{
#include "../../events/SDL_events_c.h"
};

typedef enum { 
    SDL_QT_NO_ROTATION = 0, 
    SDL_QT_ROTATION_90, 
    SDL_QT_ROTATION_270 
} screenRotationT;

extern screenRotationT screenRotation;

class QCopChannel;

class SDL_ZWin : public ZKbMainWidget
{
Q_OBJECT
public:
	SDL_ZWin(const QSize& size);
	virtual ~SDL_ZWin();

	void resume();
	void suspend( int n=1 );

	const QPoint& mousePos() const { return my_mouse_pos; }
	void setMousePos(const QPoint& newpos);

	bool SetVideoMode(uint32_t width, uint32_t height, uint32_t in_dbpp);
	void * getFBBuf();
	void flipScreen();
	void uninitVideo();
	
	bool isOK() { return isOk; }

 public slots:
	void channel(const QCString &, const QByteArray &);

	void slotRaise();
	void slotReturnToIdle(int);
  
 protected:
	void closeEvent(QCloseEvent *e);
	bool eventFilter( QObject *, QEvent * );
    void focusInEvent( QFocusEvent * );
    void focusOutEvent( QFocusEvent * );

private:
	int keyUp();
	int keyDown();
	int keyLeft();
	int keyRight();

	//Omega
	#ifdef OMEGA_SUPPORT
	int bOmgParse;
	void omgScroll(QKeyEvent *e);
	#endif

	enum 
	{
		EZX_LEFT_BUTTON = 1,
		EZX_RIGHT_BUTTON = 2,
	};
	void QueueKey(QKeyEvent *e, int pressed);

	QPoint my_mouse_pos;
	bool my_special;
	QCopChannel *inCallChannel,*ounCallChannel;
	int my_suspended;
	SDL_keysym last;
	bool last_mod;

	bool isOk;
	bool FocusOut;
};

#endif
