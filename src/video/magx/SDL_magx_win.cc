#include "SDL_magx_win.h"

#include <ZApplication.h>
#include <ZKeyDef.h>
#include <qcopchannel_qws.h>
#include <qtimer.h>

#include "SDL_magx_kernel.h"
#ifdef MPH_MODE_INT_PHONE
#include "morphing_mode.h"
#endif

#if 0
#define DebugFunction() printf("MAGX_VO: win - %s()\n",__FUNCTION__) 
#else
#define DebugFunction()
#endif

extern "C" {
#include "../../events/SDL_events_c.h"
};

screenRotationT screenRotation = SDL_QT_NO_ROTATION;

#ifdef __cplusplus
extern "C" {
#endif
//For compobility
void SDL_ChannelExists(const char *){};
void SDL_ShowSplash(){};
void SDL_HideSplash(){};
//For suspend on call
extern int UTIL_GetIncomingCallStatus();
extern int UTIL_GetSideKeyLock();
//Syspend and focus flag
int my_suspended;
int my_focus;
#ifdef __cplusplus
}
#endif

static inline bool needSuspend()
{
  if(UTIL_GetIncomingCallStatus() || UTIL_GetSideKeyLock())
    return true;
  return false;
}

int myRED, myCENTER, myUP, myDOWN, myLEFT, myRIGHT, mySIDE, myMUSIC, myC, myLSOFT, myRSOFT, myCALL, myCAMERA, myVOLUP, myVOLDOWN, my0, my1, my2, my3, my4, my5, my6, my7, my8, my9, myASTERISK, myNUMERAL;
int SmyRED, SmyCENTER, SmyUP, SmyDOWN, SmyLEFT, SmyRIGHT, SmySIDE, SmyMUSIC, SmyC, SmyLSOFT, SmyRSOFT, SmyCALL, SmyCAMERA, SmyVOLUP, SmyVOLDOWN, Smy0, Smy1, Smy2, Smy3, Smy4, Smy5, Smy6, Smy7, Smy8, Smy9, SmyASTERISK, SmyNUMERAL;

