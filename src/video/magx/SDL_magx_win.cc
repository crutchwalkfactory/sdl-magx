#include "SDL_magx_win.h"

#include <ZApplication.h>
#include <ZKeyDef.h>
#include <qcopchannel_qws.h>
#include <qtimer.h>

#include "SDL_magx_kernel.h"

extern "C" {
#include "../../events/SDL_events_c.h"
};

/* Name of the environment variable used to invert the screen rotation or not:
Possible values:
!=0 : Screen is 270° rotated
0: Screen is 90° rotated*/
#define SDL_QT_ROTATION_ENV_NAME "SDL_QT_INVERT_ROTATION"

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
//For load keymap
extern void CargarTeclas();
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

void CargarTeclas()
{
	ZConfig mySDL(QString(qApp->argv()[0])+"_SDL.cfg", false);
	
	//Get default rotation
	int envValue;
	char * envString = SDL_getenv(SDL_QT_ROTATION_ENV_NAME);
	if ( envString )
		envValue = atoi(envString);
	else
		envValue = mySDL.readNumEntry("SYSTEM", "Rotation", 1);		  
	screenRotation = envValue ? SDL_QT_ROTATION_90 : SDL_QT_ROTATION_270;
	
	ZConfig keyCFG("/usr/mlib/SDL/keyconfig.cfg", false);
	QString val;
	val = mySDL.readEntry("SDL", "Red", "ESCAPE");
	myRED = keyCFG.readNumEntry("KEYCODES", QString(val), SDLK_ESCAPE);
	val = mySDL.readEntry("SDL", "Center", "RETURN");
	myCENTER = keyCFG.readNumEntry("KEYCODES", QString(val), SDLK_RETURN);
	val = mySDL.readEntry("SDL", "Up", "UP");
	myUP = keyCFG.readNumEntry("KEYCODES", QString(val), SDLK_UP);
	val = mySDL.readEntry("SDL", "Down", "DOWN");
	myDOWN = keyCFG.readNumEntry("KEYCODES", QString(val), SDLK_DOWN);
	val = mySDL.readEntry("SDL", "Left", "LEFT");
	myLEFT = keyCFG.readNumEntry("KEYCODES", QString(val), SDLK_LEFT);
	val = mySDL.readEntry("SDL", "Right", "RIGHT");
	myRIGHT = keyCFG.readNumEntry("KEYCODES", QString(val), SDLK_RIGHT);
	val = mySDL.readEntry("SDL", "SideKey", "TAB");
	mySIDE = keyCFG.readNumEntry("KEYCODES", QString(val), SDLK_TAB);
	val = mySDL.readEntry("SDL", "Music", "F10");
	myMUSIC = keyCFG.readNumEntry("KEYCODES", QString(val), 291);
	val = mySDL.readEntry("SDL", "C", "BACKSPACE");
	myC = keyCFG.readNumEntry("KEYCODES", QString(val), SDLK_BACKSPACE);
	val = mySDL.readEntry("SDL", "LeftSoftkey", "F9");
	myLSOFT = keyCFG.readNumEntry("KEYCODES", QString(val), SDLK_F9);
	val = mySDL.readEntry("SDL", "RightSoftkey", "F11");
	myRSOFT = keyCFG.readNumEntry("KEYCODES", QString(val), SDLK_F11);
	val = mySDL.readEntry("SDL", "Call", "SPACE");
	myCALL = keyCFG.readNumEntry("KEYCODES", QString(val), SDLK_SPACE);
	val = mySDL.readEntry("SDL", "Camera", "PAUSE");
	myCAMERA = keyCFG.readNumEntry("KEYCODES", QString(val), SDLK_PAUSE);
	val = mySDL.readEntry("SDL", "VolumeUp", "PLUS");
	myVOLUP = keyCFG.readNumEntry("KEYCODES", QString(val), SDLK_PLUS);
	val = mySDL.readEntry("SDL", "VolumeDown", "MINUS");
	myVOLDOWN = keyCFG.readNumEntry("KEYCODES", QString(val), SDLK_MINUS);
	val = mySDL.readEntry("SDL", "0", "0");
	my0 = keyCFG.readNumEntry("KEYCODES", QString(val), 48);
	val = mySDL.readEntry("SDL", "1", "1");
	my1 = keyCFG.readNumEntry("KEYCODES", QString(val), 49);
	val = mySDL.readEntry("SDL", "2", "2");
	my2 = keyCFG.readNumEntry("KEYCODES", QString(val), 50);
	val = mySDL.readEntry("SDL", "3", "3");
	my3 = keyCFG.readNumEntry("KEYCODES", QString(val), 51);
	val = mySDL.readEntry("SDL", "4", "4");
	my4 = keyCFG.readNumEntry("KEYCODES", QString(val), 52);
	val = mySDL.readEntry("SDL", "5", "5");
	my5 = keyCFG.readNumEntry("KEYCODES", QString(val), 53);
	val = mySDL.readEntry("SDL", "6", "6");
	my6 = keyCFG.readNumEntry("KEYCODES", QString(val), 54);
	val = mySDL.readEntry("SDL", "7", "7");
	my7 = keyCFG.readNumEntry("KEYCODES", QString(val), 55);
	val = mySDL.readEntry("SDL", "8", "8");
	my8 = keyCFG.readNumEntry("KEYCODES", QString(val), 56);
	val = mySDL.readEntry("SDL", "9", "9");
	my9 = keyCFG.readNumEntry("KEYCODES", QString(val), 57);
	val = mySDL.readEntry("SDL", "Asterisk", "ASTERISK");
	myASTERISK = keyCFG.readNumEntry("KEYCODES", QString(val), 42);
	val = mySDL.readEntry("SDL", "Numeral", "HASH");
	myNUMERAL = keyCFG.readNumEntry("KEYCODES", QString(val), 35);
	
	val = mySDL.readEntry("SDLextra", "Red", "");
	SmyRED = keyCFG.readNumEntry("KEYCODES", QString(val), myRED);
	val = mySDL.readEntry("SDLextra", "Center", "");
	SmyCENTER = keyCFG.readNumEntry("KEYCODES", QString(val), myCENTER);
	val = mySDL.readEntry("SDLextra", "Up", "");
	SmyUP = keyCFG.readNumEntry("KEYCODES", QString(val), myUP);
	val = mySDL.readEntry("SDLextra", "Down", "");
	SmyDOWN = keyCFG.readNumEntry("KEYCODES", QString(val), myDOWN);
	val = mySDL.readEntry("SDLextra", "Left", "");
	SmyLEFT = keyCFG.readNumEntry("KEYCODES", QString(val), myLEFT);
	val = mySDL.readEntry("SDLextra", "Right", "");
	SmyRIGHT = keyCFG.readNumEntry("KEYCODES", QString(val), myRIGHT);
	val = mySDL.readEntry("SDLextra", "SideKey", "");
	SmySIDE = keyCFG.readNumEntry("KEYCODES", QString(val), mySIDE);
	val = mySDL.readEntry("SDLextra", "Music", "");
	SmyMUSIC = keyCFG.readNumEntry("KEYCODES", QString(val), myMUSIC);
	val = mySDL.readEntry("SDLextra", "C", "");
	SmyC = keyCFG.readNumEntry("KEYCODES", QString(val), myC);
	val = mySDL.readEntry("SDLextra", "LeftSoftkey", "");
	SmyLSOFT = keyCFG.readNumEntry("KEYCODES", QString(val), myLSOFT);
	val = mySDL.readEntry("SDLextra", "RightSoftkey", "");
	SmyRSOFT = keyCFG.readNumEntry("KEYCODES", QString(val), myRSOFT);
	val = mySDL.readEntry("SDLextra", "Call", "");
	SmyCALL = keyCFG.readNumEntry("KEYCODES", QString(val), myCALL);
	val = mySDL.readEntry("SDLextra", "Camera", "");
	SmyCAMERA = keyCFG.readNumEntry("KEYCODES", QString(val), myCAMERA);
	val = mySDL.readEntry("SDLextra", "VolumeUp", "");
	SmyVOLUP = keyCFG.readNumEntry("KEYCODES", QString(val), myVOLUP);
	val = mySDL.readEntry("SDLextra", "VolumeDown", "");
	SmyVOLDOWN = keyCFG.readNumEntry("KEYCODES", QString(val), myVOLDOWN);
	val = mySDL.readEntry("SDLextra", "0", "");
	Smy0 = keyCFG.readNumEntry("KEYCODES", QString(val), my0);
	val = mySDL.readEntry("SDLextra", "1", "");
	Smy1 = keyCFG.readNumEntry("KEYCODES", QString(val), my1);
	val = mySDL.readEntry("SDLextra", "2", "");
	Smy2 = keyCFG.readNumEntry("KEYCODES", QString(val), my2);
	val = mySDL.readEntry("SDLextra", "3", "");
	Smy3 = keyCFG.readNumEntry("KEYCODES", QString(val), my3);
	val = mySDL.readEntry("SDLextra", "4", "");
	Smy4 = keyCFG.readNumEntry("KEYCODES", QString(val), my4);
	val = mySDL.readEntry("SDLextra", "5", "");
	Smy5 = keyCFG.readNumEntry("KEYCODES", QString(val), my5);
	val = mySDL.readEntry("SDLextra", "6", "");
	Smy6 = keyCFG.readNumEntry("KEYCODES", QString(val), my6);
	val = mySDL.readEntry("SDLextra", "7", "");
	Smy7 = keyCFG.readNumEntry("KEYCODES", QString(val), my7);
	val = mySDL.readEntry("SDLextra", "8", "");
	Smy8 = keyCFG.readNumEntry("KEYCODES", QString(val), my8);
	val = mySDL.readEntry("SDLextra", "9", "");
	Smy9 = keyCFG.readNumEntry("KEYCODES", QString(val), my9);
	val = mySDL.readEntry("SDLextra", "Asterisk", "");
	SmyASTERISK = keyCFG.readNumEntry("KEYCODES", QString(val), myASTERISK);
	val = mySDL.readEntry("SDLextra", "Numeral", "");
	SmyNUMERAL = keyCFG.readNumEntry("KEYCODES", QString(val), myNUMERAL);
}

