#include "mirror.hpp"
#include "../x11mirror.hpp"

#include <QApplication>
#include <QDebug>

Mirror::Mirror()
    : QWidget()
{
    new X11Mirror(qApp->arguments().at(1).toULong(0, 16), this);
}
