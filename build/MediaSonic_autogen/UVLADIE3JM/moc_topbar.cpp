/****************************************************************************
** Meta object code from reading C++ file 'topbar.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.13)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/topbar.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'topbar.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.13. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_TopBar_t {
    QByteArrayData data[24];
    char stringdata0[295];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_TopBar_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_TopBar_t qt_meta_stringdata_TopBar = {
    {
QT_MOC_LITERAL(0, 0, 6), // "TopBar"
QT_MOC_LITERAL(1, 7, 11), // "playClicked"
QT_MOC_LITERAL(2, 19, 0), // ""
QT_MOC_LITERAL(3, 20, 12), // "pauseClicked"
QT_MOC_LITERAL(4, 33, 11), // "stopClicked"
QT_MOC_LITERAL(5, 45, 15), // "previousClicked"
QT_MOC_LITERAL(6, 61, 11), // "nextClicked"
QT_MOC_LITERAL(7, 73, 13), // "volumeChanged"
QT_MOC_LITERAL(8, 87, 6), // "volume"
QT_MOC_LITERAL(9, 94, 11), // "seekChanged"
QT_MOC_LITERAL(10, 106, 8), // "position"
QT_MOC_LITERAL(11, 115, 12), // "viewSwitched"
QT_MOC_LITERAL(12, 128, 9), // "viewIndex"
QT_MOC_LITERAL(13, 138, 17), // "searchTextChanged"
QT_MOC_LITERAL(14, 156, 4), // "text"
QT_MOC_LITERAL(15, 161, 18), // "onPlayPauseClicked"
QT_MOC_LITERAL(16, 180, 17), // "onPreviousClicked"
QT_MOC_LITERAL(17, 198, 13), // "onNextClicked"
QT_MOC_LITERAL(18, 212, 15), // "onVolumeChanged"
QT_MOC_LITERAL(19, 228, 5), // "value"
QT_MOC_LITERAL(20, 234, 19), // "onViewButtonClicked"
QT_MOC_LITERAL(21, 254, 2), // "id"
QT_MOC_LITERAL(22, 257, 19), // "onSearchTextChanged"
QT_MOC_LITERAL(23, 277, 17) // "updateElapsedTime"

    },
    "TopBar\0playClicked\0\0pauseClicked\0"
    "stopClicked\0previousClicked\0nextClicked\0"
    "volumeChanged\0volume\0seekChanged\0"
    "position\0viewSwitched\0viewIndex\0"
    "searchTextChanged\0text\0onPlayPauseClicked\0"
    "onPreviousClicked\0onNextClicked\0"
    "onVolumeChanged\0value\0onViewButtonClicked\0"
    "id\0onSearchTextChanged\0updateElapsedTime"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_TopBar[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      16,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       9,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   94,    2, 0x06 /* Public */,
       3,    0,   95,    2, 0x06 /* Public */,
       4,    0,   96,    2, 0x06 /* Public */,
       5,    0,   97,    2, 0x06 /* Public */,
       6,    0,   98,    2, 0x06 /* Public */,
       7,    1,   99,    2, 0x06 /* Public */,
       9,    1,  102,    2, 0x06 /* Public */,
      11,    1,  105,    2, 0x06 /* Public */,
      13,    1,  108,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      15,    0,  111,    2, 0x08 /* Private */,
      16,    0,  112,    2, 0x08 /* Private */,
      17,    0,  113,    2, 0x08 /* Private */,
      18,    1,  114,    2, 0x08 /* Private */,
      20,    1,  117,    2, 0x08 /* Private */,
      22,    1,  120,    2, 0x08 /* Private */,
      23,    0,  123,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    8,
    QMetaType::Void, QMetaType::LongLong,   10,
    QMetaType::Void, QMetaType::Int,   12,
    QMetaType::Void, QMetaType::QString,   14,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   19,
    QMetaType::Void, QMetaType::Int,   21,
    QMetaType::Void, QMetaType::QString,   14,
    QMetaType::Void,

       0        // eod
};