SDL_MainWindow::SDL_MainWindow()
    :ZKbMainWidget ( ZHeader::MAINDISPLAY_HEADER, 0, "SDL_MainWidget", 0),
    my_mouse_pos(0, 0), my_special(false), last_mod(false), rot(SDL_QT_NO_ROTATION)
{
	my_suspended = false;
	my_focus = true;

	#ifdef QT_SUPPORT_LANDSCAPE_MODE
	setOrientationLandscape(true);
	#endif

	CargarTeclas();
	
	last.scancode = 0;
  	
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
	int keypadmod = MPH_MODE_INT_PHONE;
	char * envString = SDL_getenv("SDL_QT_KEYPADMODE");
	if ( envString )
		keypadmod = atoi(envString);	  
	setMorphMode(keypadmod);
	#endif
	
	qApp->installEventFilter( this );
}

SDL_MainWindow::~SDL_MainWindow() 
{
	delete inCallChannel;
	delete ounCallChannel;
}

void SDL_MainWindow::channel(const QCString &msg, const QByteArray &)
{
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
	printf("MAGX: signal raise\n");
	resume();
}

void SDL_MainWindow::slotReturnToIdle(int)
{
	printf("MAGX: signal askReturnToIdle\n");
	suspend();
}

void SDL_MainWindow::suspend( int n )
{
	printf("MAGX: suspend\n");
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
	printf("MAGX: focus in\n");	
	setOriginalBPP(0);
	my_focus=1;
	SDL_PrivateAppActive(true, SDL_APPINPUTFOCUS);
}

