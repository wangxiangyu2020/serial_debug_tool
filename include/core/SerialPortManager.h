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
#include "core/ChannelManager.h"

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
    void setTimestampStatus(bool status);
    void setChannelDataProcess(bool status);
    bool getChannelDataProcess();
    bool openSerialPort(const QMap<QString, QVariant>& serialParams);
    bool closeSerialPort();
    void handleWriteData(const QByteArray& writeByteArray);
    void handleReadData(const QByteArray& readByteArray);
    static void handlerError(QSerialPort::SerialPortError error);
    void handleChannelData(const QByteArray& readByteArray);
    void startWaveformRecording();

private:
    void connectSignals();
    void configureSerialPort(const QMap<QString, QVariant>& serialParams);
    void serialPortWrite(const QByteArray& data);
    void serialPortRead();
    QByteArray& generateTimestamp(const QString& data);
    void clearAllChannelData();
    void processDataPointInternal(const QByteArray& bytes);
    void processQueueInternal();

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
    void sigDisplayTimestamp(bool isDisplay);

private:
    QSerialPort* m_pSerialPort = nullptr;
    QMutex m_serialMutex; // 保护串口操作
    std::atomic_bool m_isHexDisplay{false}; // 原子标志
    bool m_isHexSend = false;
    bool m_isSendStringDisplay = false;
    bool m_isDisplayTimestamp = false;
    bool m_isChannelDataProcess = false;
    // 通道数据相关
    static constexpr int MAX_QUEUE_SIZE = 4096; // 队列最大尺寸
    mutable QMutex m_channelMutex;
    QQueue<char> m_dataQueue; // 数据队列
    QSet<QString> m_channelIds; // 有效通道ID集合
    QByteArray m_currentPoint; // 当前正在构建的数据点
    double m_sampleRate = 0;
    double m_lastTimestamp = 0; // 上一次采集的时间
    ChannelManager* m_channelManager = nullptr;
};

#endif //SERIALPORTMANAGER_H
