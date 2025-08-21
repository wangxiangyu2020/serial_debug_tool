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
    this->connectSignals();
}

void ChannelManager::connectSignals()
{
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

int ChannelManager::getSampleRate() const
{
    return m_sampleRate;
}

bool ChannelManager::isDataRecordingEnabled()
{
    return m_isChannelDataProcess;
}

void ChannelManager::onClearAllChannelData()
{
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
    m_isChannelDataProcess = true;
    emit statusChanged(QString("数据分发已启动"));
}

void ChannelManager::onStopDataDispatch()
{
    m_isChannelDataProcess = false;
    emit statusChanged(QString("数据分发已停止"));
}
