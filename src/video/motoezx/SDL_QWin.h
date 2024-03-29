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

extern "C" {
#include "../../events/SDL_events_c.h"
};

typedef enum { 
    SDL_QT_NO_ROTATION = 0, 
    SDL_QT_ROTATION_90, 
    SDL_QT_ROTATION_270 
} screenRotationT;

extern screenRotationT screenRotation;

class QCopChannel;
//class SDL_QWin : public QWidget
class SDL_QWin : public ZKbMainWidget
{
 Q_OBJECT
 private:
  enum {
    EZX_LEFT_BUTTON = 1,
    EZX_RIGHT_BUTTON = 2
  };
  void QueueKey(QKeyEvent *e, int pressed);
 public:
  SDL_QWin(const QSize& size);
  virtual ~SDL_QWin();
  virtual bool shown(void) {
    return isVisible();
  }
  /* If called, the next resize event will not be forwarded to SDL. */
  virtual void inhibitResize(void) {
    my_inhibit_resize = true;
  }
  void setImage(QImage *image);
  void setOffset(int x, int y) {
    my_offset = QPoint(x, y);
  }
  void GetXYOffset(int &x, int &y) {
    x = my_offset.x();
    y = my_offset.y();
  }
  QImage *image(void) { return my_image; }
  
  void setWFlags(WFlags flags) {
    QWidget::setWFlags(flags);
    my_flags = flags;
  }
  const QPoint& mousePos() const { return my_mouse_pos; }
  void setMousePos(const QPoint& newpos);

  void repaintRect(const QRect& rect);
  bool isOK()
  { return fbdev != -1 && vmem != (char *)-1; }
  
 public slots:
  void channel(const QCString &, const QByteArray &);
	void signalRaise();
	void signalAskReturnToIdle(int);
  //void clickAppIcon();
  
 protected:
  /* Handle resizing of the window */
  virtual void resizeEvent(QResizeEvent *e);
#ifdef MOTOEZX_TEST
  void focusInEvent(QFocusEvent *);
  void focusOutEvent(QFocusEvent *);
#endif
  void timerEvent(QTimerEvent *);
  void closeEvent(QCloseEvent *e);
  void mouseMoveEvent(QMouseEvent *e);
  void mousePressEvent(QMouseEvent *e);
  void mouseReleaseEvent(QMouseEvent *e);
  void paintEvent(QPaintEvent *ev);
  void keyPressEvent(QKeyEvent *e)   { QueueKey(e, 1); }
  void keyReleaseEvent(QKeyEvent *e) { QueueKey(e, 0); }
  //bool  eventFilter( QObject *, QEvent * );
 private:
  void init();
  void suspend();
  void resume();
  
  int keyUp();
  int keyDown();
  int keyLeft();
  int keyRight();

  int fbdev;
  char *vmem;
  size_t vmem_length;
  QImage *my_image;
  bool my_inhibit_resize;
  QPoint my_offset;
  QPoint my_mouse_pos;
  WFlags my_flags;
  unsigned int my_locked;
  int cur_mouse_button;
  bool my_special;
  QCopChannel *qcop;
  int my_timer;
  bool my_suspended;
  SDL_keysym last;
  bool last_mod;
};

#endif /* _SDL_QWin_h */
