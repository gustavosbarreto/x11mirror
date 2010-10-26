#include "x11mirror.hpp"

#include <QApplication>
#include <QX11Info>
#include <QList>
#include <QDebug>

#include <X11/extensions/Xcomposite.h>
#include <X11/extensions/Xdamage.h>

static QCoreApplication::EventFilter oldEventFilter = NULL;
static QList<X11Mirror *> instances;

int X11Mirror::eventBase = 0;
int X11Mirror::damageEventBase = 0;

bool eventFilter(void *message, long *result)
{
    if (oldEventFilter && oldEventFilter(message, result))
        return true;

    foreach (X11Mirror *instance, instances)
    {
        XEvent *event = reinterpret_cast<XEvent *>(message);
        instance->x11EventFilter(event);
    }

    return false;
}

X11Mirror::X11Mirror(Qt::HANDLE winId, QObject *parent)
    : QObject(parent)
    , windowId(winId)
{
    if (!oldEventFilter)
    {
        int errorBase;

        XCompositeQueryExtension(QX11Info::display(), &eventBase, &errorBase);
        XDamageQueryExtension(QX11Info::display(), &damageEventBase, &errorBase);

        oldEventFilter = qApp->setEventFilter(::eventFilter);
    }

    instances << this;
}

void X11Mirror::x11EventFilter(_XEvent *event)
{
    if (event->type == damageEventBase + XDamageNotify)
    {
        XDamageNotifyEvent *e = reinterpret_cast<XDamageNotifyEvent *>(event);
        qDebug("damaged");
    }
}
