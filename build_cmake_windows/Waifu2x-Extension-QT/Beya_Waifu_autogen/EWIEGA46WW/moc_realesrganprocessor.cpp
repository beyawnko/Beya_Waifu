/****************************************************************************
** Meta object code from reading C++ file 'realesrganprocessor.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.6.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../Waifu2x-Extension-QT/realesrganprocessor.h"
#include <QtCore/qmetatype.h>

#if __has_include(<QtCore/qtmochelpers.h>)
#include <QtCore/qtmochelpers.h>
#else
QT_BEGIN_MOC_NAMESPACE
#endif


#include <memory>

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'realesrganprocessor.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_CLASSRealEsrganProcessorENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSRealEsrganProcessorENDCLASS = QtMocHelpers::stringData(
    "RealEsrganProcessor",
    "logMessage",
    "",
    "message",
    "processingFinished",
    "rowNum",
    "success",
    "statusChanged",
    "status",
    "fileProgress",
    "percent",
    "onProcessFinished",
    "exitCode",
    "QProcess::ExitStatus",
    "exitStatus",
    "onProcessError",
    "QProcess::ProcessError",
    "error",
    "onReadyReadStandardOutput",
    "onFfmpegFinished",
    "onFfmpegError",
    "onFfmpegStdErr",
    "onPipeDecoderReadyReadStandardOutput",
    "onPipeDecoderFinished",
    "onPipeDecoderError",
    "onPipeEncoderReadyReadStandardError",
    "onPipeEncoderFinished",
    "onPipeEncoderError",
    "onMediaPlayerStatusChanged",
    "QMediaPlayer::MediaStatus",
    "onMediaPlayerError",
    "QMediaPlayer::Error",
    "errorString",
    "onQtVideoFrameChanged",
    "QVideoFrame",
    "frame"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSRealEsrganProcessorENDCLASS_t {
    uint offsetsAndSizes[72];
    char stringdata0[20];
    char stringdata1[11];
    char stringdata2[1];
    char stringdata3[8];
    char stringdata4[19];
    char stringdata5[7];
    char stringdata6[8];
    char stringdata7[14];
    char stringdata8[7];
    char stringdata9[13];
    char stringdata10[8];
    char stringdata11[18];
    char stringdata12[9];
    char stringdata13[21];
    char stringdata14[11];
    char stringdata15[15];
    char stringdata16[23];
    char stringdata17[6];
    char stringdata18[26];
    char stringdata19[17];
    char stringdata20[14];
    char stringdata21[15];
    char stringdata22[37];
    char stringdata23[22];
    char stringdata24[19];
    char stringdata25[36];
    char stringdata26[22];
    char stringdata27[19];
    char stringdata28[27];
    char stringdata29[26];
    char stringdata30[19];
    char stringdata31[20];
    char stringdata32[12];
    char stringdata33[22];
    char stringdata34[12];
    char stringdata35[6];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASSRealEsrganProcessorENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASSRealEsrganProcessorENDCLASS_t qt_meta_stringdata_CLASSRealEsrganProcessorENDCLASS = {
    {
        QT_MOC_LITERAL(0, 19),  // "RealEsrganProcessor"
        QT_MOC_LITERAL(20, 10),  // "logMessage"
        QT_MOC_LITERAL(31, 0),  // ""
        QT_MOC_LITERAL(32, 7),  // "message"
        QT_MOC_LITERAL(40, 18),  // "processingFinished"
        QT_MOC_LITERAL(59, 6),  // "rowNum"
        QT_MOC_LITERAL(66, 7),  // "success"
        QT_MOC_LITERAL(74, 13),  // "statusChanged"
        QT_MOC_LITERAL(88, 6),  // "status"
        QT_MOC_LITERAL(95, 12),  // "fileProgress"
        QT_MOC_LITERAL(108, 7),  // "percent"
        QT_MOC_LITERAL(116, 17),  // "onProcessFinished"
        QT_MOC_LITERAL(134, 8),  // "exitCode"
        QT_MOC_LITERAL(143, 20),  // "QProcess::ExitStatus"
        QT_MOC_LITERAL(164, 10),  // "exitStatus"
        QT_MOC_LITERAL(175, 14),  // "onProcessError"
        QT_MOC_LITERAL(190, 22),  // "QProcess::ProcessError"
        QT_MOC_LITERAL(213, 5),  // "error"
        QT_MOC_LITERAL(219, 25),  // "onReadyReadStandardOutput"
        QT_MOC_LITERAL(245, 16),  // "onFfmpegFinished"
        QT_MOC_LITERAL(262, 13),  // "onFfmpegError"
        QT_MOC_LITERAL(276, 14),  // "onFfmpegStdErr"
        QT_MOC_LITERAL(291, 36),  // "onPipeDecoderReadyReadStandar..."
        QT_MOC_LITERAL(328, 21),  // "onPipeDecoderFinished"
        QT_MOC_LITERAL(350, 18),  // "onPipeDecoderError"
        QT_MOC_LITERAL(369, 35),  // "onPipeEncoderReadyReadStandar..."
        QT_MOC_LITERAL(405, 21),  // "onPipeEncoderFinished"
        QT_MOC_LITERAL(427, 18),  // "onPipeEncoderError"
        QT_MOC_LITERAL(446, 26),  // "onMediaPlayerStatusChanged"
        QT_MOC_LITERAL(473, 25),  // "QMediaPlayer::MediaStatus"
        QT_MOC_LITERAL(499, 18),  // "onMediaPlayerError"
        QT_MOC_LITERAL(518, 19),  // "QMediaPlayer::Error"
        QT_MOC_LITERAL(538, 11),  // "errorString"
        QT_MOC_LITERAL(550, 21),  // "onQtVideoFrameChanged"
        QT_MOC_LITERAL(572, 11),  // "QVideoFrame"
        QT_MOC_LITERAL(584, 5)   // "frame"
    },
    "RealEsrganProcessor",
    "logMessage",
    "",
    "message",
    "processingFinished",
    "rowNum",
    "success",
    "statusChanged",
    "status",
    "fileProgress",
    "percent",
    "onProcessFinished",
    "exitCode",
    "QProcess::ExitStatus",
    "exitStatus",
    "onProcessError",
    "QProcess::ProcessError",
    "error",
    "onReadyReadStandardOutput",
    "onFfmpegFinished",
    "onFfmpegError",
    "onFfmpegStdErr",
    "onPipeDecoderReadyReadStandardOutput",
    "onPipeDecoderFinished",
    "onPipeDecoderError",
    "onPipeEncoderReadyReadStandardError",
    "onPipeEncoderFinished",
    "onPipeEncoderError",
    "onMediaPlayerStatusChanged",
    "QMediaPlayer::MediaStatus",
    "onMediaPlayerError",
    "QMediaPlayer::Error",
    "errorString",
    "onQtVideoFrameChanged",
    "QVideoFrame",
    "frame"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSRealEsrganProcessorENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      19,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,  128,    2, 0x06,    1 /* Public */,
       4,    2,  131,    2, 0x06,    3 /* Public */,
       7,    2,  136,    2, 0x06,    6 /* Public */,
       9,    2,  141,    2, 0x06,    9 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
      11,    2,  146,    2, 0x08,   12 /* Private */,
      15,    1,  151,    2, 0x08,   15 /* Private */,
      18,    0,  154,    2, 0x08,   17 /* Private */,
      19,    2,  155,    2, 0x08,   18 /* Private */,
      20,    1,  160,    2, 0x08,   21 /* Private */,
      21,    0,  163,    2, 0x08,   23 /* Private */,
      22,    0,  164,    2, 0x08,   24 /* Private */,
      23,    2,  165,    2, 0x08,   25 /* Private */,
      24,    1,  170,    2, 0x08,   28 /* Private */,
      25,    0,  173,    2, 0x08,   30 /* Private */,
      26,    2,  174,    2, 0x08,   31 /* Private */,
      27,    1,  179,    2, 0x08,   34 /* Private */,
      28,    1,  182,    2, 0x08,   36 /* Private */,
      30,    2,  185,    2, 0x08,   38 /* Private */,
      33,    1,  190,    2, 0x08,   41 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::Int, QMetaType::Bool,    5,    6,
    QMetaType::Void, QMetaType::Int, QMetaType::QString,    5,    8,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    5,   10,

 // slots: parameters
    QMetaType::Void, QMetaType::Int, 0x80000000 | 13,   12,   14,
    QMetaType::Void, 0x80000000 | 16,   17,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, 0x80000000 | 13,   12,   14,
    QMetaType::Void, 0x80000000 | 16,   17,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, 0x80000000 | 13,   12,   14,
    QMetaType::Void, 0x80000000 | 16,   17,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, 0x80000000 | 13,   12,   14,
    QMetaType::Void, 0x80000000 | 16,   17,
    QMetaType::Void, 0x80000000 | 29,    8,
    QMetaType::Void, 0x80000000 | 31, QMetaType::QString,   17,   32,
    QMetaType::Void, 0x80000000 | 34,   35,

       0        // eod
};

