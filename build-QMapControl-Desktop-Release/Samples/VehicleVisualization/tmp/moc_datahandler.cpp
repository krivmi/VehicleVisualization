/****************************************************************************
** Meta object code from reading C++ file 'datahandler.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../Samples/VehicleVisualization/src/datahandler.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'datahandler.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_DataHandler_t {
    QByteArrayData data[32];
    char stringdata0[466];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_DataHandler_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_DataHandler_t qt_meta_stringdata_DataHandler = {
    {
QT_MOC_LITERAL(0, 0, 11), // "DataHandler"
QT_MOC_LITERAL(1, 12, 16), // "trafficLightShow"
QT_MOC_LITERAL(2, 29, 0), // ""
QT_MOC_LITERAL(3, 30, 11), // "crossroadID"
QT_MOC_LITERAL(4, 42, 25), // "adjacentIngressLanesIndex"
QT_MOC_LITERAL(5, 68, 16), // "sameTrafficLight"
QT_MOC_LITERAL(6, 85, 16), // "trafficLightHide"
QT_MOC_LITERAL(7, 102, 10), // "changeInfo"
QT_MOC_LITERAL(8, 113, 9), // "stationID"
QT_MOC_LITERAL(9, 123, 12), // "MessageToLog"
QT_MOC_LITERAL(10, 136, 24), // "std::shared_ptr<Message>"
QT_MOC_LITERAL(11, 161, 7), // "message"
QT_MOC_LITERAL(12, 169, 15), // "MessageReceived"
QT_MOC_LITERAL(13, 185, 11), // "CAMReceived"
QT_MOC_LITERAL(14, 197, 20), // "std::shared_ptr<Cam>"
QT_MOC_LITERAL(15, 218, 6), // "newCam"
QT_MOC_LITERAL(16, 225, 13), // "MAPEMReceived"
QT_MOC_LITERAL(17, 239, 22), // "std::shared_ptr<Mapem>"
QT_MOC_LITERAL(18, 262, 8), // "newMAPEM"
QT_MOC_LITERAL(19, 271, 14), // "SPATEMReceived"
QT_MOC_LITERAL(20, 286, 23), // "std::shared_ptr<Spatem>"
QT_MOC_LITERAL(21, 310, 9), // "newSPATEM"
QT_MOC_LITERAL(22, 320, 12), // "SREMReceived"
QT_MOC_LITERAL(23, 333, 21), // "std::shared_ptr<Srem>"
QT_MOC_LITERAL(24, 355, 7), // "newSREM"
QT_MOC_LITERAL(25, 363, 12), // "DENMReceived"
QT_MOC_LITERAL(26, 376, 21), // "std::shared_ptr<Denm>"
QT_MOC_LITERAL(27, 398, 7), // "newDENM"
QT_MOC_LITERAL(28, 406, 11), // "messagePlay"
QT_MOC_LITERAL(29, 418, 5), // "index"
QT_MOC_LITERAL(30, 424, 24), // "handleCrossroadProximity"
QT_MOC_LITERAL(31, 449, 16) // "checkCamsForSrem"

    },
    "DataHandler\0trafficLightShow\0\0crossroadID\0"
    "adjacentIngressLanesIndex\0sameTrafficLight\0"
    "trafficLightHide\0changeInfo\0stationID\0"
    "MessageToLog\0std::shared_ptr<Message>\0"
    "message\0MessageReceived\0CAMReceived\0"
    "std::shared_ptr<Cam>\0newCam\0MAPEMReceived\0"
    "std::shared_ptr<Mapem>\0newMAPEM\0"
    "SPATEMReceived\0std::shared_ptr<Spatem>\0"
    "newSPATEM\0SREMReceived\0std::shared_ptr<Srem>\0"
    "newSREM\0DENMReceived\0std::shared_ptr<Denm>\0"
    "newDENM\0messagePlay\0index\0"
    "handleCrossroadProximity\0checkCamsForSrem"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_DataHandler[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    3,   79,    2, 0x06 /* Public */,
       6,    0,   86,    2, 0x06 /* Public */,
       7,    1,   87,    2, 0x06 /* Public */,
       9,    1,   90,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      12,    1,   93,    2, 0x0a /* Public */,
      13,    1,   96,    2, 0x0a /* Public */,
      16,    1,   99,    2, 0x0a /* Public */,
      19,    1,  102,    2, 0x0a /* Public */,
      22,    1,  105,    2, 0x0a /* Public */,
      25,    1,  108,    2, 0x0a /* Public */,
      28,    1,  111,    2, 0x0a /* Public */,
      30,    0,  114,    2, 0x0a /* Public */,
      31,    0,  115,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Bool,    3,    4,    5,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Long,    8,
    QMetaType::Void, 0x80000000 | 10,   11,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 10,   11,
    QMetaType::Void, 0x80000000 | 14,   15,
    QMetaType::Void, 0x80000000 | 17,   18,
    QMetaType::Void, 0x80000000 | 20,   21,
    QMetaType::Void, 0x80000000 | 23,   24,
    QMetaType::Void, 0x80000000 | 26,   27,
    QMetaType::Void, QMetaType::Int,   29,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void DataHandler::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<DataHandler *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->trafficLightShow((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        case 1: _t->trafficLightHide(); break;
        case 2: _t->changeInfo((*reinterpret_cast< long(*)>(_a[1]))); break;
        case 3: _t->MessageToLog((*reinterpret_cast< std::shared_ptr<Message>(*)>(_a[1]))); break;
        case 4: _t->MessageReceived((*reinterpret_cast< std::shared_ptr<Message>(*)>(_a[1]))); break;
        case 5: _t->CAMReceived((*reinterpret_cast< std::shared_ptr<Cam>(*)>(_a[1]))); break;
        case 6: _t->MAPEMReceived((*reinterpret_cast< std::shared_ptr<Mapem>(*)>(_a[1]))); break;
        case 7: _t->SPATEMReceived((*reinterpret_cast< std::shared_ptr<Spatem>(*)>(_a[1]))); break;
        case 8: _t->SREMReceived((*reinterpret_cast< std::shared_ptr<Srem>(*)>(_a[1]))); break;
        case 9: _t->DENMReceived((*reinterpret_cast< std::shared_ptr<Denm>(*)>(_a[1]))); break;
        case 10: _t->messagePlay((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 11: _t->handleCrossroadProximity(); break;
        case 12: _t->checkCamsForSrem(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (DataHandler::*)(int , int , bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DataHandler::trafficLightShow)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (DataHandler::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DataHandler::trafficLightHide)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (DataHandler::*)(long );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DataHandler::changeInfo)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (DataHandler::*)(std::shared_ptr<Message> );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DataHandler::MessageToLog)) {
                *result = 3;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject DataHandler::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_DataHandler.data,
    qt_meta_data_DataHandler,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *DataHandler::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DataHandler::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_DataHandler.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int DataHandler::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 13)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 13;
    }
    return _id;
}

// SIGNAL 0
void DataHandler::trafficLightShow(int _t1, int _t2, bool _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void DataHandler::trafficLightHide()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void DataHandler::changeInfo(long _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void DataHandler::MessageToLog(std::shared_ptr<Message> _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