void SDL_MainWindow::focusOutEvent( QFocusEvent * )
{
	printf("MAGX: focus out\n");
	my_focus=0;
	setOriginalBPP(1);	
	SDL_PrivateAppActive(false, SDL_APPINPUTFOCUS);
}

void SDL_MainWindow::resume()
{
	if(!my_suspended || needSuspend()) return;
	printf("MAGX: resume\n");
	show();
	
	my_suspended = 0;
}

void SDL_MainWindow::closeEvent(QCloseEvent *e) 
{
	SDL_PrivateQuit();
	e->ignore();
}

void SDL_MainWindow::setMousePos(const QPoint &pos) 
{
	if (rot == SDL_QT_NO_ROTATION)
		my_mouse_pos = pos;		
	else if (rot == SDL_QT_ROTATION_270)
		my_mouse_pos = QPoint(height()-pos.y(), pos.x());
	else if (rot == SDL_QT_ROTATION_90)
		my_mouse_pos = QPoint(pos.y(), width()-pos.x());
}

inline int SDL_MainWindow::keyUp()
{
  return my_special ? SmyUP : myUP;
}

inline int SDL_MainWindow::keyDown()
{
  return my_special ? SmyDOWN : myDOWN;
}

inline int SDL_MainWindow::keyLeft()
{
  return my_special ? SmyLEFT : myLEFT;
}

inline int SDL_MainWindow::keyRight()
{
  return my_special ? SmyRIGHT : myRIGHT;
}

