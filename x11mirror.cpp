#include <QApplication>
#include <QX11Info>
#include <QList>
#include <QDebug>

#include <X11/extensions/Xcomposite.h>
#include <X11/extensions/Xdamage.h>

#include "x11mirror.hpp"

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

    XWindowAttributes attr;

    XCompositeRedirectSubwindows(QX11Info::display(), QX11Info::appRootWindow(), CompositeRedirectAutomatic);
    XGetWindowAttributes(QX11Info::display(), windowId, &attr);

    XRenderPictFormat *format = XRenderFindVisualFormat(QX11Info::display(), attr.visual);
    XRenderPictureAttributes pa;
    pa.subwindow_mode = IncludeInferiors;

    hasAlpha = (format->type == PictTypeDirect && format->direct.alphaMask);
    picture = XRenderCreatePicture(QX11Info::display(), windowId, format, CPSubwindowMode, &pa);
    pixmap = QPixmap(attr.width, attr.height);

    XRenderComposite(QX11Info::display(), hasAlpha ? PictOpOver : PictOpSrc, picture, None, pixmap.x11PictureHandle(), 0, 0, 0, 0, 0, 0, attr.width, attr.height);

    damage = XDamageCreate(QX11Info::display(), windowId, XDamageReportNonEmpty);

    instances << this;
}

const QPixmap &X11Mirror::buffer() const
{
    return pixmap;
}

void X11Mirror::x11EventFilter(_XEvent *event)
{
    if (event->type == damageEventBase + XDamageNotify)
    {
        XDamageNotifyEvent *e = reinterpret_cast<XDamageNotifyEvent *>(event);
        if (e->drawable != windowId)
            return;

        XWindowAttributes attr;

        XGetWindowAttributes(QX11Info::display(), windowId, &attr);
        XDamageSubtract(QX11Info::display(), e->damage, None, None);
        XRenderComposite(QX11Info::display(), hasAlpha ? PictOpOver : PictOpSrc, picture, None, pixmap.x11PictureHandle(), 0, 0, 0, 0, 0, 0, attr.width, attr.height);
        emit updated();
    }
}
