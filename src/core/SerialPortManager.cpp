/**
  ******************************************************************************
  * @file           : SerialPortManager.cpp
  * @author         : wangxiangyu
  * @brief          : None
  * @attention      : None
  * @date           : 2025/7/21
  ******************************************************************************
  */
#include "core/SerialPortManager.h"

#include "ui/SerialPortConnectConfigWidget.h"
#include "ui/SerialPortDataSendWidget.h"
#include "ui/SerialPortSendSettingsWidget.h"

// 静态成员定义
SerialPortManager* SerialPortManager::m_pInstance = nullptr;
QMutex SerialPortManager::m_mutex;

// 静态工厂方法/单例方法
SerialPortManager* SerialPortManager::getInstance()
{
    if (m_pInstance == nullptr)
    {
        QMutexLocker locker(&m_mutex);
        if (m_pInstance == nullptr) m_pInstance = new SerialPortManager();
    }
    return m_pInstance;
}

QSerialPort* SerialPortManager::getSerialPort() const
{
    return m_pSerialPort;
}

bool SerialPortManager::isHexDisplayEnabled()
{
    return m_isHexDisplay;
}

bool SerialPortManager::isTimestampEnabled()
{
    return m_isDisplayTimestamp;
}

// 数据处理方法
void SerialPortManager::handleWriteData(const QByteArray& writeByteArray)
{
    if (m_isSendStringDisplay)
    {
        QByteArray showByteArray = m_isDisplayTimestamp
                                       ? this->generateTimestamp(QString::fromUtf8(writeByteArray))
                                       : writeByteArray;
        emit sendData2ReceiveChanged(showByteArray);
    }
    this->serialPortWrite(m_isHexSend ? writeByteArray.toHex() : writeByteArray);
}

// 主要业务方法
void SerialPortManager::openSerialPort(const QMap<QString, QVariant>& serialParams)
{
    this->configureSerialPort(serialParams);
    if (!m_pSerialPort->open(QIODevice::ReadWrite))
    {
        this->handlerError(m_pSerialPort->error());
        return;
    }
    emit statusChanged(tr("串口已打开"), ConnectStatus::Connected);
}

void SerialPortManager::closeSerialPort()
{
    if (!m_pSerialPort->isOpen()) return;
    m_pSerialPort->close();
    emit statusChanged(tr("串口已关闭"), ConnectStatus::Disconnected);
}

// 配置方法
void SerialPortManager::setHexSendStatus(bool status)
{
    m_isHexSend = status;
}

void SerialPortManager::setHexDisplayStatus(bool status)
{
    m_isHexDisplay = status;
}

void SerialPortManager::setSendStringDisplayStatus(bool status)
{
    m_isSendStringDisplay = status;
}

void SerialPortManager::setTimestampStatus(bool status)
{
    m_isDisplayTimestamp = status;
}

void SerialPortManager::startTimedSend(double interval, const QByteArray& data)
{
    this->stopTimedSend();

    // 保存循环发送的数据
    m_timedSendData = data;

    // 创建并配置新的定时器
    m_pTimedSendTimer = new QTimer(this);
    this->connect(m_pTimedSendTimer, &QTimer::timeout, this, [this]()
    {
        // 定时器触发时，只做一件事：发送已保存的数据
        // 注意：这里不再需要 sender()，也不再访问任何UI元素
        if (!m_timedSendData.isEmpty()) this->handleWriteData(m_timedSendData);
    }, Qt::QueuedConnection);

    int intervalMs = static_cast<int>(interval * 1000);
    m_pTimedSendTimer->start(intervalMs);
}

void SerialPortManager::stopTimedSend()
{
    if (m_pTimedSendTimer)
    {
        m_pTimedSendTimer->stop();
        m_pTimedSendTimer->deleteLater(); // 使用 deleteLater 安全删除
        m_pTimedSendTimer = nullptr;
        m_timedSendData.clear();
    }
}

