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
#include "utils/DataPacket.h"
#include "utils/PacketProcessor.h"

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
    QSerialPort* getSerialPort() const;
    bool isHexDisplayEnabled();
    bool isTimestampEnabled();

public slots:
    void handleWriteData(const QByteArray& writeByteArray);
    void openSerialPort(const QMap<QString, QVariant>& serialParams);
    void closeSerialPort();
    void setHexSendStatus(bool status);
    void setHexDisplayStatus(bool status);
    void setSendStringDisplayStatus(bool status);
    void setTimestampStatus(bool status);
    void startTimedSend(double interval, const QByteArray& data);
    void stopTimedSend();

signals:
    void statusChanged(const QString& status, int connectStatus = -1);
    void sendData2ReceiveChanged(const QByteArray& data);

private slots:
    void onSerialPortRead();
    void onReadBufferTimeout(); // 【新增】处理缓冲区超时的槽函数

private:
    // 构造函数和析构函数
    explicit SerialPortManager(QObject* parent = nullptr);
    ~SerialPortManager() = default;
    // 私有方法
    void connectSignals();
    void configureSerialPort(const QMap<QString, QVariant>& serialParams);
    void serialPortWrite(const QByteArray& data);
    // 错误处理
    void handlerError(QSerialPort::SerialPortError error);
    QByteArray& generateTimestamp(const QString& data);

    enum ConnectStatus
    {
        Disconnected = 0,
        Connected = 1,
    };

    // 静态成员变量
    static SerialPortManager* m_pInstance;
    static QMutex m_mutex;
    static constexpr int MAX_QUEUE_SIZE = 4096;

    // 核心对象成员
    QSerialPort* m_pSerialPort = nullptr;

    // 同步对象
    QMutex m_serialMutex;
    mutable QMutex m_channelMutex;

    // 配置变量
    bool m_isHexDisplay = false;
    bool m_isHexSend = false;
    bool m_isSendStringDisplay = false;
    bool m_isDisplayTimestamp = false;


    QTimer* m_pTimedSendTimer;
    QByteArray m_timedSendData;

    QByteArray m_readBuffer; // 【新增】用于缓冲零散数据的成员
    QTimer* m_pReadTimer; // 【新增】用于检测数据流暂停的定时器
    QMutex m_bufferMutex; // 【新增】用于保护缓冲区的互斥锁
};

#endif //SERIALPORTMANAGER_H
