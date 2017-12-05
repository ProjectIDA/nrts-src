/****************************************************************************
** Meta object code from reading C++ file 'MainWindow.h'
**
** Created: Tue May 11 11:00:34 2010
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "MainWindow.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MainWindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MainWindow[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
      26,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      12,   11,   11,   11, 0x05,
      25,   11,   11,   11, 0x05,

 // slots: signature, parameters, type, tag, flags
      41,   11,   11,   11, 0x09,
      53,   11,   11,   11, 0x09,
      71,   11,   11,   11, 0x09,
      85,   11,   11,   11, 0x09,
     100,   11,   11,   11, 0x09,
     115,   11,   11,   11, 0x09,
     130,   11,   11,   11, 0x09,
     146,   11,   11,   11, 0x09,
     166,   11,   11,   11, 0x09,
     186,   11,   11,   11, 0x09,
     204,   11,   11,   11, 0x09,
     223,   11,   11,   11, 0x09,
     239,   11,   11,   11, 0x09,
     255,   11,   11,   11, 0x09,
     274,  270,   11,   11, 0x09,
     300,   11,   11,   11, 0x09,
     315,   11,   11,   11, 0x09,
     339,   11,   11,   11, 0x09,
     352,   11,   11,   11, 0x09,
     368,   11,   11,   11, 0x09,
     381,   11,   11,   11, 0x09,
     400,   11,   11,   11, 0x09,
     419,   11,   11,   11, 0x09,
     436,   11,   11,   11, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_MainWindow[] = {
    "MainWindow\0\0openWFDISC()\0updatePreview()\0"
    "file_open()\0amp_on_ampfixed()\0"
    "amp_on_auto()\0amp_on_auto1()\0"
    "amp_on_auto0()\0amp_on_autoA()\0"
    "traces_select()\0traces_select_all()\0"
    "traces_delete_all()\0traces_original()\0"
    "traces_cliponoff()\0traces_delete()\0"
    "traces_invert()\0traces_gain1()\0pAA\0"
    "filter_selected(QAction*)\0commands_fit()\0"
    "commands_fit_vertical()\0help_about()\0"
    "traces_choose()\0file_print()\0"
    "commands_settime()\0commands_tobegin()\0"
    "commands_toend()\0commands_config()\0"
};

const QMetaObject MainWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_MainWindow,
      qt_meta_data_MainWindow, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &MainWindow::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow))
        return static_cast<void*>(const_cast< MainWindow*>(this));
    if (!strcmp(_clname, "MainWindowInterface"))
        return static_cast< MainWindowInterface*>(const_cast< MainWindow*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: openWFDISC(); break;
        case 1: updatePreview(); break;
        case 2: file_open(); break;
        case 3: amp_on_ampfixed(); break;
        case 4: amp_on_auto(); break;
        case 5: amp_on_auto1(); break;
        case 6: amp_on_auto0(); break;
        case 7: amp_on_autoA(); break;
        case 8: traces_select(); break;
        case 9: traces_select_all(); break;
        case 10: traces_delete_all(); break;
        case 11: traces_original(); break;
        case 12: traces_cliponoff(); break;
        case 13: traces_delete(); break;
        case 14: traces_invert(); break;
        case 15: traces_gain1(); break;
        case 16: filter_selected((*reinterpret_cast< QAction*(*)>(_a[1]))); break;
        case 17: commands_fit(); break;
        case 18: commands_fit_vertical(); break;
        case 19: help_about(); break;
        case 20: traces_choose(); break;
        case 21: file_print(); break;
        case 22: commands_settime(); break;
        case 23: commands_tobegin(); break;
        case 24: commands_toend(); break;
        case 25: commands_config(); break;
        default: ;
        }
        _id -= 26;
    }
    return _id;
}

// SIGNAL 0
void MainWindow::openWFDISC()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void MainWindow::updatePreview()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}
QT_END_MOC_NAMESPACE
