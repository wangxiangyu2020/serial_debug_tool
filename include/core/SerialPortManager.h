/**
  ******************************************************************************
  * @file           : SerialPortManager.h
  * @author         : wangxiangyu
  * @brief          : None
  * @attention      : None
  * @date           : 2025/7/21
  ******************************************************************************
  */


#ifndef SERIALPORTMANAGER_H
#define SERIALPORTMANAGER_H

#include <QObject>
#include <QMap>
#include <QVariant>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QThread>
#include "ui/CMessageBox.h"
#include "utils/ThreadPoolManager.h"
#include <QMutex>
#include <functional>
#include <QDateTime>
#include <atomic>  // 添加原子操作头文件

using WriteCallback = std::function<void(QSerialPort::SerialPortError)>;

class SerialPortManager : public QObject
{
    Q_OBJECT

public:
    explicit SerialPortManager(QObject* parent = nullptr);
    ~SerialPortManager() = default;

    QSerialPort* getSerialPort() const;
    void setHexSendStatus(bool status);
    void setHexDisplayStatus(bool status);
    void setSendStringDisplayStatus(bool status);
    bool openSerialPort(const QMap<QString, QVariant>& serialParams);
    bool closeSerialPort();
    void handleWriteData(const QByteArray& writeByteArray);
    void handleReadData(const QByteArray& readByteArray);
    static void handlerError(QSerialPort::SerialPortError error);

private:
    void connectSignals();
    void configureSerialPort(const QMap<QString, QVariant>& serialParams);
    void serialPortWrite(const QByteArray& data);
    void serialPortRead();

private slots:
    void onReadyRead();

signals:
    void sigReceiveData(const QByteArray& data);
    void sigHexDisplay(bool isHex);
    void sigSendData(const QByteArray& data);
    void sigHexSend(bool isHex);
    void sigSendStringDisplay(bool isDisplay);
    void sigSendData2Receive(const QByteArray& data);
    void sigTimedSend(bool isTimed, double interval);

private:
    QSerialPort* m_pSerialPort = nullptr;
    QMutex m_serialMutex; // 保护串口操作
    std::atomic_bool m_isHexDisplay{false}; // 原子标志
    bool m_isHexSend = false;
    bool m_isSendStringDisplay = false;
};

#endif //SERIALPORTMANAGER_H
