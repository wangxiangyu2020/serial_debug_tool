/**
  ******************************************************************************
  * @file           : ThreadPoolManager.h
  * @author         : wangxiangyu
  * @brief          : None
  * @attention      : None
  * @date           : 2025/7/21
  ******************************************************************************
  */

#include <QObject>
#include <QFutureWatcher>
#include <QVector>
#include <QtConcurrent>

#ifndef THREADPOOLMANAGER_H
#define THREADPOOLMANAGER_H

class ThreadPoolManager : public QObject
{
    Q_OBJECT

public:
    explicit ThreadPoolManager(QObject* parent = nullptr);
    ~ThreadPoolManager();

    static QVector<QFutureWatcher<void>*> m_pWatchers;

    template <typename Func, typename Callback, typename... Args>
    static void addTask(Func&& func, Callback&& callback, Args&&... args)
    {
        using ReturnType = std::invoke_result_t<Func, Args...>;
        auto* watcher = new QFutureWatcher<ReturnType>();
        if constexpr (std::is_void_v<ReturnType>)
        {
            QObject::connect(watcher, &QFutureWatcher<ReturnType>::finished, [watcher, callback]()
            {
                callback();
                watcher->deleteLater();
            });
        }
        else
        {
            QObject::connect(watcher, &QFutureWatcher<ReturnType>::finished, [watcher, callback]()
            {
                callback(watcher->result());
                watcher->deleteLater();
            });
        }
        QFuture<ReturnType> future = QtConcurrent::run(std::forward<Func>(func), std::forward<Args>(args)...);
        watcher->setFuture(future);
        ThreadPoolManager::m_pWatchers.append(reinterpret_cast<QFutureWatcher<void>*>(watcher));
    }

    static void clear();
};

#endif //THREADPOOLMANAGER_H
