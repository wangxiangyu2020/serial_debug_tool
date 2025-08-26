/**
  ******************************************************************************
  * @file           : PacketProcessor.cpp
  * @author         : wangxiangyu
  * @brief          : None
  * @attention      : None
  * @date           : 2025/8/20
  ******************************************************************************
  */

#include "utils/PacketProcessor.h"


PacketProcessor* PacketProcessor::m_instance = nullptr;
QMutex PacketProcessor::m_instanceMutex;

PacketProcessor* PacketProcessor::getInstance()
{
    if (m_instance == nullptr)
    {
        QMutexLocker locker(&m_instanceMutex);
        if (m_instance == nullptr) m_instance = new PacketProcessor;
    }
    return m_instance;
}

void PacketProcessor::enqueueData(const DataPacket& packet)
{
    QMutexLocker locker(&m_mutex);
    m_dataQueue.enqueue(packet);
    m_condition.wakeOne();
}

void PacketProcessor::run()
{
    while (!m_quit)
    {
        m_mutex.lock();
        if (m_dataQueue.isEmpty()) m_condition.wait(&m_mutex);
        if (m_quit)
        {
            m_mutex.unlock();
            break;
        }
        QQueue<DataPacket> localQueue;
        localQueue.swap(m_dataQueue);
        m_mutex.unlock();
        while (!localQueue.isEmpty())
        {
            DataPacket packet = localQueue.dequeue();
            if (packet.sourceInfo.startsWith("COM") || packet.sourceInfo.startsWith("tty"))
            {
                this->processSerialData(packet);
            }
            else if (packet.sourceInfo.contains(":"))
            {
                this->processTcpData(packet);
            }
            else
            {
                qWarning() << "Invalid source info: " << packet.sourceInfo;
            }
        }
    }
    qDebug() << "PacketProcessor thread exited";
}

PacketProcessor::PacketProcessor(QObject* parent) : QThread(parent)
{
    this->connect(ChannelManager::getInstance(), &ChannelManager::channelsDataAllClearedRequested, [this]
    {
        m_channelTimestamps.clear();
        m_serialWaveformBuffer.clear();
    });
}

PacketProcessor::~PacketProcessor()
{
    m_mutex.lock();
    m_quit = true;
    m_mutex.unlock();
    m_condition.wakeAll();
    wait();
}

void PacketProcessor::processSerialData(const DataPacket& packet)
{
    // 判断是否使用用户脚本处理数据
    if (ScriptManager::getInstance()->isEnableSerialPortScript()) this->processSerialDataWithScript(packet);
    else this->processSerialDataWithoutScript(packet);
}

void PacketProcessor::processSerialDataWithScript(const DataPacket& packet)
{
    ScriptManager* scriptManager = ScriptManager::getInstance();
    // 限制缓冲区大小，避免内存无限增长
    const int MAX_BUFFER_SIZE = 8192;
    if (m_serialWaveformBuffer.size() + packet.data.size() > MAX_BUFFER_SIZE)
        m_serialWaveformBuffer.clear();
    m_serialWaveformBuffer.append(packet.data);
    if (m_serialWaveformBuffer.isEmpty()) return;
    QJSValue context = scriptManager->getJsEngine()->newObject();
    context.setProperty("source", packet.sourceInfo);
    QJSValue scriptResult = scriptManager->processBuffer("serialPort", m_serialWaveformBuffer, context);
    if (!scriptResult.isObject() || scriptResult.isUndefined() || scriptResult.isNull()) return;
    // 检查返回对象是否包含预期属性
    if (!scriptResult.hasProperty("bytesConsumed") || !scriptResult.hasProperty("frames"))
        return;
    // 根据脚本返回的已处理字节数，更新缓冲区
    int bytesConsumed = scriptResult.property("bytesConsumed").toInt();
    if (bytesConsumed > 0) m_serialWaveformBuffer = m_serialWaveformBuffer.mid(bytesConsumed);
    // 获取所有解析好的帧数组
    QJSValue framesArray = scriptResult.property("frames");
    if (!framesArray.isArray()) return;
    // 获取所需配置
    ChannelManager* chManager = ChannelManager::getInstance();
    const bool isRecording = chManager->isDataRecordingEnabled();
    const QList<ChannelInfo> activeChannels = chManager->getAllChannels();
    const double sampleRate = chManager->getSampleRate();
    const bool isHex = SerialPortManager::getInstance()->isHexDisplayEnabled();
    const bool isTimestamp = SerialPortManager::getInstance()->isTimestampEnabled();

    QHash<QString, QString> idToNameMap;
    QSet<QString> activeChannelIds;
    for (const auto& ch : activeChannels)
    {
        idToNameMap[ch.id] = ch.name;
        activeChannelIds.insert(ch.id);
    }
    // 遍历脚本返回的所有已解析帧
    const int framesCount = framesArray.property("length").toInt();
    for (int i = 0; i < framesCount; ++i)
    {
        QJSValue parsedResult = framesArray.property(i);
        if (!parsedResult.isObject()) continue;
        QVariant resultVariant = parsedResult.toVariant();
        if (!resultVariant.isValid()) continue;
        QJsonObject resultObject = QJsonDocument::fromVariant(resultVariant).object();
        // 处理显示的文本
        if (resultObject.contains("displayText"))
        {
            QString displayText = resultObject.value("displayText").toString();
            QString formattedData = isHex
                                        ? QString::fromLatin1(displayText.toLatin1().toHex(' ').toUpper())
                                        : displayText;
            QString timestampText;
            if (isTimestamp)
            {
                QString timestamp = QDateTime::currentDateTime().toString("[HH:mm:ss.zzz]");
                timestampText = QString("%1 %2").arg(timestamp).arg(formattedData);
            }
            else
            {
                timestampText = formattedData;
            }
            emit serialPortReceiveDataChanged(timestampText.toLocal8Bit());
        }
        // 处理录波数据
        if (isRecording && resultObject.contains("chartData"))
        {
            QJsonObject chartObj = resultObject.value("chartData").toObject();
            QString channelId = chartObj.value("channelId").toString();
            if (activeChannelIds.contains(channelId))
            {
                double value = chartObj.value("point").toDouble();
                QString channelName = idToNameMap.value(channelId);
                if (!m_channelTimestamps.contains(channelId)) m_channelTimestamps[channelId] = 0;
                double currentTime = m_channelTimestamps[channelId];
                m_channelTimestamps[channelId] += sampleRate;
                emit waveformDataReady(channelName, QVariantList{currentTime, value});
            }
        }
    }
}

