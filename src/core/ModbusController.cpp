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
    quint16 crc = calculateCRC(request);
    request.append(static_cast<char>(crc & 0xFF));
    request.append(static_cast<char>((crc >> 8) & 0xFF));

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

    int quantity = values.size();
    int byteCount = quantity * 2;

    QByteArray request;
    request.append(static_cast<char>(slaveId)); // 1. 从站地址
    request.append(static_cast<char>(0x10)); // 2. 功能码
    // 起始地址
    request.append(static_cast<char>((startAddress >> 8) & 0xFF));
    request.append(static_cast<char>(startAddress & 0xFF));
    // 寄存器数量
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
    quint16 crc = calculateCRC(request);
    request.append(static_cast<char>(crc & 0xFF));
    request.append(static_cast<char>((crc >> 8) & 0xFF));

    m_pSerialPortManager->handleWriteDataFromModbus(request);

    m_pResponseTimer->start(1000); // 启动1秒超时定时器
}

void ModbusController::onDataReceived(const QByteArray& data)
{
    // 将新收到的数据追加的缓存中
    m_buffer.append(data);
    // 尝试解析数据
    // 一个Modbus帧至少有5个字节（地址+功能码+字节数/异常码+CRC）
    while (m_buffer.length() >= 5)
    {
        int functionCode = static_cast<quint8>(m_buffer[1]);
        // 判断是否为异常响应 异常响应的第二个字节是功能码 + 0x80
        if (functionCode & 0x80)
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
            // 检查异常功能码是否与请求匹配
            if ((functionCode & 0x7F) == m_lastRequestFuncCode)
            {
                int exceptionCode = static_cast<quint8>(response[2]);
                emit errorOccurred(QString("Modbus异常响应: 功能码 %1, 异常码 %2")
                                   .arg(m_lastRequestFuncCode, 2, 16)
                                   .arg(exceptionCode));
            }
            m_lastRequestSlaveId = -1;
            continue;
        }
        // 处理0x03响应
        if (functionCode == 0x03 && m_lastRequestFuncCode == 0x03)
        {
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
        else if ((functionCode == 0x06 && m_lastRequestFuncCode == 0x06)
            || (functionCode == 0x10 && m_lastRequestFuncCode == 0x10))
        {
            if (m_buffer.length() < 8) return; // 确保至少有8个字节
            m_pResponseTimer->stop();
            QByteArray response = m_buffer.left(8);
            m_buffer.remove(0, 8);
            // CRC 校验
            quint16 receivedCrc = (static_cast<quint8>(response[7] << 8)) | static_cast<quint8>(response[6]);
            if (this->calculateCRC(response.left(6)) != receivedCrc)
            {
                emit errorOccurred(QString("写入响应CRC校验失败"));
                continue;
            }
            // 校验从站地址和功能码是否匹配
            if (static_cast<quint8>(response[0]) != m_lastRequestSlaveId ||
                static_cast<quint8>(response[1]) != m_lastRequestFuncCode)
            {
                emit errorOccurred(QString("写入响应的从站地址与功能码与请求不匹配"));
                continue;
            }
            // 校验返回的地址和数量/值是否匹配
            uint16_t respAddress = (static_cast<quint8>(response[2]) << 8) | static_cast<quint8>(response[3]);
            // 假设我们之前已将请求的地址存放在 m_lastRequestAddress 中
            if (respAddress != m_lastRequestAddress)
            {
                emit errorOccurred("写入响应的地址与请求不匹配");
                continue;
            }
            // 所有校验通过，发射成功信号
            emit writeSuccessful(functionCode, m_lastRequestAddress + 40001); // 传回UI地址

            m_lastRequestSlaveId = -1; // 清理状态
        }
        else
        {
            // 收到的响应功能码与我们等待的不匹配，可能是上一条指令的延迟响应
            // 为了防止解析错误，我们丢弃一个字节，尝试寻找下一帧的起始
            m_buffer.remove(0, 1);
        }
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
    quint16 crc = this->calculateCRC(request);
    request.append(static_cast<char>(crc & 0xFF)); // crc 低位在前
    request.append(static_cast<char>((crc >> 8) & 0xFF));
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