Q_CONSTINIT const QMetaObject RealEsrganProcessor::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_CLASSRealEsrganProcessorENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSRealEsrganProcessorENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSRealEsrganProcessorENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<RealEsrganProcessor, std::true_type>,
        // method 'logMessage'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'processingFinished'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'statusChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'fileProgress'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'onProcessFinished'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<QProcess::ExitStatus, std::false_type>,
        // method 'onProcessError'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QProcess::ProcessError, std::false_type>,
        // method 'onReadyReadStandardOutput'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onFfmpegFinished'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<QProcess::ExitStatus, std::false_type>,
        // method 'onFfmpegError'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QProcess::ProcessError, std::false_type>,
        // method 'onFfmpegStdErr'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onPipeDecoderReadyReadStandardOutput'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onPipeDecoderFinished'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<QProcess::ExitStatus, std::false_type>,
        // method 'onPipeDecoderError'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QProcess::ProcessError, std::false_type>,
        // method 'onPipeEncoderReadyReadStandardError'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onPipeEncoderFinished'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<QProcess::ExitStatus, std::false_type>,
        // method 'onPipeEncoderError'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QProcess::ProcessError, std::false_type>,
        // method 'onMediaPlayerStatusChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QMediaPlayer::MediaStatus, std::false_type>,
        // method 'onMediaPlayerError'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QMediaPlayer::Error, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'onQtVideoFrameChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QVideoFrame &, std::false_type>
    >,
    nullptr
} };

