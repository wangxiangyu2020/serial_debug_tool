/**
  ******************************************************************************
  * @file           : ChannelManager.cpp
  * @author         : wangxiangyu
  * @brief          : 通道信息管理工具类实现
  * @attention      : None
  * @date           : 2025/7/31
  ******************************************************************************
  */

#include "core/ChannelManager.h"

#include "core/SerialPortManager.h"
#include "ui/SerialPortConnectConfigWidget.h"

ChannelManager* ChannelManager::m_instance = nullptr;
QMutex ChannelManager::m_mutex;

ChannelManager::ChannelManager(QObject* parent)
    : QObject(parent)
{
    m_dataDispatchTimer = new QTimer(this);
    m_dataDispatchTimer->setInterval(16); // 约60FPS
    this->connect(m_dataDispatchTimer, &QTimer::timeout, this, &ChannelManager::dispatchQueuedData);
}

ChannelManager* ChannelManager::getInstance()
{
    if (m_instance == nullptr)
    {
        QMutexLocker locker(&m_mutex);
        if (m_instance == nullptr)
        {
            m_instance = new ChannelManager();
        }
    }
    return m_instance;
}

bool ChannelManager::addChannel(const QString& id, const QString& name, const QString& color)
{
    QMutexLocker locker(&m_dataMutex);

    if (name.isEmpty() || id.isEmpty() || m_channels.contains(id))
    {
        return false;
    }

    m_channels.insert(id, ChannelInfo(id, name, color));
    emit channelAdded(name, color);
    return true;
}

bool ChannelManager::removeChannel(const QString& id)
{
    QMutexLocker locker(&m_dataMutex);

    if (!m_channels.contains(id))
    {
        return false;
    }
    const auto& channel = m_channels.value(id);
    m_channels.remove(id);
    emit channelRemoved(channel.name);
    return true;
}

bool ChannelManager::updateChannel(const QString& id, const QString& name, const QString& newColor)
{
    QMutexLocker locker(&m_dataMutex);

    if (!m_channels.contains(id))
    {
        return false;
    }

    m_channels[id] = ChannelInfo(id, name, newColor);
    emit channelUpdated(name, newColor);
    return true;
}

void ChannelManager::clearChannels()
{
    QMutexLocker locker(&m_dataMutex);
    m_channels.clear();
    emit channelsCleared();
}

QList<ChannelInfo> ChannelManager::getAllChannels() const
{
    QMutexLocker locker(&m_dataMutex);
    return m_channels.values();
}

ChannelInfo ChannelManager::getChannel(const QString& id) const
{
    QMutexLocker locker(&m_dataMutex);

    if (m_channels.contains(id))
    {
        return m_channels.value(id);
    }

    return ChannelInfo();
}

bool ChannelManager::hasChannel(const QString& id) const
{
    QMutexLocker locker(&m_dataMutex);
    return m_channels.contains(id);
}

int ChannelManager::getChannelCount() const
{
    QMutexLocker locker(&m_dataMutex);
    return m_channels.size();
}

void ChannelManager::addChannelData(const QString& channelId, const QVariant& data)
{
    QMutexLocker locker(&m_channelDataMutex);

    // 将数据添加到队列
    m_dataQueue.enqueue(qMakePair(channelId, data));

    // 如果定时器未启动且允许分发，则启动定时器（通过信号槽机制）
    if (m_isDispatching && !m_dataDispatchTimer->isActive())
    {
        // 使用信号槽确保在正确线程启动定时器
        QMetaObject::invokeMethod(this, [this]()
        {
            if (m_isDispatching && !m_dataDispatchTimer->isActive())
                m_dataDispatchTimer->start();
        }, Qt::QueuedConnection);
    }
}

void ChannelManager::startDataDispatch()
{
    QMutexLocker locker(&m_queueMutex);
    m_isDispatching = true;
    if (!m_dataQueue.isEmpty() && !m_dataDispatchTimer->isActive()) m_dataDispatchTimer->start();
}

void ChannelManager::stopDataDispatch()
{
    QMutexLocker locker(&m_queueMutex);
    m_isDispatching = false;
    if (m_dataDispatchTimer->isActive()) m_dataDispatchTimer->stop();
}

void ChannelManager::dispatchQueuedData()
{
    QMutexLocker locker(&m_queueMutex);

    // 可以一次处理多个数据点，而不是一个一个处理
    int batchSize = 10; // 每次处理10个数据点
    for (int i = 0; i < batchSize && !m_dataQueue.isEmpty() && m_isDispatching; ++i)
    {
        auto dataPair = m_dataQueue.dequeue();
        locker.unlock();
        emit channelDataAdded(dataPair.first, dataPair.second);
        locker.relock();
    }

    // 如果队列为空或停止分发，则停止定时器
    if (m_dataQueue.isEmpty() || !m_isDispatching) m_dataDispatchTimer->stop();
}
