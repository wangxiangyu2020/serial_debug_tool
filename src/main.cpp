// main.cpp
#include <QApplication>
#include "ui/SplashScreen.h"
#include "ui/MainWindow.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    SplashScreen splash;
    MainWindow* mainWindow = nullptr;

    QObject::connect(&splash, &SplashScreen::finished, [&]()
    {
        mainWindow = new MainWindow();
        mainWindow->show();
    });

    splash.show();

    return app.exec();
}