void SDL_MainWindow::CargarTeclas()
{
	ZConfig mySDL(QString(qApp->argv()[0])+"_SDL.cfg", false);
	
	//Get default rotation
	int envValue;
	char * envString = SDL_getenv("SDL_QT_INVERT_ROTATION");
	if ( envString )
		envValue = atoi(envString);
	else
		envValue = mySDL.readNumEntry("SYSTEM", "Rotation", 1);
	screenRotation = envValue ? SDL_QT_ROTATION_90 : SDL_QT_ROTATION_270;
	bool bUseCursor = mySDL.readBoolEntry("MOUSE", "UseCursor", 0);
	if ( bUseCursor )
	{
		setCanUseCursor( bUseCursor );
		bControlCursor = mySDL.readBoolEntry("MOUSE", "ControlCursor", 0);
		iCursorStep = mySDL.readNumEntry("MOUSE", "CursorStep", 12);
	}
	
	ZConfig * keyCFG;
	if ( QFile::exists("/mmc/mmca1/games/lib/keyconfig.cfg") )
		keyCFG = new ZConfig("/mmc/mmca1/games/lib/keyconfig.cfg", false);
	else if ( QFile::exists("/ezxlocal/download/mystuff/games/lib/keyconfig.cfg") )
		keyCFG = new ZConfig("/ezxlocal/download/mystuff/games/lib/keyconfig.cfg", false);
	else
		keyCFG = new ZConfig("/usr/mlib/SDL/keyconfig.cfg", false);
	QString val;
	val = mySDL.readEntry("SDL", "Red", "ESCAPE");
	myRED = keyCFG->readNumEntry("KEYCODES", QString(val), SDLK_ESCAPE);
	val = mySDL.readEntry("SDL", "Center", "RETURN");
	myCENTER = keyCFG->readNumEntry("KEYCODES", QString(val), SDLK_RETURN);
	val = mySDL.readEntry("SDL", "Up", "UP");
	myUP = keyCFG->readNumEntry("KEYCODES", QString(val), SDLK_UP);
	val = mySDL.readEntry("SDL", "Down", "DOWN");
	myDOWN = keyCFG->readNumEntry("KEYCODES", QString(val), SDLK_DOWN);
	val = mySDL.readEntry("SDL", "Left", "LEFT");
	myLEFT = keyCFG->readNumEntry("KEYCODES", QString(val), SDLK_LEFT);
	val = mySDL.readEntry("SDL", "Right", "RIGHT");
	myRIGHT = keyCFG->readNumEntry("KEYCODES", QString(val), SDLK_RIGHT);
	val = mySDL.readEntry("SDL", "SideKey", "TAB");
	mySIDE = keyCFG->readNumEntry("KEYCODES", QString(val), SDLK_TAB);
	val = mySDL.readEntry("SDL", "Music", "F10");
	myMUSIC = keyCFG->readNumEntry("KEYCODES", QString(val), 291);
	val = mySDL.readEntry("SDL", "C", "BACKSPACE");
	myC = keyCFG->readNumEntry("KEYCODES", QString(val), SDLK_BACKSPACE);
	val = mySDL.readEntry("SDL", "LeftSoftkey", "F9");
	myLSOFT = keyCFG->readNumEntry("KEYCODES", QString(val), SDLK_F9);
	val = mySDL.readEntry("SDL", "RightSoftkey", "F11");
	myRSOFT = keyCFG->readNumEntry("KEYCODES", QString(val), SDLK_F11);
	val = mySDL.readEntry("SDL", "Call", "SPACE");
	myCALL = keyCFG->readNumEntry("KEYCODES", QString(val), SDLK_SPACE);
	val = mySDL.readEntry("SDL", "Camera", "PAUSE");
	myCAMERA = keyCFG->readNumEntry("KEYCODES", QString(val), SDLK_PAUSE);
	val = mySDL.readEntry("SDL", "VolumeUp", "PLUS");
	myVOLUP = keyCFG->readNumEntry("KEYCODES", QString(val), SDLK_PLUS);
	val = mySDL.readEntry("SDL", "VolumeDown", "MINUS");
	myVOLDOWN = keyCFG->readNumEntry("KEYCODES", QString(val), SDLK_MINUS);
	val = mySDL.readEntry("SDL", "0", "0");
	my0 = keyCFG->readNumEntry("KEYCODES", QString(val), 48);
	val = mySDL.readEntry("SDL", "1", "1");
	my1 = keyCFG->readNumEntry("KEYCODES", QString(val), 49);
	val = mySDL.readEntry("SDL", "2", "2");
	my2 = keyCFG->readNumEntry("KEYCODES", QString(val), 50);
	val = mySDL.readEntry("SDL", "3", "3");
	my3 = keyCFG->readNumEntry("KEYCODES", QString(val), 51);
	val = mySDL.readEntry("SDL", "4", "4");
	my4 = keyCFG->readNumEntry("KEYCODES", QString(val), 52);
	val = mySDL.readEntry("SDL", "5", "5");
	my5 = keyCFG->readNumEntry("KEYCODES", QString(val), 53);
	val = mySDL.readEntry("SDL", "6", "6");
	my6 = keyCFG->readNumEntry("KEYCODES", QString(val), 54);
	val = mySDL.readEntry("SDL", "7", "7");
	my7 = keyCFG->readNumEntry("KEYCODES", QString(val), 55);
	val = mySDL.readEntry("SDL", "8", "8");
	my8 = keyCFG->readNumEntry("KEYCODES", QString(val), 56);
	val = mySDL.readEntry("SDL", "9", "9");
	my9 = keyCFG->readNumEntry("KEYCODES", QString(val), 57);
	val = mySDL.readEntry("SDL", "Asterisk", "ASTERISK");
	myASTERISK = keyCFG->readNumEntry("KEYCODES", QString(val), 42);
	val = mySDL.readEntry("SDL", "Numeral", "HASH");
	myNUMERAL = keyCFG->readNumEntry("KEYCODES", QString(val), 35);
	
	val = mySDL.readEntry("SDLextra", "Red", "");
	SmyRED = keyCFG->readNumEntry("KEYCODES", QString(val), myRED);
	val = mySDL.readEntry("SDLextra", "Center", "");
	SmyCENTER = keyCFG->readNumEntry("KEYCODES", QString(val), myCENTER);
	val = mySDL.readEntry("SDLextra", "Up", "");
	SmyUP = keyCFG->readNumEntry("KEYCODES", QString(val), myUP);
	val = mySDL.readEntry("SDLextra", "Down", "");
	SmyDOWN = keyCFG->readNumEntry("KEYCODES", QString(val), myDOWN);
	val = mySDL.readEntry("SDLextra", "Left", "");
	SmyLEFT = keyCFG->readNumEntry("KEYCODES", QString(val), myLEFT);
	val = mySDL.readEntry("SDLextra", "Right", "");
	SmyRIGHT = keyCFG->readNumEntry("KEYCODES", QString(val), myRIGHT);
	val = mySDL.readEntry("SDLextra", "SideKey", "");
	SmySIDE = keyCFG->readNumEntry("KEYCODES", QString(val), mySIDE);
	val = mySDL.readEntry("SDLextra", "Music", "");
	SmyMUSIC = keyCFG->readNumEntry("KEYCODES", QString(val), myMUSIC);
	val = mySDL.readEntry("SDLextra", "C", "");
	SmyC = keyCFG->readNumEntry("KEYCODES", QString(val), myC);
	val = mySDL.readEntry("SDLextra", "LeftSoftkey", "");
	SmyLSOFT = keyCFG->readNumEntry("KEYCODES", QString(val), myLSOFT);
	val = mySDL.readEntry("SDLextra", "RightSoftkey", "");
	SmyRSOFT = keyCFG->readNumEntry("KEYCODES", QString(val), myRSOFT);
	val = mySDL.readEntry("SDLextra", "Call", "");
	SmyCALL = keyCFG->readNumEntry("KEYCODES", QString(val), myCALL);
	val = mySDL.readEntry("SDLextra", "Camera", "");
	SmyCAMERA = keyCFG->readNumEntry("KEYCODES", QString(val), myCAMERA);
	val = mySDL.readEntry("SDLextra", "VolumeUp", "");
	SmyVOLUP = keyCFG->readNumEntry("KEYCODES", QString(val), myVOLUP);
	val = mySDL.readEntry("SDLextra", "VolumeDown", "");
	SmyVOLDOWN = keyCFG->readNumEntry("KEYCODES", QString(val), myVOLDOWN);
	val = mySDL.readEntry("SDLextra", "0", "");
	Smy0 = keyCFG->readNumEntry("KEYCODES", QString(val), my0);
	val = mySDL.readEntry("SDLextra", "1", "");
	Smy1 = keyCFG->readNumEntry("KEYCODES", QString(val), my1);
	val = mySDL.readEntry("SDLextra", "2", "");
	Smy2 = keyCFG->readNumEntry("KEYCODES", QString(val), my2);
	val = mySDL.readEntry("SDLextra", "3", "");
	Smy3 = keyCFG->readNumEntry("KEYCODES", QString(val), my3);
	val = mySDL.readEntry("SDLextra", "4", "");
	Smy4 = keyCFG->readNumEntry("KEYCODES", QString(val), my4);
	val = mySDL.readEntry("SDLextra", "5", "");
	Smy5 = keyCFG->readNumEntry("KEYCODES", QString(val), my5);
	val = mySDL.readEntry("SDLextra", "6", "");
	Smy6 = keyCFG->readNumEntry("KEYCODES", QString(val), my6);
	val = mySDL.readEntry("SDLextra", "7", "");
	Smy7 = keyCFG->readNumEntry("KEYCODES", QString(val), my7);
	val = mySDL.readEntry("SDLextra", "8", "");
	Smy8 = keyCFG->readNumEntry("KEYCODES", QString(val), my8);
	val = mySDL.readEntry("SDLextra", "9", "");
	Smy9 = keyCFG->readNumEntry("KEYCODES", QString(val), my9);
	val = mySDL.readEntry("SDLextra", "Asterisk", "");
	SmyASTERISK = keyCFG->readNumEntry("KEYCODES", QString(val), myASTERISK);
	val = mySDL.readEntry("SDLextra", "Numeral", "");
	SmyNUMERAL = keyCFG->readNumEntry("KEYCODES", QString(val), myNUMERAL);
}