void SerialPortManager::onSerialPortRead()
{
    if (!m_pSerialPort || !m_pSerialPort->isOpen()) return;
    m_readBuffer.append(m_pSerialPort->readAll());
    // 如果数据持续快速地到来，这个定时器会不断被重置，停止永远不会触发。
    m_pReadTimer->start();
}

void SerialPortManager::onReadBufferTimeout()
{
    if (m_readBuffer.isEmpty()) return;
    // 暂停已经发生，我们认为缓冲区里是一个完整的数据包
    DataPacket packet;
    packet.sourceInfo = m_pSerialPort->portName();
    packet.data = m_readBuffer; // 使用整个缓冲区的数据
    // 【关键】处理完后，清空缓冲区，为下一条消息做准备
    m_readBuffer.clear();
    PacketProcessor::getInstance()->enqueueData(packet);
}

// 构造函数
SerialPortManager::SerialPortManager(QObject* parent)
    : QObject(parent), m_pSerialPort(new QSerialPort(this)),
      m_pTimedSendTimer(nullptr)
{
    // 初始化定时器
    m_pReadTimer = new QTimer(this);
    m_pReadTimer->setSingleShot(true); // 设置为单次触发
    m_pReadTimer->setInterval(20); // 设置20毫秒的超时，可根据实际情况调整
    this->connectSignals();
}

// 私有方法
void SerialPortManager::connectSignals()
{
    this->connect(m_pReadTimer, &QTimer::timeout, this, &SerialPortManager::onReadBufferTimeout);
    this->connect(ChannelManager::getInstance(), &ChannelManager::channelDataProcessRequested, [this](bool status)
    {
        if (status) this->startWaveformRecording();
        m_isChannelDataProcess = status;
    });
    this->connect(ChannelManager::getInstance(), &ChannelManager::channelsDataAllClearedRequested, [this]()
    {
        this->clearAllChannelData();
    });
    this->connect(this->getSerialPort(), &QSerialPort::readyRead, this, &SerialPortManager::onSerialPortRead);
}

void SerialPortManager::configureSerialPort(const QMap<QString, QVariant>& serialParams)
{
    const auto portInfo = serialParams.value("portInfo").value<QSerialPortInfo>();
    const auto baudRate = serialParams.value("baudRate").value<QSerialPort::BaudRate>();
    const auto dataBits = serialParams.value("dataBits").value<QSerialPort::DataBits>();
    const auto stopBits = serialParams.value("stopBits").value<QSerialPort::StopBits>();
    const auto parity = serialParams.value("parity").value<QSerialPort::Parity>();
    const auto flowControl = serialParams.value("flowControl").value<QSerialPort::FlowControl>();

    m_pSerialPort->setPortName(portInfo.portName());
    m_pSerialPort->setBaudRate(baudRate);
    m_pSerialPort->setDataBits(dataBits);
    m_pSerialPort->setParity(parity);
    m_pSerialPort->setStopBits(stopBits);
    m_pSerialPort->setFlowControl(flowControl);
    m_pSerialPort->setReadBufferSize(1024 * 1024);
}

void SerialPortManager::serialPortWrite(const QByteArray& data)
{
    qint64 bytesWritten = m_pSerialPort->write(data);
    if (bytesWritten == -1)
    {
        this->handlerError(QSerialPort::WriteError);
    }
}

void SerialPortManager::handleChannelData(const QByteArray& data)
{
    ThreadPoolManager::addTask([this, data]()
    {
        QMutexLocker locker(&m_channelMutex);
        for (char c : data)
        {
            m_dataQueue.enqueue(c);
        }
        this->processQueueInternal();
    });
}

