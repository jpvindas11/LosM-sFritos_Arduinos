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
    QByteArrayData data[17];
    char stringdata0[350];
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
QT_MOC_LITERAL(3, 34, 21), // "on_b_arduinos_clicked"
QT_MOC_LITERAL(4, 56, 25), // "on_b_cerrarSesion_clicked"
QT_MOC_LITERAL(5, 82, 24), // "on_user_list_itemClicked"
QT_MOC_LITERAL(6, 107, 16), // "QListWidgetItem*"
QT_MOC_LITERAL(7, 124, 4), // "item"
QT_MOC_LITERAL(8, 129, 19), // "on_user_add_clicked"
QT_MOC_LITERAL(9, 149, 22), // "on_user_delete_clicked"
QT_MOC_LITERAL(10, 172, 27), // "on_user_change_pass_clicked"
QT_MOC_LITERAL(11, 200, 27), // "on_user_change_rank_clicked"
QT_MOC_LITERAL(12, 228, 18), // "on_b_nodos_clicked"
QT_MOC_LITERAL(13, 247, 21), // "on_b_consulta_clicked"
QT_MOC_LITERAL(14, 269, 27), // "on_arduino_list_itemClicked"
QT_MOC_LITERAL(15, 297, 28), // "on_arduino_consultar_Clicked"
QT_MOC_LITERAL(16, 326, 23) // "on_arduino_turn_clicked"

    },
    "MenuWindow\0on_b_usuarios_clicked\0\0"
    "on_b_arduinos_clicked\0on_b_cerrarSesion_clicked\0"
    "on_user_list_itemClicked\0QListWidgetItem*\0"
    "item\0on_user_add_clicked\0"
    "on_user_delete_clicked\0"
    "on_user_change_pass_clicked\0"
    "on_user_change_rank_clicked\0"
    "on_b_nodos_clicked\0on_b_consulta_clicked\0"
    "on_arduino_list_itemClicked\0"
    "on_arduino_consultar_Clicked\0"
    "on_arduino_turn_clicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MenuWindow[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   79,    2, 0x08 /* Private */,
       3,    0,   80,    2, 0x08 /* Private */,
       4,    0,   81,    2, 0x08 /* Private */,
       5,    1,   82,    2, 0x08 /* Private */,
       8,    0,   85,    2, 0x08 /* Private */,
       9,    0,   86,    2, 0x08 /* Private */,
      10,    0,   87,    2, 0x08 /* Private */,
      11,    0,   88,    2, 0x08 /* Private */,
      12,    0,   89,    2, 0x08 /* Private */,
      13,    0,   90,    2, 0x08 /* Private */,
      14,    1,   91,    2, 0x08 /* Private */,
      15,    0,   94,    2, 0x08 /* Private */,
      16,    0,   95,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 6,    7,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 6,    7,
    QMetaType::Void,
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
        case 1: _t->on_b_arduinos_clicked(); break;
        case 2: _t->on_b_cerrarSesion_clicked(); break;
        case 3: _t->on_user_list_itemClicked((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        case 4: _t->on_user_add_clicked(); break;
        case 5: _t->on_user_delete_clicked(); break;
        case 6: _t->on_user_change_pass_clicked(); break;
        case 7: _t->on_user_change_rank_clicked(); break;
        case 8: _t->on_b_nodos_clicked(); break;
        case 9: _t->on_b_consulta_clicked(); break;
        case 10: _t->on_arduino_list_itemClicked((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        case 11: _t->on_arduino_consultar_Clicked(); break;
        case 12: _t->on_arduino_turn_clicked(); break;
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
QT_WARNING_POP
QT_END_MOC_NAMESPACE
