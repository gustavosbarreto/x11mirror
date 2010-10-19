#include "x11mirror.hpp"

#include <QApplication>
#include <QX11Info>

#include <X11/extensions/Xcomposite.h>
#include <X11/extensions/Xdamage.h>

static QCoreApplication::EventFilter oldEventFilter = NULL;

X11Mirror::X11Mirror(Qt::HANDLE windowId, QObject *parent)
    : QObject(parent)
{
    if (!oldEventFilter)
    {
        int errorBase;

        XCompositeQueryExtension(QX11Info::display(), &eventBase, &errorBase);
        XDamageQueryExtension(QX11Info::display(), &damageEventBase, &errorBase);

        oldEventFilter = qApp->setEventFilter(x11EventFilter);
    }
}

bool X11Mirror::x11EventFilter(void *message, long *result)
{
    XEvent *event = reinterpret_cast<XEvent *>(message);

    if (event->type == damageEventBase + XDamageNotify)
    {
        XDamageNotifyEvent *e = reinterpret_cast<XDamageNotifyEvent *>(event);
    }
}