bool SDL_MainWindow::eventFilter(QObject* o, QEvent* pEvent)
{
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
	SDL_keysym keysym;
	int scancode = e->key();

	if(last.scancode)
	{
		// we press/release mod-key without releasing another key
		if(last_mod != my_special)
			SDL_PrivateKeyboard(SDL_RELEASED, &last);
	}

	/* Set the keysym information */
	switch(scancode) 
	{
		case KEYCODE_END:
			scancode = my_special ? SmyRED : myRED;
			break;
		case KEYCODE_CENTER_SELECT:
			scancode = my_special ? SmyCENTER : myCENTER;
			break;
		case KEYCODE_LEFT:
			if (rot == SDL_QT_ROTATION_270) scancode = keyUp();
			else if (rot == SDL_QT_ROTATION_90) scancode = keyDown();
			else scancode = keyLeft();
			break;
		case KEYCODE_UP:
			if (rot == SDL_QT_ROTATION_270) scancode = keyRight();
			else if (rot == SDL_QT_ROTATION_90) scancode = keyLeft();
			else scancode = keyUp();
			break;
		case KEYCODE_RIGHT:
			if (rot == SDL_QT_ROTATION_270) scancode = keyDown();
			else if (rot == SDL_QT_ROTATION_90) scancode = keyUp();
			else scancode = keyRight();
			break;
		case KEYCODE_DOWN:
			if (rot == SDL_QT_ROTATION_270) scancode = keyLeft();
			else if (rot == SDL_QT_ROTATION_90) scancode = keyRight();
			else scancode = keyDown();
			break;
		case KEYCODE_SIDE_SELECT:
			scancode =  my_special ? SmySIDE : mySIDE;
			break;
		case KEYCODE_CARRIER:
			scancode =  my_special ? SmyMUSIC : myMUSIC;
			break;
		case KEYCODE_CLEAR:
			scancode = my_special ? SmyC : myC;
			break;
		case KEYCODE_LSK:
			scancode = my_special ? SmyLSOFT : myLSOFT;
			break;
		case KEYCODE_RSK:
			scancode = my_special ? SmyRSOFT : myRSOFT;
			break;
		case KEYCODE_SEND:
			scancode = my_special ? SmyCALL : myCALL;
			break;
		//case KEYCODE_IMAGING:
		case 0x4021: //Key camera pre pres
			scancode = my_special ? SmyCAMERA : myCAMERA;
			break;
		case KEYCODE_SIDE_UP:
			scancode = my_special ? SmyVOLUP : myVOLUP; 
			break;
		case KEYCODE_SIDE_DOWN:
			scancode = my_special ? SmyVOLDOWN : myVOLDOWN;
			break;
		case KEYCODE_0:
			scancode = my_special ? Smy0 : my0;
			break;
		case KEYCODE_1:
			scancode = my_special ? Smy1 : my1;
			break;
		case KEYCODE_2:
			scancode = my_special ? Smy2 : my2;
			break;
		case KEYCODE_3:
			scancode = my_special ? Smy3 : my3;
			break;
		case KEYCODE_4:
			scancode = my_special ? Smy4 : my4;
			break;
		case KEYCODE_5:
			scancode = my_special ? Smy5 : my5;
			break;
		case KEYCODE_6:
			scancode = my_special ? Smy6 : my6;
			break;
		case KEYCODE_7: 
			scancode = my_special ? Smy7 : my7;
			break;
		case KEYCODE_8:
			scancode = my_special ? Smy8 : my8;
			break;
		case KEYCODE_9:
			scancode = my_special ? Smy9 : my9;
			break;
		case KEYCODE_STAR:
			scancode = my_special ? SmyASTERISK : myASTERISK;
			break;
		case KEYCODE_POUND:
			scancode = my_special ? SmyNUMERAL : myNUMERAL;
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
		if (my_special == false) 
		{ 
			if(pressed) my_special = true; else my_special = false;
			} else{
			if(pressed) my_special = false; else my_special = true;
		}
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

	last = keysym;
	last_mod = my_special;

	/* Queue the key event */
	if ( pressed )
		SDL_PrivateKeyboard(SDL_PRESSED, &keysym);
	else 
	{
		last.scancode = 0;
		SDL_PrivateKeyboard(SDL_RELEASED, &keysym);
	}
}

#ifdef OMEGA_SUPPORT
void SDL_MainWindow::omgScroll(QKeyEvent *e)
{
	int step = e->step();
	//if ( (bOmgParse==2) && (step != 0) )
	if (step != 0)
	{
		step = (step>0)?1:-1;
		if (rot == SDL_QT_ROTATION_270) 
			my_mouse_pos.setX(my_mouse_pos.x()-step*10);
		if (rot == SDL_QT_ROTATION_90)
			my_mouse_pos.setX(my_mouse_pos.x()+step*10);	
		
		if (my_mouse_pos.x()<0) my_mouse_pos.setX(0);
		if (my_mouse_pos.x()>in_width) my_mouse_pos.setX(in_width);
			
		SDL_PrivateMouseMotion(0, 0, my_mouse_pos.x(), my_mouse_pos.y());
	}
	
	QApplication::setOmegaWheelScrollLines(-step);//e->step() - 
}
#endif