SDL_MainWindow::SDL_MainWindow()
    :ZKbMainWidget ( ZHeader::MAINDISPLAY_HEADER, 0, "SDL_MainWidget", 0),
    bMySpecial(false), bLastMod(false), rot(SDL_QT_NO_ROTATION),
    bControlCursor(false), iCursorStep(5), iMouseKeyState(false)
{
	DebugFunction();
	
	my_suspended = false;
	my_focus = true;

	#ifdef QT_SUPPORT_LANDSCAPE_MODE
	setOrientationLandscape(true);
	#endif

	CargarTeclas();
	
	keyLast.scancode = 0;
  	
  	disconnect( qApp, SIGNAL(askReturnToIdle(int)), qApp, SLOT(slotReturnToIdle(int)) );
  	
	connect(qApp, SIGNAL(signalRaise()), this, SLOT(slotRaise()));
	connect(qApp, SIGNAL(askReturnToIdle(int)), this, SLOT(slotReturnToIdle(int))); 
	  	
    inCallChannel = new QCopChannel("/EZX/PHONE/INCOMING_CALL", this);
    connect( inCallChannel, SIGNAL(received(const QCString&, const QByteArray&)), this, SLOT(channel(const QCString&, const QByteArray&)));
    ounCallChannel = new QCopChannel("/EZX/PHONE/OUTGOING_CALL", this);
    connect( ounCallChannel, SIGNAL(received(const QCString&, const QByteArray&)), this, SLOT(channel(const QCString&, const QByteArray&)));
	
	setFocusPolicy(QWidget::StrongFocus);
	setFullScreenMode(true);
	setFocus();
	
	#ifdef MPH_MODE_INT_PHONE
	int keypadmod = MORPHING_MODE_PHONE;
	char * envString = SDL_getenv("SDL_QT_KEYPADMODE");
	if ( envString )
		keypadmod = atoi(envString);	  
	setMorphMode(keypadmod);
	#endif
	
	qApp->installEventFilter( this );
}