void TopBar::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<TopBar *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->playClicked(); break;
        case 1: _t->pauseClicked(); break;
        case 2: _t->stopClicked(); break;
        case 3: _t->previousClicked(); break;
        case 4: _t->nextClicked(); break;
        case 5: _t->volumeChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: _t->seekChanged((*reinterpret_cast< qint64(*)>(_a[1]))); break;
        case 7: _t->viewSwitched((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 8: _t->searchTextChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 9: _t->onPlayPauseClicked(); break;
        case 10: _t->onPreviousClicked(); break;
        case 11: _t->onNextClicked(); break;
        case 12: _t->onVolumeChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 13: _t->onViewButtonClicked((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 14: _t->onSearchTextChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 15: _t->updateElapsedTime(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (TopBar::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TopBar::playClicked)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (TopBar::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TopBar::pauseClicked)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (TopBar::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TopBar::stopClicked)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (TopBar::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TopBar::previousClicked)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (TopBar::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TopBar::nextClicked)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (TopBar::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TopBar::volumeChanged)) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (TopBar::*)(qint64 );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TopBar::seekChanged)) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (TopBar::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TopBar::viewSwitched)) {
                *result = 7;
                return;
            }
        }
        {
            using _t = void (TopBar::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TopBar::searchTextChanged)) {
                *result = 8;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject TopBar::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_TopBar.data,
    qt_meta_data_TopBar,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *TopBar::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *TopBar::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_TopBar.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int TopBar::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 16)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 16;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 16)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 16;
    }
    return _id;
}

// SIGNAL 0
void TopBar::playClicked()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void TopBar::pauseClicked()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void TopBar::stopClicked()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void TopBar::previousClicked()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void TopBar::nextClicked()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void TopBar::volumeChanged(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void TopBar::seekChanged(qint64 _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void TopBar::viewSwitched(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void TopBar::searchTextChanged(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}
struct qt_meta_stringdata_VolumeSlider_t {
    QByteArrayData data[1];
    char stringdata0[13];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_VolumeSlider_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_VolumeSlider_t qt_meta_stringdata_VolumeSlider = {
    {
QT_MOC_LITERAL(0, 0, 12) // "VolumeSlider"

    },
    "VolumeSlider"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_VolumeSlider[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

void VolumeSlider::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

QT_INIT_METAOBJECT const QMetaObject VolumeSlider::staticMetaObject = { {
    QMetaObject::SuperData::link<QSlider::staticMetaObject>(),
    qt_meta_stringdata_VolumeSlider.data,
    qt_meta_data_VolumeSlider,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *VolumeSlider::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *VolumeSlider::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_VolumeSlider.stringdata0))
        return static_cast<void*>(this);
    return QSlider::qt_metacast(_clname);
}

int VolumeSlider::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QSlider::qt_metacall(_c, _id, _a);
    return _id;
}
struct qt_meta_stringdata_ViewSwitcher_t {
    QByteArrayData data[4];
    char stringdata0[36];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ViewSwitcher_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ViewSwitcher_t qt_meta_stringdata_ViewSwitcher = {
    {
QT_MOC_LITERAL(0, 0, 12), // "ViewSwitcher"
QT_MOC_LITERAL(1, 13, 11), // "viewChanged"
QT_MOC_LITERAL(2, 25, 0), // ""
QT_MOC_LITERAL(3, 26, 9) // "viewIndex"

    },
    "ViewSwitcher\0viewChanged\0\0viewIndex"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ViewSwitcher[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   19,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int,    3,

       0        // eod
};

void ViewSwitcher::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ViewSwitcher *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->viewChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ViewSwitcher::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ViewSwitcher::viewChanged)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject ViewSwitcher::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_ViewSwitcher.data,
    qt_meta_data_ViewSwitcher,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *ViewSwitcher::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ViewSwitcher::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ViewSwitcher.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int ViewSwitcher::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void ViewSwitcher::viewChanged(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
