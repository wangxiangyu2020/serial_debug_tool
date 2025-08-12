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

// 构造函数
SerialPortManager::SerialPortManager(QObject* parent)
    : QObject(parent), m_pSerialPort(new QSerialPort(this))
{
    this->connectSignals();
}

// 主要业务方法
bool SerialPortManager::openSerialPort(const QMap<QString, QVariant>& serialParams)
{
    QMutexLocker locker(&m_serialMutex);
    if (m_pSerialPort->isOpen())
    {
        this->closeSerialPort();
        QThread::msleep(100);
    }
    this->configureSerialPort(serialParams);
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

QSerialPort* SerialPortManager::getSerialPort() const
{
    return m_pSerialPort;
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

// 配置方法
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

// 获取方法
bool SerialPortManager::getChannelDataProcess()
{
    return m_isChannelDataProcess;
}

// 数据处理方法
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

// 静态错误处理
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

// 私有槽函数
void SerialPortManager::onReadyRead()
{
    this->serialPortRead();
}

// 私有方法
void SerialPortManager::connectSignals()
{
    ChannelManager* channelManager = ChannelManager::getInstance();
    this->connect(this->getSerialPort(), &QSerialPort::readyRead, this, &SerialPortManager::onReadyRead);
    this->connect(this, &SerialPortManager::sigHexDisplay, [this](bool isHex)
    {
        this->setHexDisplayStatus(isHex);
    });
    this->connect(this, &SerialPortManager::sigHexSend, [this](bool isHex)
    {
        this->setHexSendStatus(isHex);
    });
    this->connect(this, &SerialPortManager::sigSendData, [this](const QByteArray& data)
    {
        this->serialPortWrite(data);
    });
    this->connect(this, &SerialPortManager::sigSendStringDisplay, [this](bool isDisplay)
    {
        this->setSendStringDisplayStatus(isDisplay);
    });
    this->connect(this, &SerialPortManager::sigTimedSend, [this](bool isTimed, double interval)
    {
        static QPointer<QTimer> timedSendTimer;
        if (timedSendTimer)
        {
            timedSendTimer->stop();
            timedSendTimer->deleteLater();
        }
        if (!isTimed)
            return;
        timedSendTimer = new QTimer(this);
        connect(timedSendTimer, &QTimer::timeout, [this]()
        {
            if (!this->getSerialPort()->isOpen())
            {
                SerialPortSendSettingsWidget::getTimedSendCheckBox()->setChecked(false);
                timedSendTimer->stop();
                return;
            }
            QByteArray sendByteArray = SerialPortDataSendWidget::getSendTextEdit()->toPlainText().toLocal8Bit();
            this->handleWriteData(sendByteArray);
        });
        int intervalMs = static_cast<int>(interval * 1000);
        timedSendTimer->start(intervalMs);
    });
    this->connect(this, &SerialPortManager::sigDisplayTimestamp, [this](bool isDisplay)
    {
        this->setTimestampStatus(isDisplay);
    });
    this->connect(channelManager, &ChannelManager::channelDataProcess, [this](bool status)
    {
        if (status) this->startWaveformRecording();
        this->setChannelDataProcess(status);
    });
    this->connect(channelManager, &ChannelManager::channelsDataAllCleared, [this]()
    {
        this->clearAllChannelData();
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
