/****************************************************************************
** Meta object code from reading C++ file 'menuwindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.15)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../menuwindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'menuwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.15. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_MenuWindow_t {
    QByteArrayData data[15];
    char stringdata0[302];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MenuWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MenuWindow_t qt_meta_stringdata_MenuWindow = {
    {
QT_MOC_LITERAL(0, 0, 10), // "MenuWindow"
QT_MOC_LITERAL(1, 11, 21), // "on_b_usuarios_clicked"
QT_MOC_LITERAL(2, 33, 0), // ""
QT_MOC_LITERAL(3, 34, 25), // "on_b_cerrarSesion_clicked"
QT_MOC_LITERAL(4, 60, 24), // "on_user_list_itemClicked"
QT_MOC_LITERAL(5, 85, 16), // "QListWidgetItem*"
QT_MOC_LITERAL(6, 102, 4), // "item"
QT_MOC_LITERAL(7, 107, 19), // "on_user_add_clicked"
QT_MOC_LITERAL(8, 127, 22), // "on_user_delete_clicked"
QT_MOC_LITERAL(9, 150, 27), // "on_user_change_pass_clicked"
QT_MOC_LITERAL(10, 178, 27), // "on_user_change_rank_clicked"
QT_MOC_LITERAL(11, 206, 18), // "on_b_nodos_clicked"
QT_MOC_LITERAL(12, 225, 21), // "on_b_consulta_clicked"
QT_MOC_LITERAL(13, 247, 24), // "on_data_list_itemClicked"
QT_MOC_LITERAL(14, 272, 29) // "updateSensorDataAutomatically"

    },
    "MenuWindow\0on_b_usuarios_clicked\0\0"
    "on_b_cerrarSesion_clicked\0"
    "on_user_list_itemClicked\0QListWidgetItem*\0"
    "item\0on_user_add_clicked\0"
    "on_user_delete_clicked\0"
    "on_user_change_pass_clicked\0"
    "on_user_change_rank_clicked\0"
    "on_b_nodos_clicked\0on_b_consulta_clicked\0"
    "on_data_list_itemClicked\0"
    "updateSensorDataAutomatically"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MenuWindow[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   69,    2, 0x08 /* Private */,
       3,    0,   70,    2, 0x08 /* Private */,
       4,    1,   71,    2, 0x08 /* Private */,
       7,    0,   74,    2, 0x08 /* Private */,
       8,    0,   75,    2, 0x08 /* Private */,
       9,    0,   76,    2, 0x08 /* Private */,
      10,    0,   77,    2, 0x08 /* Private */,
      11,    0,   78,    2, 0x08 /* Private */,
      12,    0,   79,    2, 0x08 /* Private */,
      13,    1,   80,    2, 0x08 /* Private */,
      14,    0,   83,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 5,    6,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 5,    6,
    QMetaType::Void,

       0        // eod
};

void MenuWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MenuWindow *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->on_b_usuarios_clicked(); break;
        case 1: _t->on_b_cerrarSesion_clicked(); break;
        case 2: _t->on_user_list_itemClicked((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        case 3: _t->on_user_add_clicked(); break;
        case 4: _t->on_user_delete_clicked(); break;
        case 5: _t->on_user_change_pass_clicked(); break;
        case 6: _t->on_user_change_rank_clicked(); break;
        case 7: _t->on_b_nodos_clicked(); break;
        case 8: _t->on_b_consulta_clicked(); break;
        case 9: _t->on_data_list_itemClicked((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        case 10: _t->updateSensorDataAutomatically(); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject MenuWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_MenuWindow.data,
    qt_meta_data_MenuWindow,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *MenuWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MenuWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MenuWindow.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int MenuWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 11)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 11;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
