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
    if (m_useUserScript && ScriptManager::getInstance()->isScriptLoaded()) this->processSerialDataWithScript(packet);
    else this->processSerialDataWithoutScript(packet);
}

void PacketProcessor::processSerialDataWithScript(const DataPacket& packet)
{
    ScriptManager* scriptManager = ScriptManager::getInstance();
    // 将新数据追加到缓冲区，进行帧同步
    m_serialWaveformBuffer.append(packet.data);
    // 先获取通道信息
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
    while (true)
    {
        // 请求脚本查找一个完整帧
        int frameEndPos = scriptManager->findFrame(m_serialWaveformBuffer);
        // 脚本告诉我们没有找到一个完整的帧，或者帧结束位置超出缓冲区范围，跳出循环
        if (frameEndPos == -1 || frameEndPos >= m_serialWaveformBuffer.size()) break;
        // 根据脚本返回的位置截取一个完整的帧
        QByteArray completeFrame = m_serialWaveformBuffer.left(frameEndPos + 1);
        m_serialWaveformBuffer = m_serialWaveformBuffer.mid(frameEndPos + 1);
        // 请求脚本解析这个帧
        QJSValue parsedResult = scriptManager->parseFrame(completeFrame);
        // 根据脚本返回的结果决定如何处理
        if (parsedResult.isUndefined() || parsedResult.isNull()) continue;
        SerialPortManager* spManager = SerialPortManager::getInstance();
        const bool isHex = spManager->isHexDisplayEnabled();
        const bool addTimestamp = spManager->isTimestampEnabled();
        // 返回字符串或者对象类型需要进行解析
        if (parsedResult.isString())
        {
            // 对返回的字符串进行处理
            QString formattedData = isHex
                                        ? QString::fromLatin1(parsedResult
                                                              .toString()
                                                              .toLocal8Bit()
                                                              .toHex(' ')
                                                              .toUpper())
                                        : parsedResult.toString();
            this->emitFormattedSerialData(formattedData);
        }
        else if (parsedResult.isObject())
        {
            // 对脚本返回的对象进行解析
            QJsonObject resultObject = QJsonDocument::fromVariant(parsedResult.toVariant()).object();
            // 检查是否有用于显示的文本数据
            if (resultObject.contains("displayText"))
            {
                // 对返回的字符串进行处理
                QString formattedData = isHex
                                            ? QString::fromLatin1(resultObject.value("displayText")
                                                                              .toString()
                                                                              .toLocal8Bit()
                                                                              .toHex(' ')
                                                                              .toUpper())
                                            : resultObject.value("displayText").toString();
                this->emitFormattedSerialData(formattedData);
            }
            // 判断是否需要录波
            if (!ChannelManager::getInstance()->isDataRecordingEnabled()) continue;
            if (resultObject.contains("chartData"))
            {
                QJsonObject chartObj = resultObject.value("chartData").toObject();
                // 假设脚本返回了所有信息
                if (chartObj.contains("channelId") && chartObj.contains("point"))
                {
                    QString channelId = chartObj.value("channelName").toString();
                    double value = chartObj.value("point").toDouble();
                    if (channelId.isEmpty() && qIsNaN(value)) continue;
                    if (!activeChannelIds.contains(channelId)) continue;
                    QString channelName = idToNameMap.value(channelId);
                    if (!m_channelTimestamps.contains(channelId)) m_channelTimestamps[channelId] = 0;
                    double currentTime = m_channelTimestamps[channelId];
                    m_channelTimestamps[channelId] += sampleRate;
                    QVariantList point;
                    point << currentTime << value;
                    emit waveformDataReady(channelName, point);
                }
            }
        }
    }
}

void PacketProcessor::processSerialDataWithoutScript(const DataPacket& packet)
{
    SerialPortManager* spManager = SerialPortManager::getInstance();
    const bool isHex = spManager->isHexDisplayEnabled();
    const bool addTimestamp = spManager->isTimestampEnabled();

    QString formattedData = isHex
                                ? QString::fromLatin1(packet.data.toHex(' ').toUpper())
                                : QString::fromUtf8(packet.data);
    this->emitFormattedSerialData(formattedData);
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

void PacketProcessor::emitFormattedSerialData(const QString& data)
{
    QString displayText;
    if (SerialPortManager::getInstance()->isTimestampEnabled())
    {
        QString timestamp = QDateTime::currentDateTime().toString("[HH:mm:ss.zzz]");
        displayText = QString("%1 %2").arg(timestamp).arg(data);
    }
    else
    {
        displayText = data;
    }
    emit serialPortReceiveDataChanged(displayText.toLocal8Bit());
}

void PacketProcessor::processTcpData(const DataPacket& packet)
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
