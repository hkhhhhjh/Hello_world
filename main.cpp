#include "clientwidget.h"
#include <QDebug>
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ClientWidget w;
    w.show();
    return a.exec();
}
