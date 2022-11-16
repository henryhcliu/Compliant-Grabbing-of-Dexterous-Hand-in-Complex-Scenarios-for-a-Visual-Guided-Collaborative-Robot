#include "GloveApp.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    GloveApp w;
    w.show();
    return a.exec();
}
