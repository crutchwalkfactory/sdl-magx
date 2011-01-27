/****************************************************************************
** SDL_ZWin meta object code from reading C++ file 'SDL_QWin.h'
**
** Created: Thu Jan 27 19:30:28 2011
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



const char *SDL_ZWin::className() const
{
    return "SDL_ZWin";
}

QMetaObject *SDL_ZWin::metaObj = 0;

#ifdef QWS
static class SDL_ZWin_metaObj_Unloader {
public:
    ~SDL_ZWin_metaObj_Unloader()
    {
         if ( objectDict )
             objectDict->remove( "SDL_ZWin" );
    }
} SDL_ZWin_metaObj_unloader;
#endif

void SDL_ZWin::initMetaObject()
{
    if ( metaObj )
	return;
    if ( qstrcmp(ZKbMainWidget::className(), "ZKbMainWidget") != 0 )
	badSuperclassWarning("SDL_ZWin","ZKbMainWidget");
    (void) staticMetaObject();
}

#ifndef QT_NO_TRANSLATION

QString SDL_ZWin::tr(const char* s)
{
    return qApp->translate( "SDL_ZWin", s, 0 );
}

QString SDL_ZWin::tr(const char* s, const char * c)
{
    return qApp->translate( "SDL_ZWin", s, c );
}

#endif // QT_NO_TRANSLATION

QMetaObject* SDL_ZWin::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    (void) ZKbMainWidget::staticMetaObject();
#ifndef QT_NO_PROPERTIES
#endif // QT_NO_PROPERTIES
    typedef void (SDL_ZWin::*m1_t0)(const QCString&,const QByteArray&);
    typedef void (QObject::*om1_t0)(const QCString&,const QByteArray&);
    typedef void (SDL_ZWin::*m1_t1)();
    typedef void (QObject::*om1_t1)();
    typedef void (SDL_ZWin::*m1_t2)(int);
    typedef void (QObject::*om1_t2)(int);
    m1_t0 v1_0 = &SDL_ZWin::channel;
    om1_t0 ov1_0 = (om1_t0)v1_0;
    m1_t1 v1_1 = &SDL_ZWin::slotRaise;
    om1_t1 ov1_1 = (om1_t1)v1_1;
    m1_t2 v1_2 = &SDL_ZWin::slotReturnToIdle;
    om1_t2 ov1_2 = (om1_t2)v1_2;
    QMetaData *slot_tbl = QMetaObject::new_metadata(3);
    QMetaData::Access *slot_tbl_access = QMetaObject::new_metaaccess(3);
    slot_tbl[0].name = "channel(const QCString&,const QByteArray&)";
    slot_tbl[0].ptr = (QMember)ov1_0;
    slot_tbl_access[0] = QMetaData::Public;
    slot_tbl[1].name = "slotRaise()";
    slot_tbl[1].ptr = (QMember)ov1_1;
    slot_tbl_access[1] = QMetaData::Public;
    slot_tbl[2].name = "slotReturnToIdle(int)";
    slot_tbl[2].ptr = (QMember)ov1_2;
    slot_tbl_access[2] = QMetaData::Public;
    metaObj = QMetaObject::new_metaobject(
	"SDL_ZWin", "ZKbMainWidget",
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