void PacketProcessor::processSerialDataWithoutScript(const DataPacket& packet)
{
    SerialPortManager* spManager = SerialPortManager::getInstance();
    const bool isHex = spManager->isHexDisplayEnabled();

    QString formattedData = isHex
                                ? QString::fromLatin1(packet.data.toHex(' ').toUpper())
                                : QString::fromUtf8(packet.data);
    QString displayText;
    if (SerialPortManager::getInstance()->isTimestampEnabled())
    {
        QString timestamp = QDateTime::currentDateTime().toString("[HH:mm:ss.zzz]");
        displayText = QString("%1 %2").arg(timestamp).arg(formattedData);
    }
    else
    {
        displayText = formattedData;
    }
    emit serialPortReceiveDataChanged(displayText.toLocal8Bit());
    // 判断是否需要录波
    if (!ChannelManager::getInstance()->isDataRecordingEnabled()) return;
    // a. 将新数据追加到上一次剩下的不完整帧后面
    m_serialWaveformBuffer.append(packet.data);
    // b. 寻找最后一个完整帧的分隔符
    int lastSeparator = m_serialWaveformBuffer.lastIndexOf(',');
    if (lastSeparator == -1) return; //如果连一个分隔符都没有，说明数据还不够一个完整帧，直接返回等待更多数据
    // c. 提取出所有完整的帧进行处理
    QByteArray completeFrames = m_serialWaveformBuffer.left(lastSeparator);
    // d. 将最后一个分隔符之后的不完整部分，作为新的“半成品”存起来
    m_serialWaveformBuffer = m_serialWaveformBuffer.mid(lastSeparator + 1);
    ChannelManager* chManager = ChannelManager::getInstance();
    const QList<ChannelInfo> activeChannels = chManager->getAllChannels();
    const double sampleRate = chManager->getSampleRate();
    QHash<QString, QString> idToNameMap;
    QSet<QString> activeChannelIds;
    for (const auto& ch : activeChannels)
    {
        idToNameMap[ch.id] = ch.name;
        activeChannelIds.insert(ch.id);
    }
    QList<QByteArray> points = completeFrames.split(',');
    for (const QByteArray& dataPoint : points)
    {
        int eqPos = dataPoint.indexOf('=');
        if (eqPos == -1) continue;
        QString channelId = QString::fromLatin1(dataPoint.left(eqPos).trimmed());
        if (!activeChannelIds.contains(channelId)) continue;
        QString channelName = idToNameMap.value(channelId);
        bool ok;
        double value = dataPoint.mid(eqPos + 1).trimmed().toDouble(&ok);
        if (!ok) continue;
        if (!m_channelTimestamps.contains(channelId)) m_channelTimestamps[channelId] = 0;
        double currentTime = m_channelTimestamps[channelId];
        m_channelTimestamps[channelId] += sampleRate;
        QVariantList point;
        point << currentTime << value;
        emit waveformDataReady(channelName, point);
    }
}

