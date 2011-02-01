#ifndef SDL_MAGX_WIN
#define SDL_MAGX_WIN

#include <ZKbMainWidget.h>

#include "SDL_events.h"

#include "ipu.h"
typedef enum { 
    SDL_QT_NO_ROTATION = IPU_ROTATE_NONE, 
    SDL_QT_ROTATION_90 = IPU_ROTATE_90_RIGHT, 
    SDL_QT_ROTATION_270 = IPU_ROTATE_90_LEFT, 
} screenRotationT;

extern screenRotationT screenRotation;

class QCopChannel;

class SDL_MainWindow: public ZKbMainWidget
{
Q_OBJECT
public:
	SDL_MainWindow();
	virtual ~SDL_MainWindow();

	void resume();
	void suspend( int n=1 );

	const QPoint& mousePos() const { return my_mouse_pos; }
	void setMousePos(const QPoint& newpos);

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
	SDL_keysym last;
	bool last_mod;
};

#endif
