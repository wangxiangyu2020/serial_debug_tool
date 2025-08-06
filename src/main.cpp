#include <QApplication>
#include "ui/MainWindow.h"

int main(int argc, char* argv[])
{
    qputenv("QT_QPA_PLATFORM", "windows:fontengine=freetype");
    QApplication a(argc, argv);
    a.setApplicationDisplayName("iKUN");
    QFont defaultFont("Microsoft YaHei UI", 9);
    defaultFont.setStyleHint(QFont::SansSerif);
    a.setFont(defaultFont);
    MainWindow w;
    w.show();
    return QApplication::exec();
}
