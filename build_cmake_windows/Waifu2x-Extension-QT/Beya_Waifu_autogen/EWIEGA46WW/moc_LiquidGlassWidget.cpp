/****************************************************************************
** Meta object code from reading C++ file 'LiquidGlassWidget.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.6.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../Waifu2x-Extension-QT/LiquidGlassWidget.h"
#include <QtCore/qmetatype.h>

#if __has_include(<QtCore/qtmochelpers.h>)
#include <QtCore/qtmochelpers.h>
#else
QT_BEGIN_MOC_NAMESPACE
#endif


#include <memory>

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'LiquidGlassWidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.6.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSLiquidGlassWidgetENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSLiquidGlassWidgetENDCLASS = QtMocHelpers::stringData(
    "LiquidGlassWidget",
    "setIOR",
    "",
    "ior",
    "setBorderRadius",
    "radius",
    "setChromaticAberrationOffset",
    "offset",
    "updateElapsedTime"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSLiquidGlassWidgetENDCLASS_t {
    uint offsetsAndSizes[18];
    char stringdata0[18];
    char stringdata1[7];
    char stringdata2[1];
    char stringdata3[4];
    char stringdata4[16];
    char stringdata5[7];
    char stringdata6[29];
    char stringdata7[7];
    char stringdata8[18];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASSLiquidGlassWidgetENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASSLiquidGlassWidgetENDCLASS_t qt_meta_stringdata_CLASSLiquidGlassWidgetENDCLASS = {
    {
        QT_MOC_LITERAL(0, 17),  // "LiquidGlassWidget"
        QT_MOC_LITERAL(18, 6),  // "setIOR"
        QT_MOC_LITERAL(25, 0),  // ""
        QT_MOC_LITERAL(26, 3),  // "ior"
        QT_MOC_LITERAL(30, 15),  // "setBorderRadius"
        QT_MOC_LITERAL(46, 6),  // "radius"
        QT_MOC_LITERAL(53, 28),  // "setChromaticAberrationOffset"
        QT_MOC_LITERAL(82, 6),  // "offset"
        QT_MOC_LITERAL(89, 17)   // "updateElapsedTime"
    },
    "LiquidGlassWidget",
    "setIOR",
    "",
    "ior",
    "setBorderRadius",
    "radius",
    "setChromaticAberrationOffset",
    "offset",
    "updateElapsedTime"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSLiquidGlassWidgetENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   38,    2, 0x0a,    1 /* Public */,
       4,    1,   41,    2, 0x0a,    3 /* Public */,
       6,    1,   44,    2, 0x0a,    5 /* Public */,
       8,    0,   47,    2, 0x08,    7 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::Float,    3,
    QMetaType::Void, QMetaType::Float,    5,
    QMetaType::Void, QMetaType::Float,    7,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject LiquidGlassWidget::staticMetaObject = { {
    QMetaObject::SuperData::link<QOpenGLWidget::staticMetaObject>(),
    qt_meta_stringdata_CLASSLiquidGlassWidgetENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSLiquidGlassWidgetENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSLiquidGlassWidgetENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<LiquidGlassWidget, std::true_type>,
        // method 'setIOR'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<float, std::false_type>,
        // method 'setBorderRadius'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<float, std::false_type>,
        // method 'setChromaticAberrationOffset'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<float, std::false_type>,
        // method 'updateElapsedTime'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void LiquidGlassWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<LiquidGlassWidget *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->setIOR((*reinterpret_cast< std::add_pointer_t<float>>(_a[1]))); break;
        case 1: _t->setBorderRadius((*reinterpret_cast< std::add_pointer_t<float>>(_a[1]))); break;
        case 2: _t->setChromaticAberrationOffset((*reinterpret_cast< std::add_pointer_t<float>>(_a[1]))); break;
        case 3: _t->updateElapsedTime(); break;
        default: ;
        }
    }
}

const QMetaObject *LiquidGlassWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *LiquidGlassWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSLiquidGlassWidgetENDCLASS.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "QOpenGLFunctions_3_3_Core"))
        return static_cast< QOpenGLFunctions_3_3_Core*>(this);
    return QOpenGLWidget::qt_metacast(_clname);
}

int LiquidGlassWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QOpenGLWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 4;
    }
    return _id;
}
QT_WARNING_POP