void RealEsrganProcessor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<RealEsrganProcessor *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->logMessage((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 1: _t->processingFinished((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[2]))); break;
        case 2: _t->statusChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 3: _t->fileProgress((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 4: _t->onProcessFinished((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QProcess::ExitStatus>>(_a[2]))); break;
        case 5: _t->onProcessError((*reinterpret_cast< std::add_pointer_t<QProcess::ProcessError>>(_a[1]))); break;
        case 6: _t->onReadyReadStandardOutput(); break;
        case 7: _t->onFfmpegFinished((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QProcess::ExitStatus>>(_a[2]))); break;
        case 8: _t->onFfmpegError((*reinterpret_cast< std::add_pointer_t<QProcess::ProcessError>>(_a[1]))); break;
        case 9: _t->onFfmpegStdErr(); break;
        case 10: _t->onPipeDecoderReadyReadStandardOutput(); break;
        case 11: _t->onPipeDecoderFinished((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QProcess::ExitStatus>>(_a[2]))); break;
        case 12: _t->onPipeDecoderError((*reinterpret_cast< std::add_pointer_t<QProcess::ProcessError>>(_a[1]))); break;
        case 13: _t->onPipeEncoderReadyReadStandardError(); break;
        case 14: _t->onPipeEncoderFinished((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QProcess::ExitStatus>>(_a[2]))); break;
        case 15: _t->onPipeEncoderError((*reinterpret_cast< std::add_pointer_t<QProcess::ProcessError>>(_a[1]))); break;
        case 16: _t->onMediaPlayerStatusChanged((*reinterpret_cast< std::add_pointer_t<QMediaPlayer::MediaStatus>>(_a[1]))); break;
        case 17: _t->onMediaPlayerError((*reinterpret_cast< std::add_pointer_t<QMediaPlayer::Error>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 18: _t->onQtVideoFrameChanged((*reinterpret_cast< std::add_pointer_t<QVideoFrame>>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 18:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QVideoFrame >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (RealEsrganProcessor::*)(const QString & );
            if (_t _q_method = &RealEsrganProcessor::logMessage; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (RealEsrganProcessor::*)(int , bool );
            if (_t _q_method = &RealEsrganProcessor::processingFinished; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (RealEsrganProcessor::*)(int , const QString & );
            if (_t _q_method = &RealEsrganProcessor::statusChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (RealEsrganProcessor::*)(int , int );
            if (_t _q_method = &RealEsrganProcessor::fileProgress; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
    }
}

const QMetaObject *RealEsrganProcessor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *RealEsrganProcessor::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSRealEsrganProcessorENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int RealEsrganProcessor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 19)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 19;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 19)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 19;
    }
    return _id;
}

// SIGNAL 0
void RealEsrganProcessor::logMessage(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void RealEsrganProcessor::processingFinished(int _t1, bool _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void RealEsrganProcessor::statusChanged(int _t1, const QString & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void RealEsrganProcessor::fileProgress(int _t1, int _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_WARNING_POP
