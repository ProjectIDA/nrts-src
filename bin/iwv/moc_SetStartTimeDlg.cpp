/****************************************************************************
** Meta object code from reading C++ file 'SetStartTimeDlg.h'
**
** Created: Tue May 11 11:00:42 2010
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "SetStartTimeDlg.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'SetStartTimeDlg.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_SetStartTimeDlg[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      17,   16,   16,   16, 0x09,
      32,   16,   16,   16, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_SetStartTimeDlg[] = {
    "SetStartTimeDlg\0\0on_ok_button()\0"
    "on_cancel_button()\0"
};

const QMetaObject SetStartTimeDlg::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_SetStartTimeDlg,
      qt_meta_data_SetStartTimeDlg, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &SetStartTimeDlg::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *SetStartTimeDlg::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *SetStartTimeDlg::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SetStartTimeDlg))
        return static_cast<void*>(const_cast< SetStartTimeDlg*>(this));
    return QDialog::qt_metacast(_clname);
}

int SetStartTimeDlg::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: on_ok_button(); break;
        case 1: on_cancel_button(); break;
        default: ;
        }
        _id -= 2;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
