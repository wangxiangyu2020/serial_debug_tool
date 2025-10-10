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

void ModbusController::writeSingleRegister(int slaveId, int startAddress, quint16 value)
{
    if (!m_pSerialPortManager || !m_pSerialPortManager->getSerialPort()->isOpen())
    {
        emit errorOccurred("串口未打开");
        return;
    }

    if (m_pResponseTimer->isActive()) m_pResponseTimer->stop();
    m_lastRequestSlaveId = slaveId;
    m_lastRequestFuncCode = 0x06; // 功能码设为0x06
    m_lastRequestAddress = startAddress;
    // 构造请求
    QByteArray request = this->buildWriteSingleRequest(slaveId, startAddress, value);
    m_pSerialPortManager->handleWriteDataFromModbus(request);
    m_pResponseTimer->start(1000); // 启动1秒超时定时器
}

void ModbusController::writeMultipleRegisters(int slaveId, int startAddress, const QList<quint16>& values)
{
    if (!m_pSerialPortManager || !m_pSerialPortManager->getSerialPort()->isOpen())
    {
        emit errorOccurred("串口未打开");
        return;
    }

    if (m_pResponseTimer->isActive()) m_pResponseTimer->stop();
    m_lastRequestSlaveId = slaveId;
    m_lastRequestFuncCode = 0x10; // 功能码设为0x10
    m_lastRequestAddress = startAddress;
    // 构造请求
    QByteArray request = this->buildWriteMultipleRegisters(slaveId, startAddress, values);
    m_pSerialPortManager->handleWriteDataFromModbus(request);
    m_pResponseTimer->start(1000); // 启动1秒超时定时器
}

