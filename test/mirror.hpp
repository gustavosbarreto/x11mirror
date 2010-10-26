#ifndef _MIRROR_HPP

#include <QWidget>

class X11Mirror;

class Mirror: public QWidget
{
    Q_OBJECT

public:
    Mirror();

protected:
    virtual void paintEvent(QPaintEvent *e);

private:
    X11Mirror *x11Mirror;
};

#endif
