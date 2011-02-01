/****************************************************************************
** SDL_QWin meta object code from reading C++ file 'SDL_QWin.h'
**
** Created: Tue Feb 1 15:28:41 2011
**      by: The Qt MOC ($Id: qt/src/moc/moc.y   2.3.8   edited 2004-08-05 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#if !defined(Q_MOC_OUTPUT_REVISION)
#define Q_MOC_OUTPUT_REVISION 9
#elif Q_MOC_OUTPUT_REVISION != 9
#error "Moc format conflict - please regenerate all moc files"
#endif

#include "SDL_QWin.h"
#include <qmetaobject.h>
#include <qapplication.h>
#ifdef QWS
#include <qobjectdict.h>
#endif



const char *SDL_QWin::className() const
{
    return "SDL_QWin";
}

QMetaObject *SDL_QWin::metaObj = 0;

#ifdef QWS
static class SDL_QWin_metaObj_Unloader {
public:
    ~SDL_QWin_metaObj_Unloader()
    {
         if ( objectDict )
             objectDict->remove( "SDL_QWin" );
    }
} SDL_QWin_metaObj_unloader;
#endif

void SDL_QWin::initMetaObject()
{
    if ( metaObj )
	return;
    if ( qstrcmp(ZKbMainWidget::className(), "ZKbMainWidget") != 0 )
	badSuperclassWarning("SDL_QWin","ZKbMainWidget");
    (void) staticMetaObject();
}

#ifndef QT_NO_TRANSLATION

QString SDL_QWin::tr(const char* s)
{
    return qApp->translate( "SDL_QWin", s, 0 );
}

QString SDL_QWin::tr(const char* s, const char * c)
{
    return qApp->translate( "SDL_QWin", s, c );
}

#endif // QT_NO_TRANSLATION

QMetaObject* SDL_QWin::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    (void) ZKbMainWidget::staticMetaObject();
#ifndef QT_NO_PROPERTIES
#endif // QT_NO_PROPERTIES
    typedef void (SDL_QWin::*m1_t0)(const QCString&,const QByteArray&);
    typedef void (QObject::*om1_t0)(const QCString&,const QByteArray&);
    typedef void (SDL_QWin::*m1_t1)();
    typedef void (QObject::*om1_t1)();
    typedef void (SDL_QWin::*m1_t2)(int);
    typedef void (QObject::*om1_t2)(int);
    m1_t0 v1_0 = &SDL_QWin::channel;
    om1_t0 ov1_0 = (om1_t0)v1_0;
    m1_t1 v1_1 = &SDL_QWin::signalRaise;
    om1_t1 ov1_1 = (om1_t1)v1_1;
    m1_t2 v1_2 = &SDL_QWin::signalAskReturnToIdle;
    om1_t2 ov1_2 = (om1_t2)v1_2;
    QMetaData *slot_tbl = QMetaObject::new_metadata(3);
    QMetaData::Access *slot_tbl_access = QMetaObject::new_metaaccess(3);
    slot_tbl[0].name = "channel(const QCString&,const QByteArray&)";
    slot_tbl[0].ptr = (QMember)ov1_0;
    slot_tbl_access[0] = QMetaData::Public;
    slot_tbl[1].name = "signalRaise()";
    slot_tbl[1].ptr = (QMember)ov1_1;
    slot_tbl_access[1] = QMetaData::Public;
    slot_tbl[2].name = "signalAskReturnToIdle(int)";
    slot_tbl[2].ptr = (QMember)ov1_2;
    slot_tbl_access[2] = QMetaData::Public;
    metaObj = QMetaObject::new_metaobject(
	"SDL_QWin", "ZKbMainWidget",
	slot_tbl, 3,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    metaObj->set_slot_access( slot_tbl_access );
#ifndef QT_NO_PROPERTIES
#endif // QT_NO_PROPERTIES
    return metaObj;
}
