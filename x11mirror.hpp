#ifndef _X11MIRROR_HPP
#define _X11MIRROR_HPP

#include <QObject>
#include <QPixmap>

#include <X11/extensions/Xdamage.h>
#include <X11/extensions/Xrender.h>

union _XEvent;

class X11Mirror: public QObject
{
    Q_OBJECT

public:
    X11Mirror(Qt::HANDLE winId, QObject *parent = 0);

    const QPixmap &buffer() const;

signals:
    void updated();

private:
    Qt::HANDLE windowId;
    Damage damage;
    Picture picture;
    QPixmap pixmap;
    bool hasAlpha;
    static int eventBase;
    static int damageEventBase;

    void x11EventFilter(_XEvent *event);

    friend bool eventFilter(void *message, long *result);
};

#endif
