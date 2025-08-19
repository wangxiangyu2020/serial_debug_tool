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

ChannelManager::ChannelManager(QObject* parent)
    : QObject(parent)
{
    m_dataDispatchTimer = new QTimer(this);
    m_dataDispatchTimer->setInterval(16); // 约60FPS
    this->connectSignals();
}

void ChannelManager::connectSignals()
{
    this->connect(m_dataDispatchTimer, &QTimer::timeout, this, &ChannelManager::onDispatchQueuedData);
}

void ChannelManager::onGetAllChannels()
{
    emit sendAllChannelsRequested(getAllChannels());
}

void ChannelManager::onAddChannel(const QString& id, const QString& name, const QString& color)
{
    if (name.isEmpty() || id.isEmpty() || m_channels.contains(id))
    {
        emit statusChanged(QString("通道标识%1已存在").arg(id));
        return;
    }

    m_channels.insert(id, ChannelInfo(id, name, color));
    emit channelAddedRequested(name, color);
    emit statusChanged(QString("通道%1添加成功").arg(name));
}

void ChannelManager::onRemoveChannel(const QString& id)
{
    if (!m_channels.contains(id))
    {
        emit statusChanged(QString("通道标识%1不存在").arg(id));
        return;
    }
    const auto& channel = m_channels.value(id);
    m_channels.remove(id);
    emit channelRemovedRequested(channel.name);
    emit statusChanged(QString("通道%1删除成功").arg(channel.name));
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

int ChannelManager::getSampleRate() const
{
    return m_sampleRate;
}

void ChannelManager::onClearAllChannelData()
{
    m_dataQueue.clear();
    emit channelsDataAllClearedRequested();
}

void ChannelManager::onImportChannelsData()
{
    emit channelsDataImportedRequested();
}

void ChannelManager::onExportChannelsData()
{
    emit channelsDataExportedRequested();
}

void ChannelManager::onGetSampleRate()
{
    emit sendSampleRateRequested(m_sampleRate);
}

void ChannelManager::onSetSampleRate(int rate)
{
    m_sampleRate = rate;
    emit statusChanged(QString("采样间隔设置为: %1 ms").arg(rate));
}

void ChannelManager::onStartDataDispatch()
{
    m_isDispatching = true;
    if (!m_dataQueue.isEmpty() && !m_dataDispatchTimer->isActive()) m_dataDispatchTimer->start();
    emit channelDataProcessRequested(true);
    emit statusChanged(QString("数据分发已启动"));
}

void ChannelManager::onStopDataDispatch()
{
    m_isDispatching = false;
    if (m_dataDispatchTimer->isActive()) m_dataDispatchTimer->stop();
    emit channelDataProcessRequested(false);
    emit statusChanged(QString("数据分发已停止"));
}

void ChannelManager::onDispatchQueuedData()
{
    QMutexLocker locker(&m_queueMutex);

    // 可以一次处理多个数据点，而不是一个一个处理
    int batchSize = 10; // 每次处理10个数据点
    for (int i = 0; i < batchSize && !m_dataQueue.isEmpty() && m_isDispatching; ++i)
    {
        auto dataPair = m_dataQueue.dequeue();
        locker.unlock();
        emit channelDataAddedRequested(this->getChannel(dataPair.first).name, dataPair.second);
        locker.relock();
    }

    // 如果队列为空或停止分发，则停止定时器
    if (m_dataQueue.isEmpty() || !m_isDispatching) m_dataDispatchTimer->stop();
}
