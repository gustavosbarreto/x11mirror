#include "mirror.hpp"

#include <QApplication>
#include <QPainter>
#include <QDebug>

#include "../x11mirror.hpp"

Mirror::Mirror()
    : QWidget()
{
    x11Mirror = new X11Mirror(qApp->arguments().at(1).toULong(0, 16), this);
    connect(x11Mirror, SIGNAL(updated()), SLOT(repaint()));
}

void Mirror::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    painter.drawPixmap(0, 0, x11Mirror->buffer());
}