SDL_MainWindow::~SDL_MainWindow() 
{
	DebugFunction();
	
	delete inCallChannel;
	delete ounCallChannel;
}

void SDL_MainWindow::channel(const QCString &msg, const QByteArray &)
{
	DebugFunction();
	
 	if ( msg == "show()" ) 
	{	
		resume();
	} else 
	if ( msg == "hide()" || msg == "MO" )
	{
		suspend();
	} else
	if (  msg == "ON" )
	{
		suspend(2);
	} else
	if (  msg == "OFF" && my_suspended==2 )
	{	
		resume();
	}
}

void SDL_MainWindow::slotRaise()
{
	printf("MAGX_VO: signal raise\n");
	resume();
}

void SDL_MainWindow::slotReturnToIdle(int)
{
	printf("MAGX_VO: signal askReturnToIdle\n");
	suspend();
}

void SDL_MainWindow::suspend( int n )
{
	printf("MAGX_VO: suspend\n");
	if (my_suspended)
	{
		hide();  
		return;
	}
	my_suspended = n;
	hide();
}

void SDL_MainWindow::focusInEvent( QFocusEvent * )
{
	printf("MAGX_VO: focus in\n");	
	setOriginalBPP(0);
	my_focus=1;
	SDL_PrivateAppActive(true, SDL_APPINPUTFOCUS);
}

void SDL_MainWindow::focusOutEvent( QFocusEvent * )
{
	printf("MAGX_VO: focus out\n");
	my_focus=0;
	setOriginalBPP(1);	
	SDL_PrivateAppActive(false, SDL_APPINPUTFOCUS);
}

void SDL_MainWindow::resume()
{
	if(!my_suspended || needSuspend()) return;
	printf("MAGX_VO: resume\n");
	show();
	
	my_suspended = 0;
}

void SDL_MainWindow::closeEvent(QCloseEvent *e) 
{
	DebugFunction();
	
	SDL_PrivateQuit();
	e->ignore();
}

inline int SDL_MainWindow::keyUp(int pressed)
{
	if ( bControlCursor )
	{
		if ( pressed )
			MouseAction(0,-1);
		return SDLK_UNKNOWN;
	}
	return bMySpecial ? SmyUP : myUP;
}

inline int SDL_MainWindow::keyDown(int pressed)
{
	if ( bControlCursor )
	{
		if ( pressed )
			MouseAction(0,1);
		return SDLK_UNKNOWN;
	}
	return bMySpecial ? SmyDOWN : myDOWN;
}

