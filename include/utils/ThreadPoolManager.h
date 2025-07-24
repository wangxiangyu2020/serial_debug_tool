/**
  ******************************************************************************
  * @file           : ThreadPoolManager.h
  * @author         : wangxiangyu
  * @brief          : None
  * @attention      : None
  * @date           : 2025/7/21
  ******************************************************************************
  */

#ifndef THREADPOOLMANAGER_H
#define THREADPOOLMANAGER_H

#include <QtConcurrent>
#include <QThreadPool>
#include <QFuture>
#include <QMutex>
#include <QList>
#include <atomic>
#include <functional>
#include <memory>

class ThreadPoolManager
{
public:
    static void initialize(int maxThreadCount = QThread::idealThreadCount());

    // 添加任务（支持任意函数和参数）
    template <typename Function, typename... Args>
    static void addTask(Function&& func, Args&&... args)
    {
        // 确保线程池已初始化
        initializeIfNeeded();

        // 使用 std::bind 绑定函数和参数
        auto boundTask = std::bind(std::forward<Function>(func), std::forward<Args>(args)...);

        // 包装任务以支持停止检查
        auto wrappedTask = [boundTask]()
        {
            if (isShutdownRequested()) return;
            boundTask();
        };

        QMutexLocker locker(&mutex());
        futures().append(QtConcurrent::run(threadPool(), wrappedTask));
    }

    // 安全停止所有线程
    static void shutdown();

    // 检查是否已请求关闭
    static bool isShutdownRequested();

private:
    // 禁止实例化
    ThreadPoolManager() = delete;
    ~ThreadPoolManager() = delete;

    // 单例管理
    static void createInstance(int maxThreadCount);
    static void resetInstance();
    static void initializeIfNeeded();

    // 内部数据结构的前向声明
    struct InternalPool;

    // 访问器函数
    static std::unique_ptr<InternalPool>& instance();
    static QThreadPool* threadPool();
    static QList<QFuture<void>>& futures();
    static std::atomic<bool>& stopFlag();
    static QMutex& mutex();
};

#endif // THREADPOOLMANAGER_H
