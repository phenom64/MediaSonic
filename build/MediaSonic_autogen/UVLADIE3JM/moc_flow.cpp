/****************************************************************************
** Meta object code from reading C++ file 'flow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.13)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/flow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'flow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.13. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Flow_t {
    QByteArrayData data[25];
    char stringdata0[237];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Flow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Flow_t qt_meta_stringdata_Flow = {
    {
QT_MOC_LITERAL(0, 0, 4), // "Flow"
QT_MOC_LITERAL(1, 5, 18), // "centerIndexChanged"
QT_MOC_LITERAL(2, 24, 0), // ""
QT_MOC_LITERAL(3, 25, 11), // "QModelIndex"
QT_MOC_LITERAL(4, 37, 11), // "centerIndex"
QT_MOC_LITERAL(5, 49, 6), // "opened"
QT_MOC_LITERAL(6, 56, 3), // "idx"
QT_MOC_LITERAL(7, 60, 12), // "setRootIndex"
QT_MOC_LITERAL(8, 73, 9), // "rootIndex"
QT_MOC_LITERAL(9, 83, 5), // "reset"
QT_MOC_LITERAL(10, 89, 11), // "dataChanged"
QT_MOC_LITERAL(11, 101, 7), // "topLeft"
QT_MOC_LITERAL(12, 109, 11), // "bottomRight"
QT_MOC_LITERAL(13, 121, 12), // "rowsInserted"
QT_MOC_LITERAL(14, 134, 6), // "parent"
QT_MOC_LITERAL(15, 141, 5), // "start"
QT_MOC_LITERAL(16, 147, 3), // "end"
QT_MOC_LITERAL(17, 151, 11), // "rowsRemoved"
QT_MOC_LITERAL(18, 163, 5), // "clear"
QT_MOC_LITERAL(19, 169, 8), // "animStep"
QT_MOC_LITERAL(20, 178, 5), // "value"
QT_MOC_LITERAL(21, 184, 14), // "updateItemsPos"
QT_MOC_LITERAL(22, 199, 14), // "scrollBarMoved"
QT_MOC_LITERAL(23, 214, 10), // "continueIf"
QT_MOC_LITERAL(24, 225, 11) // "updateScene"

    },
    "Flow\0centerIndexChanged\0\0QModelIndex\0"
    "centerIndex\0opened\0idx\0setRootIndex\0"
    "rootIndex\0reset\0dataChanged\0topLeft\0"
    "bottomRight\0rowsInserted\0parent\0start\0"
    "end\0rowsRemoved\0clear\0animStep\0value\0"
    "updateItemsPos\0scrollBarMoved\0continueIf\0"
    "updateScene"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Flow[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   79,    2, 0x06 /* Public */,
       5,    1,   82,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       7,    1,   85,    2, 0x0a /* Public */,
       9,    0,   88,    2, 0x0a /* Public */,
      10,    2,   89,    2, 0x08 /* Private */,
      13,    3,   94,    2, 0x08 /* Private */,
      17,    3,  101,    2, 0x08 /* Private */,
      18,    0,  108,    2, 0x08 /* Private */,
      19,    1,  109,    2, 0x08 /* Private */,
      21,    0,  112,    2, 0x08 /* Private */,
      22,    1,  113,    2, 0x08 /* Private */,
      23,    0,  116,    2, 0x08 /* Private */,
      24,    0,  117,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3,    6,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3,    8,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 3,   11,   12,
    QMetaType::Void, 0x80000000 | 3, QMetaType::Int, QMetaType::Int,   14,   15,   16,
    QMetaType::Void, 0x80000000 | 3, QMetaType::Int, QMetaType::Int,   14,   15,   16,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QReal,   20,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   20,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void Flow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Flow *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->centerIndexChanged((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 1: _t->opened((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 2: _t->setRootIndex((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 3: _t->reset(); break;
        case 4: _t->dataChanged((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< const QModelIndex(*)>(_a[2]))); break;
        case 5: _t->rowsInserted((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 6: _t->rowsRemoved((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 7: _t->clear(); break;
        case 8: _t->animStep((*reinterpret_cast< const qreal(*)>(_a[1]))); break;
        case 9: _t->updateItemsPos(); break;
        case 10: _t->scrollBarMoved((*reinterpret_cast< const int(*)>(_a[1]))); break;
        case 11: _t->continueIf(); break;
        case 12: _t->updateScene(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (Flow::*)(const QModelIndex & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Flow::centerIndexChanged)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (Flow::*)(const QModelIndex & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Flow::opened)) {
                *result = 1;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Flow::staticMetaObject = { {
    QMetaObject::SuperData::link<QGraphicsView::staticMetaObject>(),
    qt_meta_stringdata_Flow.data,
    qt_meta_data_Flow,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Flow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Flow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Flow.stringdata0))
        return static_cast<void*>(this);
    return QGraphicsView::qt_metacast(_clname);
}

int Flow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QGraphicsView::qt_metacall(_c, _id, _a);
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
void Flow::centerIndexChanged(const QModelIndex & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Flow::opened(const QModelIndex & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
