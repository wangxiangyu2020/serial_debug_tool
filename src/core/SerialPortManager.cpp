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

SerialPortManager::SerialPortManager(QObject* parent)
    : QObject(parent), m_pSerialPort(new QSerialPort(this))
{
    this->connectSignals();
    ThreadPoolManager::addTask([this]()
    {
        for (int i = 0; i < 100; ++i)
        {
            QByteArray showByteArray = QString("SerialPortManager initialized: %1").arg(i).toUtf8();
            this->handleReadData(showByteArray);
            QThread::msleep(100);
        }
    });
}

QSerialPort* SerialPortManager::getSerialPort() const
{
    return m_pSerialPort;
}

void SerialPortManager::setHexSendStatus(bool status)
{
    m_isHexSend = status;
}

void SerialPortManager::setSendStringDisplayStatus(bool status)
{
    m_isSendStringDisplay = status;
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
        QString timestamp = QDateTime::currentDateTime().toString("[HH:mm:ss.zzz] ");
        QString formattedData = QString::fromUtf8(writeByteArray);
        QByteArray showByteArray = (timestamp + formattedData).toUtf8();
        emit sigSendData2Receive(showByteArray);
    }
    emit sigSendData(m_isHexSend ? writeByteArray.toHex() : writeByteArray);
}

void SerialPortManager::handleReadData(const QByteArray& readByteArray)
{
    const bool isHex = m_isHexDisplay.load(std::memory_order_acquire);
    // 获取当前时间戳
    QString timestamp = QDateTime::currentDateTime().toString("[HH:mm:ss.zzz] ");
    QString formattedData = isHex
                                ? QString::fromLatin1(readByteArray.toHex(' ').toUpper())
                                : QString::fromUtf8(readByteArray);
    QByteArray showByteArray = (timestamp + formattedData).toUtf8();
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

void SerialPortManager::connectSignals()
{
    SerialPortManager* manager = SerialPortConnectConfigWidget::getSerialPortManager();
    if (!manager)
        return;
    this->connect(m_pSerialPort, &QSerialPort::readyRead, this, &SerialPortManager::onReadyRead);
    this->connect(manager, &SerialPortManager::sigHexDisplay, [this](bool isHex)
    {
        m_isHexDisplay.store(isHex, std::memory_order_release);
    });
    this->connect(manager, &SerialPortManager::sigHexSend, [manager](bool isHex)
    {
        manager->setHexSendStatus(isHex);
    });
    this->connect(manager, &SerialPortManager::sigSendData, [this](const QByteArray& data)
    {
        this->serialPortWrite(data);
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
            QByteArray sendByteArray = SerialPortDataSendWidget::getSendTextEdit()->toPlainText().toLocal8Bit();
            manager->handleWriteData(sendByteArray);
        });
        // 启动定时器，间隔为interval秒转换为毫秒
        int intervalMs = static_cast<int>(interval * 1000);
        timedSendTimer->start(intervalMs);
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
    qDebug() << "SerialPortManager::serialPortWrite" << data;
    qint64 bytesWritten = m_pSerialPort->write(data);
    if (bytesWritten == -1)
    {
        this->handlerError(QSerialPort::WriteError);
    }
}

void SerialPortManager::serialPortRead()
{
    if (!m_pSerialPort || !m_pSerialPort->isOpen())
        return;
    auto readByteArray = m_pSerialPort->readAll();
    if (readByteArray.isEmpty())
        return;
    ThreadPoolManager::addTask([this, readByteArray]()
    {
        QMutexLocker locker(&m_serialMutex);
        // 处理读取到的数据
        qDebug() << "Read: " << readByteArray;
        this->handleReadData(readByteArray);
    });
}

void SerialPortManager::onReadyRead()
{
    this->serialPortRead();
}
