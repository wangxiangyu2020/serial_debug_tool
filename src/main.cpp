// main.cpp
#include <QApplication>
#include "ui/SplashScreen.h"
#include "ui/MainWindow.h"
#include <QWebEngineProfile>

int main(int argc, char* argv[])
{
    qputenv("QT_QPA_PLATFORM", "windows:fontengine=freetype");
    QApplication app(argc, argv);
    QFont defaultFont("Microsoft YaHei UI", 9);
    defaultFont.setStyleHint(QFont::SansSerif);
    app.setFont(defaultFont);

    SplashScreen splash;
    QScopedPointer<MainWindow> mainWindow;
    QObject::connect(&splash, &SplashScreen::finished, [&]()
    {
        mainWindow.reset(new MainWindow());
        mainWindow->show();
    });

    splash.show();

    int result = app.exec();

    // 程序退出时清理 WebEngine 资源
    QWebEngineProfile::defaultProfile()->clearHttpCache();

    return result;
}
