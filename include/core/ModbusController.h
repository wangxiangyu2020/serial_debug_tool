/**
  ******************************************************************************
  * @file           : ModbusController.h
  * @author         : wangxiangyu
  * @brief          : None
  * @attention      : None
  * @date           : 2025/9/17
  ******************************************************************************
  */


#ifndef MODBUSCONTROLLER_H
#define MODBUSCONTROLLER_H

#include <QObject>
#include <QTimer>
#include "utils/ModbusTag.h"
#include "core/SerialPortManager.h"

class ModbusController : public QObject
{
    Q_OBJECT

public:
    explicit ModbusController(SerialPortManager* serialPortManager, QObject* parent = nullptr);
    ~ModbusController() = default;
    // ui 公共接口
    void readHoldingRegister(int slaveId, int startAddress, int quantity);
    void writeSingleRegister(int slaveId, int address, int value);

signals:
    void dataReady(int startAddress, const QList<quint16>& values);
    void errorOccurred(const QString& errorString);

private slots:
    void onDataReceived(const QByteArray& data);
    void onResponseTimeout();

private:
    void connectSignals();
    QByteArray buildReadRequest(int slaveId, int startAddress, int quantity);
    QByteArray buildWriteRequest(int slaveId, int address, int value);
    bool parseResponse(const QByteArray& data);
    quint16 calculateCRC(const QByteArray& data);

    SerialPortManager* m_pSerialPortManager = nullptr;
    QByteArray m_buffer;
    QTimer* m_pResponseTimer; // 响应超时定时器
    // 记住上一个请求的状态，用于验证响应
    int m_lastRequestSlaveId = -1;
    int m_lastRequestFuncCode = -1;
    int m_lastRequestAddress = -1;
    int m_lastRequestQuantity = -1;
};

#endif //MODBUSCONTROLLER_H
