/****************************************************************************
** KCMGGZPane meta object code from reading C++ file 'kcm_ggz_pane.h'
**
** Created: Wed Nov 7 08:17:08 2001
**      by: The Qt MOC ($Id: kcm_ggz_pane.moc.cpp 2685 2001-11-07 07:34:43Z dr_maux $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#if !defined(Q_MOC_OUTPUT_REVISION)
#define Q_MOC_OUTPUT_REVISION 9
#elif Q_MOC_OUTPUT_REVISION != 9
#error "Moc format conflict - please regenerate all moc files"
#endif

#include "kcm_ggz_pane.h"
#include <qmetaobject.h>
#include <qapplication.h>



const char *KCMGGZPane::className() const
{
    return "KCMGGZPane";
}

QMetaObject *KCMGGZPane::metaObj = 0;

void KCMGGZPane::initMetaObject()
{
    if ( metaObj )
	return;
    if ( qstrcmp(QWidget::className(), "QWidget") != 0 )
	badSuperclassWarning("KCMGGZPane","QWidget");
    (void) staticMetaObject();
}

#ifndef QT_NO_TRANSLATION

QString KCMGGZPane::tr(const char* s)
{
    return qApp->translate( "KCMGGZPane", s, 0 );
}

QString KCMGGZPane::tr(const char* s, const char * c)
{
    return qApp->translate( "KCMGGZPane", s, c );
}

#endif // QT_NO_TRANSLATION

QMetaObject* KCMGGZPane::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    (void) QWidget::staticMetaObject();
#ifndef QT_NO_PROPERTIES
#endif // QT_NO_PROPERTIES
    QMetaData::Access *slot_tbl_access = 0;
    typedef void (KCMGGZPane::*m2_t0)();
    typedef void (QObject::*om2_t0)();
    m2_t0 v2_0 = &KCMGGZPane::signalChanged;
    om2_t0 ov2_0 = (om2_t0)v2_0;
    QMetaData *signal_tbl = QMetaObject::new_metadata(1);
    signal_tbl[0].name = "signalChanged()";
    signal_tbl[0].ptr = (QMember)ov2_0;
    metaObj = QMetaObject::new_metaobject(
	"KCMGGZPane", "QWidget",
	0, 0,
	signal_tbl, 1,
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

// SIGNAL signalChanged
void KCMGGZPane::signalChanged()
{
    activate_signal( "signalChanged()" );
}
