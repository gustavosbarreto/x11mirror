#include "mirror.hpp"

#include <QApplication>
#include <QDebug>

#include "../x11mirror.hpp"

Mirror::Mirror()
    : QWidget()
{
    new X11Mirror(qApp->arguments().at(1).toULong(0, 16), this);
}
