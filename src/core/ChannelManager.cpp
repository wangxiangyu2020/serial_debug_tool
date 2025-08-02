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

ChannelManager* ChannelManager::m_instance = nullptr;
QMutex ChannelManager::m_mutex;

ChannelManager::ChannelManager(QObject* parent)
    : QObject(parent)
{
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
    // 发射信号通知数据更新
    emit channelDataAdded(channelId, std::move(data));
}

