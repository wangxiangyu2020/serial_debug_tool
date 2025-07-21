/**
  ******************************************************************************
  * @file           : ThreadPoolManager.cpp
  * @author         : wangxiangyu
  * @brief          : None
  * @attention      : None
  * @date           : 2025/7/21
  ******************************************************************************
  */
#include "utils/ThreadPoolManager.h"

QVector<QFutureWatcher<void>*> ThreadPoolManager::m_pWatchers;

ThreadPoolManager::ThreadPoolManager(QObject* parent)
    : QObject(parent)
{
}

ThreadPoolManager::~ThreadPoolManager()
{
    this->clear();
}

void ThreadPoolManager::clear()
{
    for (auto* watcher : m_pWatchers)
    {
        watcher->cancel();
        watcher->waitForFinished();
        watcher->deleteLater();
    }
    m_pWatchers.clear();
}
