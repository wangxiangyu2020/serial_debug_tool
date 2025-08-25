/**
  ******************************************************************************
  * @file           : PacketProcessor.h
  * @author         : wangxiangyu
  * @brief          : None
  * @attention      : None
  * @date           : 2025/8/20
  ******************************************************************************
  */

#ifndef PACKETPROCESSOR_H
#define PACKETPROCESSOR_H

#include "DataPacket.h"
#include "utils/ThreadPoolManager.h" // 引入您的线程池
#include "core/SerialPortManager.h"
#include "core/TcpNetworkManager.h"
#include "core/ScriptManager.h"
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QThread>
#include <QQueue>
#include <QMutex>
#include <QWaitCondition>
#include <QJsonValue>
#include <QGlobalStatic> // 包含头文件以使用宏

class PacketProcessor : public QThread
{
    Q_OBJECT

public:
    static PacketProcessor* getInstance(); // 改为单例，方便全局访问

    // 供生产者(Serial/Tcp Manager)调用的公共接口
    void enqueueData(const DataPacket& packet);

signals:
    // 串口显示数据信号
    void serialPortReceiveDataChanged(const QByteArray& data);
    // TCP显示数据信号
    void tcpNetworkReceiveDataChanged(const QByteArray& data);
    void waveformDataReady(const QString& channelName, const QVariantList& point);

protected:
    void run() override;

private:
    explicit PacketProcessor(QObject* parent = nullptr);
    ~PacketProcessor();
    PacketProcessor(const PacketProcessor&) = delete;
    PacketProcessor& operator=(const PacketProcessor&) = delete;

    // 【新增】为不同来源的数据创建独立的处理函数，使逻辑更清晰
    void processSerialData(const DataPacket& packet);
    void processSerialDataWithScript(const DataPacket& packet);
    void processSerialDataWithoutScript(const DataPacket& packet);
    void processTcpData(const DataPacket& packet);

    static PacketProcessor* m_instance;
    static QMutex m_instanceMutex;

    QHash<QString, double> m_channelTimestamps;

    QQueue<DataPacket> m_dataQueue;
    QMutex m_mutex;
    QWaitCondition m_condition;
    bool m_quit = false;

    QByteArray m_serialWaveformBuffer;

    bool m_useUserScript = false;
};

#endif // PACKETPROCESSOR_H
