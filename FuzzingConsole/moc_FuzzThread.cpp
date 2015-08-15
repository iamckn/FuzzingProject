/****************************************************************************
** Meta object code from reading C++ file 'FuzzThread.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "FuzzThread.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'FuzzThread.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_FuzzThread[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       8,       // signalCount

 // signals: signature, parameters, type, tag, flags
      12,   11,   11,   11, 0x05,
      33,   11,   11,   11, 0x05,
      49,   11,   11,   11, 0x05,
      71,   69,   11,   11, 0x05,
     113,   95,   11,   11, 0x05,
     143,   11,   11,   11, 0x05,
     161,  157,   11,   11, 0x05,
     181,   11,   11,   11, 0x05,

 // slots: signature, parameters, type, tag, flags
     203,  201,   11,   11, 0x0a,
     220,   11,   11,   11, 0x0a,
     228,  201,   11,   11, 0x0a,
     239,   11,   11,   11, 0x0a,
     253,   11,   11,   11, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_FuzzThread[] = {
    "FuzzThread\0\0raiseMsgbox(QString)\0"
    "setProcess(int)\0setMessage(QString)\0"
    ",\0addTab(QString,QString)\0msg,integer,state\0"
    "FuzzResult(QString,bool,bool)\0"
    "sendBTSinfo()\0htm\0sendReport(QString)\0"
    "imsigetted(QString)\0a\0addTask(QString)\0"
    "start()\0stop(bool)\0force_abort()\0"
    "setBtsinfo()\0"
};

void FuzzThread::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        FuzzThread *_t = static_cast<FuzzThread *>(_o);
        switch (_id) {
        case 0: _t->raiseMsgbox((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 1: _t->setProcess((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->setMessage((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 3: _t->addTab((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 4: _t->FuzzResult((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        case 5: _t->sendBTSinfo(); break;
        case 6: _t->sendReport((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 7: _t->imsigetted((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 8: _t->addTask((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 9: _t->start(); break;
        case 10: _t->stop((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 11: _t->force_abort(); break;
        case 12: _t->setBtsinfo(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData FuzzThread::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject FuzzThread::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_FuzzThread,
      qt_meta_data_FuzzThread, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &FuzzThread::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *FuzzThread::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *FuzzThread::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_FuzzThread))
        return static_cast<void*>(const_cast< FuzzThread*>(this));
    return QThread::qt_metacast(_clname);
}

int FuzzThread::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    }
    return _id;
}

// SIGNAL 0
void FuzzThread::raiseMsgbox(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void FuzzThread::setProcess(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void FuzzThread::setMessage(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void FuzzThread::addTab(QString _t1, QString _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void FuzzThread::FuzzResult(QString _t1, bool _t2, bool _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void FuzzThread::sendBTSinfo()
{
    QMetaObject::activate(this, &staticMetaObject, 5, 0);
}

// SIGNAL 6
void FuzzThread::sendReport(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void FuzzThread::imsigetted(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}
QT_END_MOC_NAMESPACE
