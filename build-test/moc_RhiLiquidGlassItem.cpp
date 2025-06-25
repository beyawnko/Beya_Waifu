/****************************************************************************
** Meta object code from reading C++ file 'RhiLiquidGlassItem.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../Waifu2x-Extension-QT/RhiLiquidGlassItem.h"
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'RhiLiquidGlassItem.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.4.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
namespace {
struct qt_meta_stringdata_RhiLiquidGlassItem_t {
    uint offsetsAndSizes[22];
    char stringdata0[19];
    char stringdata1[18];
    char stringdata2[1];
    char stringdata3[11];
    char stringdata4[20];
    char stringdata5[33];
    char stringdata6[17];
    char stringdata7[11];
    char stringdata8[4];
    char stringdata9[13];
    char stringdata10[26];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_RhiLiquidGlassItem_t::offsetsAndSizes) + ofs), len
Q_CONSTINIT static const qt_meta_stringdata_RhiLiquidGlassItem_t qt_meta_stringdata_RhiLiquidGlassItem = {
    {
        QT_MOC_LITERAL(0, 18),  // "RhiLiquidGlassItem"
        QT_MOC_LITERAL(19, 17),  // "backgroundChanged"
        QT_MOC_LITERAL(37, 0),  // ""
        QT_MOC_LITERAL(38, 10),  // "iorChanged"
        QT_MOC_LITERAL(49, 19),  // "borderRadiusChanged"
        QT_MOC_LITERAL(69, 32),  // "chromaticAberrationOffsetChanged"
        QT_MOC_LITERAL(102, 16),  // "onAnimationTimer"
        QT_MOC_LITERAL(119, 10),  // "background"
        QT_MOC_LITERAL(130, 3),  // "ior"
        QT_MOC_LITERAL(134, 12),  // "borderRadius"
        QT_MOC_LITERAL(147, 25)   // "chromaticAberrationOffset"
    },
    "RhiLiquidGlassItem",
    "backgroundChanged",
    "",
    "iorChanged",
    "borderRadiusChanged",
    "chromaticAberrationOffsetChanged",
    "onAnimationTimer",
    "background",
    "ior",
    "borderRadius",
    "chromaticAberrationOffset"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_RhiLiquidGlassItem[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       4,   49, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   44,    2, 0x06,    5 /* Public */,
       3,    0,   45,    2, 0x06,    6 /* Public */,
       4,    0,   46,    2, 0x06,    7 /* Public */,
       5,    0,   47,    2, 0x06,    8 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       6,    0,   48,    2, 0x08,    9 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,

 // properties: name, type, flags
       7, QMetaType::QImage, 0x00015103, uint(0), 0,
       8, QMetaType::Float, 0x00015103, uint(1), 0,
       9, QMetaType::Float, 0x00015103, uint(2), 0,
      10, QMetaType::Float, 0x00015103, uint(3), 0,

       0        // eod
};

Q_CONSTINIT const QMetaObject RhiLiquidGlassItem::staticMetaObject = { {
    QMetaObject::SuperData::link<QQuickItem::staticMetaObject>(),
    qt_meta_stringdata_RhiLiquidGlassItem.offsetsAndSizes,
    qt_meta_data_RhiLiquidGlassItem,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_RhiLiquidGlassItem_t,
        // property 'background'
        QtPrivate::TypeAndForceComplete<QImage, std::true_type>,
        // property 'ior'
        QtPrivate::TypeAndForceComplete<float, std::true_type>,
        // property 'borderRadius'
        QtPrivate::TypeAndForceComplete<float, std::true_type>,
        // property 'chromaticAberrationOffset'
        QtPrivate::TypeAndForceComplete<float, std::true_type>,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<RhiLiquidGlassItem, std::true_type>,
        // method 'backgroundChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'iorChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'borderRadiusChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'chromaticAberrationOffsetChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onAnimationTimer'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void RhiLiquidGlassItem::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<RhiLiquidGlassItem *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->backgroundChanged(); break;
        case 1: _t->iorChanged(); break;
        case 2: _t->borderRadiusChanged(); break;
        case 3: _t->chromaticAberrationOffsetChanged(); break;
        case 4: _t->onAnimationTimer(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (RhiLiquidGlassItem::*)();
            if (_t _q_method = &RhiLiquidGlassItem::backgroundChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (RhiLiquidGlassItem::*)();
            if (_t _q_method = &RhiLiquidGlassItem::iorChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (RhiLiquidGlassItem::*)();
            if (_t _q_method = &RhiLiquidGlassItem::borderRadiusChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (RhiLiquidGlassItem::*)();
            if (_t _q_method = &RhiLiquidGlassItem::chromaticAberrationOffsetChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
    }else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<RhiLiquidGlassItem *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QImage*>(_v) = _t->background(); break;
        case 1: *reinterpret_cast< float*>(_v) = _t->ior(); break;
        case 2: *reinterpret_cast< float*>(_v) = _t->borderRadius(); break;
        case 3: *reinterpret_cast< float*>(_v) = _t->chromaticAberrationOffset(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        auto *_t = static_cast<RhiLiquidGlassItem *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setBackground(*reinterpret_cast< QImage*>(_v)); break;
        case 1: _t->setIor(*reinterpret_cast< float*>(_v)); break;
        case 2: _t->setBorderRadius(*reinterpret_cast< float*>(_v)); break;
        case 3: _t->setChromaticAberrationOffset(*reinterpret_cast< float*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    } else if (_c == QMetaObject::BindableProperty) {
    }
    (void)_a;
}

const QMetaObject *RhiLiquidGlassItem::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *RhiLiquidGlassItem::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_RhiLiquidGlassItem.stringdata0))
        return static_cast<void*>(this);
    return QQuickItem::qt_metacast(_clname);
}

int RhiLiquidGlassItem::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QQuickItem::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 5;
    }else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void RhiLiquidGlassItem::backgroundChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void RhiLiquidGlassItem::iorChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void RhiLiquidGlassItem::borderRadiusChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void RhiLiquidGlassItem::chromaticAberrationOffsetChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