void ModbusController::onDataReceived(const QByteArray& data)
{
    // 将新收到的数据追加的缓存中
    m_buffer.append(data);
    // 尝试解析缓存中的数据
    this->tryParseBuffer();
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

void ModbusController::connectSignals()
{
    m_pResponseTimer = new QTimer(this);
    m_pResponseTimer->setSingleShot(true); // 定时器只触发一次
    this->connect(m_pResponseTimer, &QTimer::timeout, this, &ModbusController::onResponseTimeout);
    this->connect(m_pSerialPortManager, &SerialPortManager::sendReadData2Modbus, this,
                  &ModbusController::onDataReceived);
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
    quint16 crc = this->calculateCrc(request);
    request.append(static_cast<char>(crc & 0xFF)); // crc 低位在前
    request.append(static_cast<char>((crc >> 8) & 0xFF));
    return request;
}

QByteArray ModbusController::buildWriteSingleRequest(int slaveId, int startAddress, quint16 value)
{
    QByteArray request;
    request.append(static_cast<char>(slaveId)); // 从站地址
    request.append(static_cast<char>(0x06)); // 功能码
    // 寄存器地址
    request.append(static_cast<char>((startAddress >> 8) & 0xFF));
    request.append(static_cast<char>(startAddress & 0xFF));
    // 要写入的值
    request.append(static_cast<char>((value >> 8) & 0xFF));
    request.append(static_cast<char>(value & 0xFF));
    // CRC
    quint16 crc = calculateCrc(request);
    request.append(static_cast<char>(crc & 0xFF));
    request.append(static_cast<char>((crc >> 8) & 0xFF));
    return request;
}

QByteArray ModbusController::buildWriteMultipleRegisters(int slaveId, int startAddress, const QList<quint16>& values)
{
    QByteArray request;
    request.append(static_cast<char>(slaveId)); // 1. 从站地址
    request.append(static_cast<char>(0x10)); // 2. 功能码
    // 起始地址
    request.append(static_cast<char>((startAddress >> 8) & 0xFF));
    request.append(static_cast<char>(startAddress & 0xFF));
    // 寄存器数量
    int quantity = values.size();
    int byteCount = quantity * 2;
    request.append(static_cast<char>((quantity >> 8) & 0xFF));
    request.append(static_cast<char>(quantity & 0xFF));
    // 字节数
    request.append(static_cast<char>(byteCount));
    // 数据负载
    for (quint16 value : values)
    {
        request.append(static_cast<char>((value >> 8) & 0xFF));
        request.append(static_cast<char>(value & 0xFF));
    }
    // CRC
    quint16 crc = calculateCrc(request);
    request.append(static_cast<char>(crc & 0xFF));
    request.append(static_cast<char>((crc >> 8) & 0xFF));
    return request;
}

void ModbusController::tryParseBuffer()
{
    bool processedOneFrame;
    do
    {
        processedOneFrame = false;
        if (m_buffer.length() < 5) break; // 缓冲区数据不足以构成一个最小帧
        int frameLength = -1;
        uint8_t functionCode = m_buffer[1];
        // 根据功能码，判断一个完整帧的预期长度
        if (functionCode & 0x80) // 异常响应
        {
            frameLength = 5;
        }
        else if (functionCode == 0x03) // 读响应
        {
            if (m_buffer.length() < 3) break; // 长度不足以读取字节数
            int byteCount = static_cast<quint8>(m_buffer[2]);
            frameLength = 3 + byteCount + 2;
        }
        else if (functionCode == 0x06 || functionCode == 0x10) // 写响应
        {
            frameLength = 8;
        }
        // 如果数据不足或者功能码未知，则不处理
        if (frameLength == -1)
        {
            // 功能码与我们等待的不匹配，丢弃一个字节尝试重新同步
            m_buffer.remove(0, 1);
            processedOneFrame = true; // 认为处理过，继续循环
            continue;
        }
        if (m_buffer.length() < frameLength) break; // 缓冲区数据不足以构成一个完整帧, 等待更多数据
        // 提取完整帧并交给核心处理器
        QByteArray frame = m_buffer.left(frameLength);
        if (this->processFrame(frame))
        {
            m_buffer.remove(0, frameLength);
            processedOneFrame = true; // 成功处理，继续循环是否还有下一帧
        }
        else
        {
            // 如果处理失败，则丢弃一个字节尝试重新同步
            m_buffer.remove(0, 1);
            processedOneFrame = true;
        }
    }
    while (processedOneFrame);
}

bool ModbusController::processFrame(const QByteArray& frame)
{
    // 收到完整帧，停止定时器
    m_pResponseTimer->stop();
    // CRC校验
    int dataLength = frame.length() - 2;
    quint16 receivedCrc = (static_cast<quint16>(static_cast<quint8>(frame[dataLength + 1])) << 8)
        | static_cast<quint8>(frame[dataLength]);
    if (this->calculateCrc(frame.left(dataLength)) != receivedCrc)
    {
        emit errorOccurred("响应数据CRC校验失败");
        return false;
    }
    // 根据功能码处理数据
    uint8_t functionCode = frame[1];
    if (functionCode & 0x80) this->handleExceptionResponse(frame);
    else if (functionCode == 0x03) this->handleReadResponse(frame);
    else if (functionCode == 0x06 || functionCode == 0x10) this->handleWriteResponse(frame);
    else return false;
    m_lastRequestSlaveId = -1;
    return true;
}

void ModbusController::handleReadResponse(const QByteArray& frame)
{
    int byteCount = static_cast<quint8>(frame[2]);
    QList<quint16> values;
    for (int i = 0; i < byteCount / 2; ++i)
    {
        quint8 highByte = frame[3 + 2 * i];
        quint8 lowByte = frame[3 + 2 * i + 1];
        values.append((highByte << 8) | lowByte);
    }
    emit dataReady(m_lastRequestAddress, values);
}

void ModbusController::handleWriteResponse(const QByteArray& frame)
{
    uint16_t respAddress = (static_cast<quint8>(frame[2]) << 8) | static_cast<quint8>(frame[3]);
    if (respAddress != m_lastRequestAddress)
    {
        emit errorOccurred("写入响应的地址与请求不匹配");
        return;
    }
    emit writeSuccessful(frame[1], m_lastRequestAddress + 40001);
}

void ModbusController::handleExceptionResponse(const QByteArray& frame)
{
    if ((frame[1] & 0x7F) == m_lastRequestFuncCode)
    {
        int exceptionCode = static_cast<quint8>(frame[2]);
        emit errorOccurred(QString("Modbus异常: 功能码 0x%1, 异常码 0x%2")
                           .arg(m_lastRequestFuncCode, 2, 16, QChar('0')).toUpper()
                           .arg(exceptionCode, 2, 16, QChar('0')).toUpper());
    }
}

quint16 ModbusController::calculateCrc(const QByteArray& data)
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
