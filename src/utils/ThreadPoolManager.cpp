/**
  ******************************************************************************
  * @file           : ThreadPoolManager.cpp
  * @author         : wangxiangyu
  * @brief          : None
  * @attention      : None
  * @date           : 2025/7/24
  ******************************************************************************
  */
#include "utils/ThreadPoolManager.h"

// 内部数据结构的完整定义
struct ThreadPoolManager::InternalPool
{
    QThreadPool pool;
    QList<QFuture<void>> futures;
    std::atomic<bool> stopFlag{false};

    InternalPool() = default;

    ~InternalPool()
    {
        // 确保在销毁前清理
        if (!stopFlag.load())
        {
            // 直接执行清理逻辑，避免递归调用shutdown()
            stopFlag.store(true);

            // 取消所有未开始的任务
            for (auto& future : futures)
            {
                if (future.isRunning())
                {
                    future.cancel();
                }
            }

            // 等待所有任务结束
            for (auto& future : futures)
            {
                if (future.isRunning())
                {
                    future.waitForFinished();
                }
            }

            // 清理资源
            futures.clear();
            pool.waitForDone();
        }
    }
};

// 静态变量定义
std::unique_ptr<ThreadPoolManager::InternalPool>& ThreadPoolManager::instance()
{
    static std::unique_ptr<InternalPool> s_instance;
    return s_instance;
}

QMutex& ThreadPoolManager::mutex()
{
    static QMutex s_mutex;
    return s_mutex;
}

QThreadPool* ThreadPoolManager::threadPool()
{
    return instance() ? &instance()->pool : nullptr;
}

QList<QFuture<void>>& ThreadPoolManager::futures()
{
    return instance()->futures;
}

std::atomic<bool>& ThreadPoolManager::stopFlag()
{
    return instance()->stopFlag;
}

// 初始化线程池
void ThreadPoolManager::initialize(int maxThreadCount)
{
    QMutexLocker locker(&mutex());
    if (!instance())
    {
        createInstance(maxThreadCount);
    }
}

// 创建实例
void ThreadPoolManager::createInstance(int maxThreadCount)
{
    instance() = std::make_unique<InternalPool>();
    threadPool()->setMaxThreadCount(maxThreadCount);
    stopFlag().store(false);
}

// 重置实例
void ThreadPoolManager::resetInstance()
{
    instance().reset();
}

// 确保初始化
void ThreadPoolManager::initializeIfNeeded()
{
    if (!instance())
    {
        initialize();
    }
}

// 安全停止所有线程
void ThreadPoolManager::shutdown()
{
    QMutexLocker locker(&mutex());

    // 如果实例不存在或已经停止，直接返回
    if (!instance() || stopFlag().load()) return;

    // 设置停止标志
    stopFlag().store(true);

    // 取消所有未开始的任务
    for (auto& future : futures())
    {
        if (future.isRunning())
        {
            future.cancel();
        }
    }

    // 等待所有任务结束
    for (auto& future : futures())
    {
        if (future.isRunning())
        {
            future.waitForFinished();
        }
    }

    // 清理资源
    futures().clear();
    threadPool()->waitForDone();
    resetInstance();
}

// 检查是否已请求关闭
bool ThreadPoolManager::isShutdownRequested()
{
    QMutexLocker locker(&mutex());
    return instance() ? stopFlag().load() : false;
}
