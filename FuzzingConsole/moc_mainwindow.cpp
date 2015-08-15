/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "mainwindow.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MainWindow[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      22,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      12,   11,   11,   11, 0x08,
      36,   11,   11,   11, 0x08,
      51,   11,   11,   11, 0x08,
      69,   67,   11,   11, 0x08,
      99,   11,   11,   11, 0x08,
     109,   11,   11,   11, 0x08,
     133,   11,   11,   11, 0x08,
     146,   11,   11,   11, 0x08,
     157,   11,   11,   11, 0x08,
     170,   11,   11,   11, 0x08,
     183,   11,   11,   11, 0x08,
     199,   11,   11,   11, 0x08,
     214,   11,   11,   11, 0x08,
     230,   11,   11,   11, 0x08,
     244,   11,   11,   11, 0x08,
     258,   11,   11,   11, 0x08,
     271,   11,   11,   11, 0x08,
     292,  289,   11,   11, 0x08,
     322,   11,   11,   11, 0x08,
     334,   11,   11,   11, 0x08,
     355,   11,   11,   11, 0x08,
     360,   11,   11,   11, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_MainWindow[] = {
    "MainWindow\0\0on_action_A_triggered()\0"
    "show_tabn(int)\0msgbox(QString)\0,\0"
    "add_phoneTab(QString,QString)\0refresh()\0"
    "phonecheck_change(bool)\0getBTSinfo()\0"
    "onBTSset()\0onBTSquery()\0sulley_add()\0"
    "sulley_remove()\0select_quick()\0"
    "select_normal()\0select_deep()\0"
    "MainGetIMSI()\0start_test()\0refresh_execTab()\0"
    ",,\0set_result(QString,bool,bool)\0"
    "exec_stop()\0on_viewrep_clicked()\0rm()\0"
    "do_file_save()\0"
};

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        MainWindow *_t = static_cast<MainWindow *>(_o);
        switch (_id) {
        case 0: _t->on_action_A_triggered(); break;
        case 1: _t->show_tabn((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->msgbox((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 3: _t->add_phoneTab((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 4: _t->refresh(); break;
        case 5: _t->phonecheck_change((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 6: _t->getBTSinfo(); break;
        case 7: _t->onBTSset(); break;
        case 8: _t->onBTSquery(); break;
        case 9: _t->sulley_add(); break;
        case 10: _t->sulley_remove(); break;
        case 11: _t->select_quick(); break;
        case 12: _t->select_normal(); break;
        case 13: _t->select_deep(); break;
        case 14: _t->MainGetIMSI(); break;
        case 15: _t->start_test(); break;
        case 16: _t->refresh_execTab(); break;
        case 17: _t->set_result((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        case 18: _t->exec_stop(); break;
        case 19: _t->on_viewrep_clicked(); break;
        case 20: _t->rm(); break;
        case 21: _t->do_file_save(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData MainWindow::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject MainWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_MainWindow,
      qt_meta_data_MainWindow, &staticMetaObjectExtraData }
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
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 22)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 22;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
