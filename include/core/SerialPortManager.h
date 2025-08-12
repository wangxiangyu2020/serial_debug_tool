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
#include <QMutex>
#include <QDateTime>
#include <QQueue>
#include <QSet>
#include <functional>
#include <atomic>
#include "ui/CMessageBox.h"
#include "utils/ThreadPoolManager.h"
#include "core/ChannelManager.h"

class SerialPortManager : public QObject
{
    Q_OBJECT

public:
    // 静态工厂方法/单例方法
    static SerialPortManager* getInstance();

    // 拷贝控制
    SerialPortManager(const SerialPortManager&) = delete;
    SerialPortManager& operator=(const SerialPortManager&) = delete;

    // 主要业务方法
    bool openSerialPort(const QMap<QString, QVariant>& serialParams);
    bool closeSerialPort();
    QSerialPort* getSerialPort() const;
    void startWaveformRecording();

    // 配置方法
    void setHexSendStatus(bool status);
    void setHexDisplayStatus(bool status);
    void setSendStringDisplayStatus(bool status);
    void setTimestampStatus(bool status);
    void setChannelDataProcess(bool status);

    // 获取方法
    bool getChannelDataProcess();

    // 数据处理方法
    void handleWriteData(const QByteArray& writeByteArray);
    void handleReadData(const QByteArray& readByteArray);
    void handleChannelData(const QByteArray& readByteArray);

    // 静态错误处理
    static void handlerError(QSerialPort::SerialPortError error);

signals:
    void sigReceiveData(const QByteArray& data);
    void sigHexDisplay(bool isHex);
    void sigSendData(const QByteArray& data);
    void sigHexSend(bool isHex);
    void sigSendStringDisplay(bool isDisplay);
    void sigSendData2Receive(const QByteArray& data);
    void sigTimedSend(bool isTimed, double interval);
    void sigDisplayTimestamp(bool isDisplay);

private slots:
    void onReadyRead();

private:
    // 构造函数和析构函数
    explicit SerialPortManager(QObject* parent = nullptr);
    ~SerialPortManager() = default;

    // 私有方法
    void connectSignals();
    void configureSerialPort(const QMap<QString, QVariant>& serialParams);
    void serialPortWrite(const QByteArray& data);
    void serialPortRead();
    QByteArray& generateTimestamp(const QString& data);
    void clearAllChannelData();
    void processDataPointInternal(const QByteArray& bytes);
    void processQueueInternal();

    // 静态成员变量
    static SerialPortManager* m_pInstance;
    static QMutex m_mutex;
    static constexpr int MAX_QUEUE_SIZE = 4096;

    // 核心对象成员
    QSerialPort* m_pSerialPort = nullptr;
    ChannelManager* m_channelManager = nullptr;

    // 同步对象
    QMutex m_serialMutex;
    mutable QMutex m_channelMutex;

    // 原子变量
    std::atomic_bool m_isHexDisplay{false};

    // 配置变量
    bool m_isHexSend = false;
    bool m_isSendStringDisplay = false;
    bool m_isDisplayTimestamp = false;
    bool m_isChannelDataProcess = false;

    // 通道数据相关
    QQueue<char> m_dataQueue;
    QSet<QString> m_channelIds;
    QByteArray m_currentPoint;
    QMap<QString, double> m_channelTimestamps;
    double m_sampleRate = 0;
    double m_lastTimestamp = 0;
};

#endif //SERIALPORTMANAGER_H