inline int SDL_MainWindow::keyLeft(int pressed)
{
	if ( bControlCursor )
	{
		if ( pressed )
			MouseAction(-1,0);
		return SDLK_UNKNOWN;
	}
	return bMySpecial ? SmyLEFT : myLEFT;
}

inline int SDL_MainWindow::keyRight(int pressed)
{
	if ( bControlCursor )
	{
		if ( pressed )
			MouseAction(1,0);
		return SDLK_UNKNOWN;
	}
	return bMySpecial ? SmyRIGHT : myRIGHT;
}

bool SDL_MainWindow::eventFilter(QObject* o, QEvent* pEvent)
{
	DebugFunction();
	
    if (QEvent::KeyPress == pEvent->type())
    {
		QueueKey((QKeyEvent*)pEvent, 1);
	    return true;
	} else
    if (QEvent::KeyRelease == pEvent->type())
    {
		QueueKey((QKeyEvent*)pEvent, 0);
	    return true;
	}

    return false;
}

/* Function to translate a keyboard transition and queue the key event
 * This should probably be a table although this method isn't exactly
 * slow.
 */
void SDL_MainWindow::QueueKey(QKeyEvent *e, int pressed)
{  
	DebugFunction();
	
	SDL_keysym keysym;
	int scancode = e->key();

	if(keyLast.scancode)
	{
		// we press/release mod-key without releasing another key
		if(bLastMod != bMySpecial)
			SDL_PrivateKeyboard(SDL_RELEASED, &keyLast);
	}

	/* Set the keysym information */
	switch(scancode) 
	{
		case KEYCODE_END:
			scancode = bMySpecial ? SmyRED : myRED;
			break;
		case KEYCODE_CENTER_SELECT:
			if ( bControlCursor )
			{
				int x, y;
				getMousPos(x, y);				
				if ( pressed )
				{
					iMouseKeyState=SDL_BUTTON_LMASK;
					SDL_PrivateMouseButton(SDL_PRESSED, SDL_BUTTON_LMASK, x, y);
				} else 
				{
					iMouseKeyState=0;
					SDL_PrivateMouseButton(SDL_RELEASED, SDL_BUTTON_LMASK, x, y);
				}
				scancode = SDLK_UNKNOWN;
			} else
				scancode = bMySpecial ? SmyCENTER : myCENTER;
			break;
		case KEYCODE_LEFT:
			if (rot == SDL_QT_ROTATION_270) scancode = keyUp(pressed);
			else if (rot == SDL_QT_ROTATION_90) scancode = keyDown(pressed);
			else scancode = keyLeft(pressed);
			break;
		case KEYCODE_UP:
			if (rot == SDL_QT_ROTATION_270) scancode = keyRight(pressed);
			else if (rot == SDL_QT_ROTATION_90) scancode = keyLeft(pressed);
			else scancode = keyUp(pressed);
			break;
		case KEYCODE_RIGHT:
			if (rot == SDL_QT_ROTATION_270) scancode = keyDown(pressed);
			else if (rot == SDL_QT_ROTATION_90) scancode = keyUp(pressed);
			else scancode = keyRight(pressed);
			break;
		case KEYCODE_DOWN:
			if (rot == SDL_QT_ROTATION_270) scancode = keyLeft(pressed);
			else if (rot == SDL_QT_ROTATION_90) scancode = keyRight(pressed);
			else scancode = keyDown(pressed);
			break;
		case KEYCODE_SIDE_SELECT:
			scancode =  bMySpecial ? SmySIDE : mySIDE;
			break;
		case KEYCODE_CARRIER:
			scancode =  bMySpecial ? SmyMUSIC : myMUSIC;
			break;
		case KEYCODE_CLEAR:
			scancode = bMySpecial ? SmyC : myC;
			break;
		case KEYCODE_LSK:
			scancode = bMySpecial ? SmyLSOFT : myLSOFT;
			break;
		case KEYCODE_RSK:
			scancode = bMySpecial ? SmyRSOFT : myRSOFT;
			break;
		case KEYCODE_SEND:
			scancode = bMySpecial ? SmyCALL : myCALL;
			break;
		case KEYCODE_IMAGING: //Key camera full pres
		case 0x4021: //Key camera pre pres
			scancode = bMySpecial ? SmyCAMERA : myCAMERA;
			break;
		case KEYCODE_SIDE_UP:
			scancode = bMySpecial ? SmyVOLUP : myVOLUP; 
			break;
		case KEYCODE_SIDE_DOWN:
			scancode = bMySpecial ? SmyVOLDOWN : myVOLDOWN;
			break;
		case KEYCODE_0:
			scancode = bMySpecial ? Smy0 : my0;
			break;
		case KEYCODE_1:
			scancode = bMySpecial ? Smy1 : my1;
			break;
		case KEYCODE_2:
			scancode = bMySpecial ? Smy2 : my2;
			break;
		case KEYCODE_3:
			scancode = bMySpecial ? Smy3 : my3;
			break;
		case KEYCODE_4:
			scancode = bMySpecial ? Smy4 : my4;
			break;
		case KEYCODE_5:
			scancode = bMySpecial ? Smy5 : my5;
			break;
		case KEYCODE_6:
			scancode = bMySpecial ? Smy6 : my6;
			break;
		case KEYCODE_7: 
			scancode = bMySpecial ? Smy7 : my7;
			break;
		case KEYCODE_8:
			scancode = bMySpecial ? Smy8 : my8;
			break;
		case KEYCODE_9:
			scancode = bMySpecial ? Smy9 : my9;
			break;
		case KEYCODE_STAR:
			scancode = bMySpecial ? SmyASTERISK : myASTERISK;
			break;
		case KEYCODE_POUND:
			scancode = bMySpecial ? SmyNUMERAL : myNUMERAL;
			break;
		case KEYCODE_LOCK:
			exit(0);
			break;
		#ifdef OMEGA_SUPPORT
		case KEYCODE_OMG_TOUCH:
			printf("OMG_TOUCH\n");
			bOmgParse=2;
			scancode = 111;
			break;     
		case KEYCODE_OMG_SCROLL:  
			printf("OMG_SCROLL\n");
			omgScroll(e);
			scancode = 111;
		break;     
			case KEYCODE_OMG_STOP:  
			//bOmgParse=0;
			scancode = 111;
			break;          
		case KEYCODE_OMG_RESUME:  
			printf("OMG_RESUME\n");
			scancode = 111;
			break;     
		case KEYCODE_OMG_RATE:  
			printf("OMG_RATE\n");
			scancode = 111;
			break;    
		#endif

		default:
			printf("MAGX: Unknown key 0x%x\n", scancode);
			scancode = SDLK_UNKNOWN;
			break;
	}

	if ( scancode == 1234 ) suspend();

	if ( scancode == 9999 ) 
	{ 
		if (!pressed) 
			bMySpecial = !bMySpecial;
		scancode = SDLK_UNKNOWN;       
	} 

	keysym.sym = static_cast<SDLKey>(scancode);
	keysym.scancode = scancode;
	keysym.mod = KMOD_NONE;
	if ( SDL_TranslateUNICODE ) 
	{
		QChar qchar = e->text()[0];
		keysym.unicode = qchar.unicode();
	} else 
	{
		keysym.unicode = 0;
	}

	keyLast = keysym;
	bLastMod = bMySpecial;

	/* Queue the key event */
	if ( pressed )
		SDL_PrivateKeyboard(SDL_PRESSED, &keysym);
	else 
	{
		keyLast.scancode = 0;
		SDL_PrivateKeyboard(SDL_RELEASED, &keysym);
	}
}

void SDL_MainWindow::MouseAction(int dx, int dy)
{
	DebugFunction();
	
	if ( dx==0 && dy==0 )
		return;
	
	int x, y;
	getMousPos(x, y);
	x+=dx*iCursorStep;
	y+=dy*iCursorStep;
	if (x < 0) x=0;
	if (x > in_width) x=in_width;
	if (y < 0) y=0;
	if (y > in_height) y=in_height;
	setMousPos(x, y);
	
	SDL_PrivateMouseMotion(iMouseKeyState, 0, x, y);
}

#ifdef OMEGA_SUPPORT
void SDL_MainWindow::omgScroll(QKeyEvent *e)
{
	DebugFunction();
	
	int step = e->step();
	//if ( (bOmgParse==2) && (step != 0) )
	if (step != 0)
		MouseAction((step>0)?1:-1, 0);
	
	QApplication::setOmegaWheelScrollLines(-step);//e->step() - 
}
#endif
