#include <QApplication>
#include "ui/MainWindow.h"

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    a.setApplicationDisplayName("iKUN");
    MainWindow w;
    w.show();
    return QApplication::exec();
}
