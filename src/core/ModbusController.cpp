/**
  ******************************************************************************
  * @file           : ModbusController.cpp
  * @author         : wangxiangyu
  * @brief          : None
  * @attention      : None
  * @date           : 2025/9/17
  ******************************************************************************
  */

#include "core/ModbusController.h"

ModbusController::ModbusController(SerialPortManager* serialPortManager, QObject* parent)
    : QObject(parent), m_pSerialPortManager(serialPortManager)
{
    this->connectSignals();
}

void ModbusController::readHoldingRegister(int slaveId, int startAddress, int quantity)
{
    if (!m_pSerialPortManager || !m_pSerialPortManager->getSerialPort()->isOpen())
    {
        emit errorOccurred("串口未打开");
        return;
    }
    // 如果上一个请求还未收到响应，则认为它已经超时了
    if (m_pResponseTimer->isActive()) m_pResponseTimer->stop();
    // 记录当前请求状态
    m_lastRequestSlaveId = slaveId;
    m_lastRequestFuncCode = 0x03;
    m_lastRequestAddress = startAddress;
    m_lastRequestQuantity = quantity;
    // 构造请求
    QByteArray request = this->buildReadRequest(slaveId, startAddress, quantity);
    m_pSerialPortManager->handleWriteDataFromModbus(request);
    // 设置超时时间为1秒
    m_pResponseTimer->start(1000);
}

void ModbusController::writeSingleRegister(int slaveId, int address, int value)
{
}

void ModbusController::onDataReceived(const QByteArray& data)
{
    qDebug() << "Data Received: " << data;
    // 将新收到的数据追加的缓存中
    m_buffer.append(data);
    // 尝试解析数据
    // 一个Modbus帧至少有5个字节（地址+功能码+字节数/异常码+CRC）
    while (m_buffer.length() >= 5)
    {
        // 判断是否为异常响应 异常响应的第二个字节是功能码 + 0x80
        if (static_cast<quint8>(m_buffer[1]) == (m_lastRequestFuncCode | 0x80))
        {
            if (m_buffer.length() < 5) return;
            // 收到响应停止计时器
            m_pResponseTimer->stop();
            // 提取完整的异常响应数据
            QByteArray response = m_buffer.left(5);
            m_buffer.remove(0, 5);
            // CRC校验
            quint16 receiveCRC = (static_cast<quint8>(response[4] << 8)) | static_cast<quint8>(response[3]);
            if (this->calculateCRC(response.left(3)) != receiveCRC)
            {
                emit errorOccurred(QString("异常响应CRC校验失败"));
                continue; // 继续处理下一个数据包
            }
            // 解析异常码
            int exceptionCode = static_cast<quint8>(response[2]);
            emit errorOccurred(QString("异常响应异常码：%1").arg(exceptionCode));
            m_lastRequestSlaveId = -1;
            continue;
        }
        // 检测是否响应正常 正常响应的功能码和请求的功能码一致
        if (static_cast<quint8>(m_buffer[1]) != m_lastRequestFuncCode)
        {
            // 功能码不匹配丢弃
            m_buffer.remove(0, 1);
            continue;
        }
        // 对于读寄存器响应(0x03)，第3个字节是数据字节数
        int byteCount = static_cast<quint8>(m_buffer[2]);
        // 3(地址+功能码+字节数) + N(数据) + 2(CRC)
        int frameLength = 3 + byteCount + 2;
        if (m_buffer.length() < frameLength) return;
        // 收到完整数据包，停止定时器
        m_pResponseTimer->stop();
        // 提取一个完整的正常响应数据包
        QByteArray response = m_buffer.left(frameLength);
        m_buffer.remove(0, frameLength);
        // CRC 校验
        quint16 receivedCrc = (static_cast<quint8>(response[frameLength - 1]) << 8) | static_cast<quint8>(response[
            frameLength - 2]);
        if (calculateCRC(response.left(frameLength - 2)) != receivedCrc)
        {
            emit errorOccurred("正常响应CRC校验失败");
            continue;
        }
        // 从站地址校验
        if (static_cast<quint8>(response[0]) != m_lastRequestSlaveId)
        {
            emit errorOccurred("响应的从站地址与请求不匹配");
            continue;
        }
        // 数据解析
        QList<quint16> values;
        // 数据从第4个字节（索引为3）开始
        for (int i = 0; i < byteCount / 2; ++i)
        {
            quint8 highByte = response[3 + 2 * i];
            quint8 lowByte = response[3 + 2 * i + 1];
            quint16 value = (highByte << 8) | lowByte;
            values.append(value);
        }
        // 发射信号，将解析出的数据传递出去
        emit dataReady(m_lastRequestAddress, values);
        m_lastRequestSlaveId = -1; // 清理状态，准备下一次请求
    }
}

void ModbusController::onResponseTimeout()
{
    if (m_lastRequestSlaveId != -1)
    {
        // 检查是否真的有一个请求在等待响应
        emit errorOccurred(QString("从站 %1 响应超时").arg(m_lastRequestSlaveId));
        m_lastRequestSlaveId = -1; // 清理状态
    }
}

QByteArray ModbusController::buildReadRequest(int slaveId, int startAddress, int quantity)
{
    QByteArray request;
    request.append(static_cast<char>(slaveId)); // 从站地址
    request.append(static_cast<char>(0x03)); // 功能码
    request.append(static_cast<char>((startAddress >> 8) & 0xFF)); // 起始地址 高位在前
    request.append(static_cast<char>(startAddress & 0xFF));
    request.append(static_cast<char>((quantity >> 8) & 0xFF)); // 读取数量 高位在前
    request.append(static_cast<char>(quantity & 0xFF));
    // CRC校验
    quint16 crc = this->calculateCRC(request);
    request.append(static_cast<char>(crc & 0xFF)); // crc 低位在前
    request.append(static_cast<char>((crc >> 8) & 0xFF));
    qDebug() << "发送数据：" << request.toHex();
    return request;
}

QByteArray ModbusController::buildWriteRequest(int slaveId, int address, int value)
{
    return QByteArray();
}

bool ModbusController::parseResponse(const QByteArray& data)
{
    return true;
}

quint16 ModbusController::calculateCRC(const QByteArray& data)
{
    quint16 crc = 0xFFFF;
    for (char byte : data)
    {
        crc ^= static_cast<quint8>(byte);
        for (int i = 0; i < 8; ++i)
        {
            if (crc & 0x0001)
            {
                crc >>= 1;
                crc ^= 0xA001;
            }
            else
            {
                crc >>= 1;
            }
        }
    }
    return crc;
}

void ModbusController::connectSignals()
{
    m_pResponseTimer = new QTimer(this);
    m_pResponseTimer->setSingleShot(true); // 定时器只触发一次
    this->connect(m_pResponseTimer, &QTimer::timeout, this, &ModbusController::onResponseTimeout);
    this->connect(m_pSerialPortManager, &SerialPortManager::sendReadData2Modbus, this,
                  &ModbusController::onDataReceived);
}
