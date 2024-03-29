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

	void setRotation(screenRotationT r) { rot=r; }

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
	void CargarTeclas();

	int keyUp(int pressed);
	int keyDown(int pressed);
	int keyLeft(int pressed);
	int keyRight(int pressed);

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
	void MouseAction(int dx, int dy);

	bool bMySpecial;
	QCopChannel *inCallChannel,*ounCallChannel;
	SDL_keysym keyLast;
	bool bLastMod;
	bool bControlCursor;
	int iCursorStep;
	int iMouseKeyState;
	
	screenRotationT rot;
};

#endif
