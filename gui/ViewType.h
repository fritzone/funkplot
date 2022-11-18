#ifndef VIEWTYPE_H
#define VIEWTYPE_H

#include <QtGlobal>

enum class ViewType
{
    VIEW_2D_QWIDGET = 0,
    VIEW_2D = 1
#if QT_VERSION > QT_VERSION_CHECK(6, 0, 0)
    ,
    VIEW_3D = 2
#endif

};

#endif // VIEWTYPE_H
