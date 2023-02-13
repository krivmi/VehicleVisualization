/****************************************************************************
** Meta object code from reading C++ file 'visualizer.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../Samples/VehicleVisualization/src/visualizer.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'visualizer.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Visualizer_t {
    QByteArrayData data[11];
    char stringdata0[127];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Visualizer_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Visualizer_t qt_meta_stringdata_Visualizer = {
    {
QT_MOC_LITERAL(0, 0, 10), // "Visualizer"
QT_MOC_LITERAL(1, 11, 9), // "unitClick"
QT_MOC_LITERAL(2, 21, 0), // ""
QT_MOC_LITERAL(3, 22, 9), // "stationID"
QT_MOC_LITERAL(4, 32, 11), // "hazardClick"
QT_MOC_LITERAL(5, 44, 4), // "time"
QT_MOC_LITERAL(6, 49, 4), // "code"
QT_MOC_LITERAL(7, 54, 17), // "trafficLightClick"
QT_MOC_LITERAL(8, 72, 11), // "crossroadID"
QT_MOC_LITERAL(9, 84, 25), // "adjacentIngressLanesIndex"
QT_MOC_LITERAL(10, 110, 16) // "sameTrafficLight"

    },
    "Visualizer\0unitClick\0\0stationID\0"
    "hazardClick\0time\0code\0trafficLightClick\0"
    "crossroadID\0adjacentIngressLanesIndex\0"
    "sameTrafficLight"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Visualizer[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   29,    2, 0x06 /* Public */,
       4,    2,   32,    2, 0x06 /* Public */,
       7,    3,   37,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Long,    3,
    QMetaType::Void, QMetaType::Long, QMetaType::Int,    5,    6,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Bool,    8,    9,   10,

       0        // eod
};

void Visualizer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Visualizer *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->unitClick((*reinterpret_cast< long(*)>(_a[1]))); break;
        case 1: _t->hazardClick((*reinterpret_cast< long(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 2: _t->trafficLightClick((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (Visualizer::*)(long );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Visualizer::unitClick)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (Visualizer::*)(long , int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Visualizer::hazardClick)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (Visualizer::*)(int , int , bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Visualizer::trafficLightClick)) {
                *result = 2;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Visualizer::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_Visualizer.data,
    qt_meta_data_Visualizer,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Visualizer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Visualizer::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Visualizer.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int Visualizer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void Visualizer::unitClick(long _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Visualizer::hazardClick(long _t1, int _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void Visualizer::trafficLightClick(int _t1, int _t2, bool _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
