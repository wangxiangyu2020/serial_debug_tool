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

SerialPortManager::SerialPortManager(QObject* parent)
    : QObject(parent), m_pSerialPort(new QSerialPort(this))
{
}

bool SerialPortManager::openSerialPort(const QMap<QString, QVariant>& serialParams)
{
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
    if (m_pSerialPort->isOpen())
    {
        m_pSerialPort->close();
        return true;
    }
    return false;
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
