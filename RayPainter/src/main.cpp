#include "MainWindow.hpp"

#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
#ifdef QT_DEBUG
    w.setWindowTitle("Ray Painter DEBUG");
#else
    w.setWindowTitle("Ray Painter");
#endif
    w.resize(800, 600);
    w.show();
    return a.exec();
}
