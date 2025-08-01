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

bool ChannelManager::addChannel(const QString& name, const QString& id, const QString& color)
{
    QMutexLocker locker(&m_dataMutex);

    if (name.isEmpty() || id.isEmpty() || m_channels.contains(name))
    {
        return false;
    }

    m_channels.insert(name, ChannelInfo(name, id, color));
    emit channelAdded(name, color);
    return true;
}

bool ChannelManager::removeChannel(const QString& name)
{
    QMutexLocker locker(&m_dataMutex);

    if (!m_channels.contains(name))
    {
        return false;
    }

    m_channels.remove(name);
    emit channelRemoved(name);
    return true;
}

bool ChannelManager::updateChannel(const QString& name, const QString& id, const QString& newColor)
{
    QMutexLocker locker(&m_dataMutex);

    if (!m_channels.contains(name))
    {
        return false;
    }

    m_channels[name] = ChannelInfo(name, id, newColor);
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

ChannelInfo ChannelManager::getChannel(const QString& name) const
{
    QMutexLocker locker(&m_dataMutex);

    if (m_channels.contains(name))
    {
        return m_channels.value(name);
    }

    return ChannelInfo();
}

bool ChannelManager::hasChannel(const QString& name) const
{
    QMutexLocker locker(&m_dataMutex);
    return m_channels.contains(name);
}

int ChannelManager::getChannelCount() const
{
    QMutexLocker locker(&m_dataMutex);
    return m_channels.size();
}
