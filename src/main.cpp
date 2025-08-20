// main.cpp
#include <QApplication>
#include "ui/SplashScreen.h"
#include "core/TcpNetworkManager.h"
#include "utils/ThreadSetup.h"
#include "ui/MainWindow.h"
#include <QWebEngineProfile>
#include <utils/PacketProcessor.h>

int main(int argc, char* argv[])
{
    qputenv("QT_QPA_PLATFORM", "windows:fontengine=freetype");
    QApplication app(argc, argv);
    QFont defaultFont("Microsoft YaHei UI", 9);
    defaultFont.setStyleHint(QFont::SansSerif);
    app.setFont(defaultFont);
    // 工作线程
    AppSetup::setupManagerInThread<TcpNetworkManager>(&app, "TcpNetworkManagerThread");
    AppSetup::setupManagerInThread<SerialPortManager>(&app, "SerialPortManagerThread");
    AppSetup::setupManagerInThread<ChannelManager>(&app, "ChannelManagerThread");
    // 线程队列
    PacketProcessor::getInstance()->start();

    SplashScreen splash;
    QScopedPointer<MainWindow> mainWindow;
    // QObject::connect(&splash, &SplashScreen::finished, [&]()
    // {
    //     mainWindow.reset(new MainWindow());
    //     mainWindow->show();
    // });
    //
    // splash.show();
    mainWindow.reset(new MainWindow());
    mainWindow->show();

    int result = app.exec();

    // 程序退出时清理 WebEngine 资源
    QWebEngineProfile::defaultProfile()->clearHttpCache();
    // 确保在应用退出时，线程能被安全地停止
    QObject::connect(&app, &QApplication::aboutToQuit, []()
    {
        PacketProcessor::getInstance()->quit();
        PacketProcessor::getInstance()->wait();
    });

    return result;
}
