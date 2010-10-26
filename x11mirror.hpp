#ifndef _X11MIRROR_HPP
#define _X11MIRROR_HPP

#include <QObject>

union _XEvent;

class X11Mirror: public QObject
{
    Q_OBJECT

public:
    X11Mirror(Qt::HANDLE winId, QObject *parent = 0);

private:
    Qt::HANDLE windowId;
    static int eventBase;
    static int damageEventBase;

    void x11EventFilter(_XEvent *event);

    friend bool eventFilter(void *message, long *result);
};

#endif