void SerialPortManager::startWaveformRecording()
{
    ChannelManager* manager = ChannelManager::getInstance();
    m_channelManager = manager;

    // 使用 invokeMethod 确保在 ChannelManager 的线程中获取数据
    QMetaObject::invokeMethod(manager, [this, manager]()
    {
        // 在 ChannelManager 线程中安全获取数据
        int sampleRate = manager->getSampleRate();
        QList<ChannelInfo> channels = manager->getAllChannels();

        // 切换回 SerialPortManager 线程处理结果
        QMetaObject::invokeMethod(this, [this, sampleRate, channels]()
        {
            this->clearAllChannelData();
            // 在 SerialPortManager 线程中处理数据
            m_sampleRate = static_cast<double>(sampleRate);
            for (const auto& channel : channels)
            {
                m_channelIds.insert(channel.id);
            }
        }, Qt::QueuedConnection);
    }, Qt::QueuedConnection);
}

void SerialPortManager::handlerError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::NoError) return;
    QString errorMsg;
    switch (error)
    {
    case QSerialPort::DeviceNotFoundError:
        errorMsg = tr("设备未找到");
        break;
    case QSerialPort::PermissionError:
        errorMsg = tr("没有权限访问设备或已被占用");
        break;
    case QSerialPort::OpenError:
        errorMsg = tr("设备已打开");
        break;
    case QSerialPort::NotOpenError:
        errorMsg = tr("设备未打开");
        break;
    case QSerialPort::WriteError:
        errorMsg = tr("写入错误");
        break;
    case QSerialPort::ReadError:
        errorMsg = tr("读取错误");
        break;
    case QSerialPort::ResourceError:
        errorMsg = tr("资源错误，设备可能已断开连接");
        break;
    case QSerialPort::UnsupportedOperationError:
        errorMsg = tr("不支持的操作");
        break;
    case QSerialPort::TimeoutError:
        errorMsg = tr("操作超时");
        break;
    default:
        errorMsg = tr("未知错误: %1").arg(error);
    }
    emit statusChanged(errorMsg, ConnectStatus::Disconnected);
}

QByteArray& SerialPortManager::generateTimestamp(const QString& data)
{
    static QString timestamp;
    static QByteArray array;
    timestamp = QDateTime::currentDateTime().toString("[HH:mm:ss.zzz] ");
    array = (timestamp + data).toUtf8();
    return array;
}

void SerialPortManager::clearAllChannelData()
{
    m_channelIds.clear();
    m_channelTimestamps.clear();
    m_dataQueue.clear();
    m_currentPoint.clear();
    m_lastTimestamp = 0;
}

void SerialPortManager::processQueueInternal()
{
    if (m_dataQueue.size() > MAX_QUEUE_SIZE)
    {
        m_dataQueue.clear();
        m_currentPoint.clear();
        return;
    }
    while (!m_dataQueue.isEmpty())
    {
        char ch = m_dataQueue.dequeue();
        if (ch == ',')
        {
            if (!m_currentPoint.isEmpty())
            {
                this->processDataPointInternal(m_currentPoint);
                m_currentPoint.clear();
            }
            continue;
        }
        m_currentPoint.append(ch);
    }
}

void SerialPortManager::processDataPointInternal(const QByteArray& dataPoint)
{
    int eqPos = dataPoint.indexOf('=');
    if (eqPos == -1) return;
    QByteArray channel = dataPoint.left(eqPos).trimmed();
    QByteArray data = dataPoint.mid(eqPos + 1).trimmed();
    QString channelId = QString::fromLatin1(channel);
    if (!m_channelTimestamps.contains(channelId)) m_channelTimestamps[channelId] = m_sampleRate;
    double currentTime = m_channelTimestamps[channelId];
    if (!m_channelIds.contains(channelId)) return;
    bool ok;
    double value = data.toDouble(&ok);
    if (!ok) return;
    QVariantList point;
    point.reserve(2);
    point << currentTime << value;
    m_channelTimestamps[channelId] += m_sampleRate;
    // 使用函数指针方式，更安全且性能更好
    QMetaObject::invokeMethod(m_channelManager,
                              [this, channelId, point]()
                              {
                                  m_channelManager->addChannelData(channelId, QVariant(std::move(point)));
                              }, Qt::QueuedConnection);
}
