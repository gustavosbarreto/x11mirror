/*
 * http://ktown.kde.org/~fredrik/composite_howto.html
 * yurenjimi@gmail.com
 *
 * NOTE: Replace your WID with wId (Line52)
 * xwininfo can help you ;)
 */
#include <QApplication>
#include <QDebug>
#include <X11/Xlib.h>
#include <QPaintDevice>
#include <QX11Info>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/extensions/Xcomposite.h>
#include <X11/extensions/Xrender.h>
#include <X11/extensions/Xdamage.h>

#include <QPixmap>
#include <QWidget>
#include <QLabel>

#include <QProcess>

// Use xwininfo to get wid
// FIXME: Some place will not update ;(

class MApplication : public QApplication {
    Q_OBJECT
signals:
    void renderReady();

public:
    MApplication(int argc, char *argv[]) : QApplication(argc, argv) {
        dpy = QX11Info::display();
        hasNamePixmap = false;


        if (::XCompositeQueryExtension(dpy, &event_base, &error_base)) {
            int major = 0;
            int minor = 2;
            ::XCompositeQueryVersion(dpy, &major, &minor);

            if (major > 0 || minor >= 2)
                hasNamePixmap = true;
        }

        for (int i = 0; i < ScreenCount(dpy); ++i) {
            ::XCompositeRedirectSubwindows(dpy, RootWindow(dpy, i),
                                           CompositeRedirectAutomatic);
        }

        // Get wid
        QProcess xwininfo;
        xwininfo.start("xwininfo");
        xwininfo.waitForFinished();
        QByteArray output = xwininfo.readAllStandardOutput();
        wId = output.split(':').at(3).split(' ').at(1).toULong(0, 16);

        ::XGetWindowAttributes(dpy, wId, &attr);

        format = ::XRenderFindVisualFormat(dpy, attr.visual);
        hasAlpha = (format->type == PictTypeDirect && format->direct.alphaMask);
        x         = attr.x;
        y         = attr.y;
        width     = attr.width;
        height    = attr.height;

        qDebug("x %d y %d w %d h %d\n", x, y, width, height);



        pa.subwindow_mode = IncludeInferiors;
        picture = ::XRenderCreatePicture(dpy, wId, format, CPSubwindowMode, &pa);

    //    QPixmap::fromX11Pixmap(picture).save("xx.png");
    //    ::XserverRegion region = ::XFixesCreateRegionFromWindow(dpy, wId, WindowRegionBounding);
    //    ::XFixesTranslateRegion(dpy, region, -x, -y);
    //    ::XFixesSetPictureClipRegion(dpy, picture, 0, 0, region);
    //    ::XFixesDestroyRegion(dpy, region);


        pixmap = QPixmap(width, height);
        ::XRenderComposite(dpy, hasAlpha ? PictOpOver : PictOpSrc, picture, None,
                           pixmap.x11PictureHandle(), 0, 0, 0, 0, 0, 0, width, height);



        // Tracking damage
        ::XDamageQueryExtension(dpy, &damage_event, &damage_error);

        damage = ::XDamageCreate(dpy, wId, XDamageReportNonEmpty);
    }

    bool x11EventFilter(XEvent *event) {
        if (event->type == damage_event + XDamageNotify) {
            ::XDamageNotifyEvent *e = reinterpret_cast<XDamageNotifyEvent *>(event);
            if (e->drawable == wId) {
                XDamageSubtract(dpy, e->damage, None, None);
                ::XRenderComposite(dpy, hasAlpha ? PictOpOver : PictOpSrc, picture, None,
                               pixmap.x11PictureHandle(), 0, 0, 0, 0, 0, 0, 800, 480);
                emit renderReady();
            }
        }

//        else if (event->type == ConfigureNotify) {
//            XConfigureEvent *e = &event->xconfigure;
//        }

        return false;
    }

private:
    Display *dpy;
    bool hasNamePixmap;

    int event_base;
    int error_base;

    ::XWindowAttributes attr;
    ::Window wId;

    ::XRenderPictFormat *format;
    bool hasAlpha;
    int x;
    int y;
    int width ;
    int height ;

    ::XRenderPictureAttributes pa;

    int damage_event;
    int damage_error;
    ::Damage damage;
    ::Picture picture;

public:
    QPixmap pixmap;
};

int main(int argc, char *argv[])
{
    MApplication app(argc, argv);

    QLabel label;
    QObject::connect(&app, SIGNAL(renderReady()), &label, SLOT(repaint()));
    label.setPixmap(app.pixmap);
    label.show();

    return app.exec();
}


#include "main.moc"
