/**
  ******************************************************************************
  * @file           : ThreadSetup.h
  * @author         : wangxiangyu
  * @brief          : None
  * @attention      : None
  * @date           : 2025/8/15
  ******************************************************************************
  */

#ifndef THREADSETUP_H
#define THREADSETUP_H

#include <QThread>
#include <QCoreApplication>
#include <QObject>
#include <QDebug>

/**
 * @brief AppSetup 命名空间提供了用于应用程序启动时配置的辅助函数。
 */
namespace AppSetup {

  /**
   * @brief 为一个单例 QObject 管理器设置专用的工作线程。
   *
   * 这个模板函数自动化了创建 QThread、将单例管理器移入其中，
   * 并连接必要的信号以实现安全生命周期管理的整个过程。
   *
   * @tparam ManagerType 管理器单例的类名 (例如 TcpManager)。
   * @param app 指向 QApplication 实例的指针，用于连接 aboutToQuit 信号。
   * @param threadName 一个描述性的线程名称，非常有助于调试。
   */
  template<typename ManagerType>
  void setupManagerInThread(QCoreApplication* app, const QString& threadName)
  {
    // 1. 创建一个新的工作线程
    QThread* workerThread = new QThread();
    workerThread->setObjectName(threadName);

    // 2. 获取管理器的单例实例
    ManagerType* manager = ManagerType::getInstance();

    // 3. 将管理器对象移动到工作线程中
    //    这是关键：之后管理器的所有槽和事件都在新线程中处理。
    manager->moveToThread(workerThread);

    // 4. 设置健壮的生命周期管理
    //    a) 当应用程序即将退出时，安全地请求线程停止其事件循环。
    QObject::connect(app, &QCoreApplication::aboutToQuit, workerThread, &QThread::quit);

    //    b) 当线程的 run() 函数结束后（即 quit() 被调用后），
    //       Qt 会自动调度删除 manager 和 workerThread 对象。
    //       这可以防止内存泄漏。
    QObject::connect(workerThread, &QThread::finished, manager, &QObject::deleteLater);
    QObject::connect(workerThread, &QThread::finished, workerThread, &QObject::deleteLater);

    // 5. 启动线程，使其开始运行自己的事件循环
    workerThread->start();

    // 打印日志，确认线程已启动
    qInfo().noquote() << QString("'%1' has been started successfully.").arg(threadName);
  }

} // namespace AppSetup

#endif // THREADSETUP_H