void PacketProcessor::processTcpData(const DataPacket& packet)
{
    ScriptManager* scManager = ScriptManager::getInstance();
    if (!scManager->isEnableTcpNetworkClientScript() && !scManager->isEnableTcpNetworkServerScript())
        processTcpDataWithoutScript(packet);
    else
        processTcpDataWithScript(packet);
}

void PacketProcessor::processTcpDataWithScript(const DataPacket& packet)
{
    ScriptManager* scManager = ScriptManager::getInstance();
    const int MAX_BUFFER_SIZE = 8192; // 同样可以为每个客户端设置最大缓存
    // 1. 使用新的 m_tcpClientBuffers，并根据 packet.sourceInfo 操作对应的缓存
    QByteArray& clientBuffer = m_tcpClientBuffers[packet.sourceInfo];
    if (clientBuffer.size() + packet.data.size() > MAX_BUFFER_SIZE) clientBuffer.clear();
    clientBuffer.append(packet.data);
    if (clientBuffer.isEmpty()) return;
    // 2. 创建一个 context 对象，把 sourceInfo 放进去
    QJSValue context = scManager->getJsEngine()->newObject();
    context.setProperty("source", packet.sourceInfo);
    // 3. 调用新的 processBuffer 接口，传入客户端专属的缓存和 context
    QJSValue scriptResult;
    if (scManager->isTcpNetworkClientConnected())
    {
        // 假设客户端脚本名为"client"
        scriptResult = scManager->processBuffer("client", clientBuffer, context);
    }
    else if (scManager->isTcpNetworkServerListen())
    {
        // 假设服务端脚本名为"server"
        scriptResult = scManager->processBuffer("server", clientBuffer, context);
    }
    if (!scriptResult.isObject() || scriptResult.isUndefined() || scriptResult.isNull()) return;
    if (!scriptResult.hasProperty("bytesConsumed") || !scriptResult.hasProperty("frames")) return;
    // 4. 根据脚本返回结果，更新该客户端的缓存
    int bytesConsumed = scriptResult.property("bytesConsumed").toInt();
    if (bytesConsumed > 0) clientBuffer = clientBuffer.mid(bytesConsumed);
    if (clientBuffer.isEmpty()) m_tcpClientBuffers.remove(packet.sourceInfo); // 如果缓存空了，可以移除该条目以节省内存

    QJSValue framesArray = scriptResult.property("frames");
    if (!framesArray.isArray()) return;
    const int framesCount = framesArray.property("length").toInt();
    TcpNetworkManager* tcpManager = TcpNetworkManager::getInstance();
    const bool isHex = tcpManager->isHexDisplayEnabled();
    const bool isTimestamp = tcpManager->isTimestampEnabled();
    for (int i = 0; i < framesCount; ++i)
    {
        QJSValue parsedResult = framesArray.property(i);
        if (!parsedResult.isObject()) continue;
        QVariant resultVariant = parsedResult.toVariant();
        if (!resultVariant.isValid()) continue;
        QJsonObject resultObject = QJsonDocument::fromVariant(resultVariant).object();
        if (resultObject.contains("displayText"))
        {
            QString displayText = resultObject.value("displayText").toString();
            QString formattedData = isHex
                                        ? QString::fromLatin1(displayText.toLatin1().toHex(' ').toUpper())
                                        : displayText;
            QString timestampText;
            if (isTimestamp)
            {
                QString timestamp = QDateTime::currentDateTime().toString("[HH:mm:ss.zzz] ");
                timestampText = timestamp + formattedData;
            }
            else
            {
                timestampText = formattedData;
            }
            emit tcpNetworkReceiveDataChanged(timestampText.toLocal8Bit());
        }
    }
}

void PacketProcessor::processTcpDataWithoutScript(const DataPacket& packet)
{
    TcpNetworkManager* tcpManager = TcpNetworkManager::getInstance();
    // 假设TcpManager也将提供这些状态接口
    const bool isHex = tcpManager->isHexDisplayEnabled();
    const bool addTimestamp = tcpManager->isTimestampEnabled();

    QString formattedData = isHex
                                ? QString::fromLatin1(packet.data.toHex(' ').toUpper())
                                : QString::fromUtf8(packet.data);
    // 在最前面加上传输端信息
    formattedData.prepend("from " + packet.sourceInfo + ": ");
    QString displayText;
    if (addTimestamp)
    {
        QString timestamp = QDateTime::currentDateTime().toString("[HH:mm:ss.zzz] ");
        displayText = timestamp + formattedData;
    }
    else
    {
        displayText = formattedData;
    }
    emit tcpNetworkReceiveDataChanged(displayText.toLocal8Bit());
}
