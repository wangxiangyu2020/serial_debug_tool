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

class SerialPortManager : public QObject
{
    Q_OBJECT

public:
    explicit SerialPortManager(QObject* parent = nullptr);
    ~SerialPortManager() = default;

    bool openSerialPort(const QMap<QString, QVariant>& serialParams);
    bool closeSerialPort();

private:
    void configureSerialPort(const QMap<QString, QVariant>& serialParams);
    void handlerError(QSerialPort::SerialPortError error);

private:
    QSerialPort* m_pSerialPort = nullptr;
};

#endif //SERIALPORTMANAGER_H
