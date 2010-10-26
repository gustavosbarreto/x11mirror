#include <QApplication>

#include "mirror.hpp"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    (new Mirror)->show();

    return app.exec();
}
