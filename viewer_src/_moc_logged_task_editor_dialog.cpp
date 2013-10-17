/****************************************************************************
** Meta object code from reading C++ file 'logged_task_editor_dialog.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.0.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "logged_task_editor_dialog.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'logged_task_editor_dialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.0.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_LoggedTaskEditorDialog_t {
    QByteArrayData data[4];
    char stringdata[69];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_LoggedTaskEditorDialog_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_LoggedTaskEditorDialog_t qt_meta_stringdata_LoggedTaskEditorDialog = {
    {
QT_MOC_LITERAL(0, 0, 22),
QT_MOC_LITERAL(1, 23, 22),
QT_MOC_LITERAL(2, 46, 0),
QT_MOC_LITERAL(3, 47, 20)
    },
    "LoggedTaskEditorDialog\0renameSingleLoggedTask\0"
    "\0renameAllLoggedTasks\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_LoggedTaskEditorDialog[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   24,    2, 0x08,
       3,    0,   25,    2, 0x08,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void LoggedTaskEditorDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        LoggedTaskEditorDialog *_t = static_cast<LoggedTaskEditorDialog *>(_o);
        switch (_id) {
        case 0: _t->renameSingleLoggedTask(); break;
        case 1: _t->renameAllLoggedTasks(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject LoggedTaskEditorDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_LoggedTaskEditorDialog.data,
      qt_meta_data_LoggedTaskEditorDialog,  qt_static_metacall, 0, 0}
};


const QMetaObject *LoggedTaskEditorDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *LoggedTaskEditorDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_LoggedTaskEditorDialog.stringdata))
        return static_cast<void*>(const_cast< LoggedTaskEditorDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int LoggedTaskEditorDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 2;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
