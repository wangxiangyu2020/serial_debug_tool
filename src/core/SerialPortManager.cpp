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

SerialPortManager::SerialPortManager(QObject* parent)
    : QObject(parent), m_pSerialPort(new QSerialPort(this))
{
    this->connectSignals();
}

QSerialPort* SerialPortManager::getSerialPort() const
{
    return m_pSerialPort;
}

void SerialPortManager::setHexSendStatus(bool status)
{
    m_isHexSend = status;
}

void SerialPortManager::setHexDisplayStatus(bool status)
{
    m_isHexDisplay.store(status, std::memory_order_release);
}

void SerialPortManager::setSendStringDisplayStatus(bool status)
{
    m_isSendStringDisplay = status;
}

void SerialPortManager::setTimestampStatus(bool status)
{
    m_isDisplayTimestamp = status;
}

void SerialPortManager::setChannelDataProcess(bool status)
{
    m_isChannelDataProcess = status;
}

bool SerialPortManager::getChannelDataProcess()
{
    return m_isChannelDataProcess;
}

bool SerialPortManager::openSerialPort(const QMap<QString, QVariant>& serialParams)
{
    QMutexLocker locker(&m_serialMutex);
    if (m_pSerialPort->isOpen())
    {
        this->closeSerialPort();
        QThread::msleep(100);
    }
    // 串口配置
    this->configureSerialPort(serialParams);
    // 尝试打开串口
    if (!m_pSerialPort->open(QIODevice::ReadWrite))
    {
        this->handlerError(m_pSerialPort->error());
        return false;
    }
    return true;
}

bool SerialPortManager::closeSerialPort()
{
    QMutexLocker locker(&m_serialMutex);
    if (m_pSerialPort->isOpen())
    {
        m_pSerialPort->close();
        return true;
    }
    return false;
}

void SerialPortManager::handleWriteData(const QByteArray& writeByteArray)
{
    if (m_isSendStringDisplay)
    {
        QByteArray showByteArray = m_isDisplayTimestamp
                                       ? this->generateTimestamp(QString::fromUtf8(writeByteArray))
                                       : writeByteArray;
        emit sigSendData2Receive(showByteArray);
    }
    emit sigSendData(m_isHexSend ? writeByteArray.toHex() : writeByteArray);
}

void SerialPortManager::handleReadData(const QByteArray& readByteArray)
{
    const bool isHex = m_isHexDisplay.load(std::memory_order_acquire);
    QString formattedData = isHex
                                ? QString::fromLatin1(readByteArray.toHex(' ').toUpper())
                                : QString::fromUtf8(readByteArray);
    QByteArray showByteArray = m_isDisplayTimestamp ? this->generateTimestamp(formattedData) : formattedData.toUtf8();
    emit sigReceiveData(showByteArray);
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
    CMessageBox::showToast(errorMsg);
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
    QMutexLocker locker(&m_channelMutex);
    m_channelManager = ChannelManager::getInstance();
    m_sampleRate = static_cast<double>(m_channelManager->getSampleRate());
    this->clearAllChannelData();
    ChannelManager* manager = ChannelManager::getInstance();
    QList<ChannelInfo> channels = manager->getAllChannels();
    for (const auto& channel : channels)
    {
        m_channelIds.insert(channel.id);
    }
}

void SerialPortManager::connectSignals()
{
    SerialPortManager* manager = SerialPortConnectConfigWidget::getSerialPortManager();
    ChannelManager* channelManager = ChannelManager::getInstance();
    if (!manager) return;
    this->connect(manager->getSerialPort(), &QSerialPort::readyRead, manager, &SerialPortManager::onReadyRead);
    this->connect(manager, &SerialPortManager::sigHexDisplay, [manager](bool isHex)
    {
        manager->setHexDisplayStatus(isHex);
    });
    this->connect(manager, &SerialPortManager::sigHexSend, [manager](bool isHex)
    {
        manager->setHexSendStatus(isHex);
    });
    this->connect(manager, &SerialPortManager::sigSendData, [manager](const QByteArray& data)
    {
        manager->serialPortWrite(data);
    });
    this->connect(manager, &SerialPortManager::sigSendStringDisplay, [manager](bool isDisplay)
    {
        manager->setSendStringDisplayStatus(isDisplay);
    });
    this->connect(manager, &SerialPortManager::sigTimedSend, [this, manager](bool isTimed, double interval)
    {
        static QPointer<QTimer> timedSendTimer;
        if (timedSendTimer)
        {
            timedSendTimer->stop();
            timedSendTimer->deleteLater();
        }
        if (!isTimed)
            return;
        // 创建新的定时器
        timedSendTimer = new QTimer(this);
        connect(timedSendTimer, &QTimer::timeout, [manager]()
        {
            if (!manager->getSerialPort()->isOpen())
            {
                SerialPortSendSettingsWidget::getTimedSendCheckBox()->setChecked(false);
                timedSendTimer->stop();
                return;
            }
            QByteArray sendByteArray = SerialPortDataSendWidget::getSendTextEdit()->toPlainText().toLocal8Bit();
            manager->handleWriteData(sendByteArray);
        });
        // 启动定时器，间隔为interval秒转换为毫秒
        int intervalMs = static_cast<int>(interval * 1000);
        timedSendTimer->start(intervalMs);
    });
    this->connect(manager, &SerialPortManager::sigDisplayTimestamp, [manager](bool isDisplay)
    {
        manager->setTimestampStatus(isDisplay);
    });
    this->connect(channelManager, &ChannelManager::channelDataProcess, [manager](bool status)
    {
        if (status) manager->startWaveformRecording();
        manager->setChannelDataProcess(status);
    });
    this->connect(channelManager, &ChannelManager::channelsDataAllCleared, [manager]()
    {
        manager->clearAllChannelData();
    });
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
    m_pSerialPort->setReadBufferSize(1024 * 1024); // 1MB缓冲区
}

void SerialPortManager::serialPortWrite(const QByteArray& data)
{
    qint64 bytesWritten = m_pSerialPort->write(data);
    if (bytesWritten == -1)
    {
        this->handlerError(QSerialPort::WriteError);
    }
}

void SerialPortManager::serialPortRead()
{
    if (!m_pSerialPort || !m_pSerialPort->isOpen()) return;
    auto readByteArray = m_pSerialPort->readAll();
    if (readByteArray.isEmpty()) return;
    ThreadPoolManager::addTask([this, readByteArray]()
    {
        QMutexLocker locker(&m_serialMutex);
        if (this->getChannelDataProcess())
            this->handleChannelData(readByteArray);
        // 处理读取到的数据
        this->handleReadData(readByteArray);
    });
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
    m_channelManager->addChannelData(channelId, QVariant(std::move(point)));
}

void SerialPortManager::processQueueInternal()
{
    // 添加一个保护性检查，避免队列过大导致内存问题
    if (m_dataQueue.size() > MAX_QUEUE_SIZE)
    {
        m_dataQueue.clear();
        m_currentPoint.clear();
        return;
    }
    while (!m_dataQueue.isEmpty())
    {
        char ch = m_dataQueue.dequeue();
        // 遇到分隔符时处理数据点
        if (ch == ',')
        {
            if (!m_currentPoint.isEmpty())
            {
                this->processDataPointInternal(m_currentPoint);
                m_currentPoint.clear();
            }
            continue;
        }
        // 普通字符添加到当前数据点
        m_currentPoint.append(ch);
    }
}

void SerialPortManager::onReadyRead()
{
    this->serialPortRead();
}
