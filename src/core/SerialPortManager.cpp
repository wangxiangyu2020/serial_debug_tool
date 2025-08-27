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
void SerialPortManager::handleWriteData(const QString& text)
{
    if (m_isSendStringDisplay)
    {
        QString showText = m_isDisplayTimestamp
                               ? this->generateTimestamp(text)
                               : text;
        emit sendData2ReceiveChanged(showText);
    }
    this->serialPortWrite(m_isHexSend ? hexStringToByteArray(text) : text.toLocal8Bit());
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
    m_pReadTimer->start();
    emit statusChanged(tr("串口已打开"), ConnectStatus::Connected);
}

void SerialPortManager::closeSerialPort()
{
    m_pReadTimer->stop();
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

void SerialPortManager::startTimedSend(double interval, const QString& data)
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
    QMutexLocker locker(&m_bufferMutex);
    m_readBuffer.append(m_pSerialPort->readAll());
}

void SerialPortManager::onReadBufferTimeout()
{
    QByteArray dataToProcess;
    {
        // 使用互斥锁保护对缓冲区的读和清空操作
        QMutexLocker locker(&m_bufferMutex);
        if (m_readBuffer.isEmpty()) return;
        // 使用 swap 高效地取出数据，并清空原缓冲区，这比直接赋值更快
        dataToProcess.swap(m_readBuffer);
    } // 互斥锁在这里自动释放
    // 现在可以在无锁状态下，将取出的数据块发送给处理器
    DataPacket packet;
    packet.sourceInfo = m_pSerialPort->portName();
    packet.data = dataToProcess;

    PacketProcessor::getInstance()->enqueueData(packet);
}

void SerialPortManager::onHandleError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError) this->closeSerialPort();
}

// 构造函数
SerialPortManager::SerialPortManager(QObject* parent)
    : QObject(parent), m_pSerialPort(new QSerialPort(this)),
      m_pTimedSendTimer(nullptr)
{
    // 初始化定时器
    m_pReadTimer = new QTimer(this);
    m_pReadTimer->setInterval(20); // 保持20毫秒的“清空”周期
    this->connectSignals();
}

// 私有方法
void SerialPortManager::connectSignals()
{
    this->connect(m_pReadTimer, &QTimer::timeout, this, &SerialPortManager::onReadBufferTimeout);
    this->connect(this->getSerialPort(), &QSerialPort::readyRead, this, &SerialPortManager::onSerialPortRead);
    this->connect(this->getSerialPort(), &QSerialPort::errorOccurred, this, &SerialPortManager::onHandleError);
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

QString SerialPortManager::generateTimestamp(const QString& data)
{
    static QString timestamp;
    timestamp = QDateTime::currentDateTime().toString("[HH:mm:ss.zzz] ");
    return timestamp + data;
}

QByteArray SerialPortManager::hexStringToByteArray(const QString& hexString)
{
    // 移除所有空格
    QString cleanString = hexString;
    cleanString.remove(QChar(' '));
    // 使用 QByteArray 的静态函数从清理后的Hex字符串创建字节数组
    return QByteArray::fromHex(cleanString.toLatin1());
}
