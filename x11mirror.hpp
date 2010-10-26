#ifndef _X11MIRROR_HPP
#define _X11MIRROR_HPP

#include <QObject>

class X11Mirror: public QObject
{
    Q_OBJECT

public:
    X11Mirror(Qt::HANDLE winId, QObject *parent = 0);

private:
    Qt::HANDLE windowId;
    static int eventBase;
    static int damageEventBase;

    static bool x11EventFilter(void *message, long *result);
};

#endif
