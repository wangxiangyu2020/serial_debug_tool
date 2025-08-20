/**
  ******************************************************************************
  * @file           : PacketProcessor.cpp
  * @author         : wangxiangyu
  * @brief          : None
  * @attention      : None
  * @date           : 2025/8/20
  ******************************************************************************
  */

#include "utils/PacketProcessor.h"
#include "utils/ThreadPoolManager.h" // 引入您的线程池
#include "core/SerialPortManager.h"
#include "core/TcpNetworkManager.h"
// #include "core/ScriptManager.h"      // 引入您的脚本管理器
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>

PacketProcessor* PacketProcessor::m_instance = nullptr;
QMutex PacketProcessor::m_instanceMutex;

PacketProcessor* PacketProcessor::getInstance()
{
    if (m_instance == nullptr)
    {
        QMutexLocker locker(&m_instanceMutex);
        if (m_instance == nullptr) m_instance = new PacketProcessor;
    }
    return m_instance;
}

void PacketProcessor::enqueueData(const DataPacket& packet)
{
    QMutexLocker locker(&m_mutex);
    m_dataQueue.enqueue(packet);
    m_condition.wakeOne();
}

void PacketProcessor::run()
{
    while (!m_quit)
    {
        m_mutex.lock();
        if (m_dataQueue.isEmpty()) m_condition.wait(&m_mutex);
        if (m_quit)
        {
            m_mutex.unlock();
            break;
        }
        QQueue<DataPacket> localQueue;
        localQueue.swap(m_dataQueue);
        m_mutex.unlock();
        while (!localQueue.isEmpty())
        {
            DataPacket packet = localQueue.dequeue();
            if (packet.sourceInfo.startsWith("COM") || packet.sourceInfo.startsWith("tty"))
            {
                processSerialData(packet);
            }
            else if (packet.sourceInfo.contains(":"))
            {
                processTcpData(packet);
            }
            else
            {
                qWarning() << "Invalid source info: " << packet.sourceInfo;
            }
        }
    }
    qDebug() << "PacketProcessor thread exited";
}

PacketProcessor::PacketProcessor(QObject* parent) : QThread(parent)
{
}

PacketProcessor::~PacketProcessor()
{
    m_mutex.lock();
    m_quit = true;
    m_mutex.unlock();
    m_condition.wakeAll();
    wait();
}

void PacketProcessor::processSerialData(const DataPacket& packet)
{
    SerialPortManager* spManager = SerialPortManager::getInstance();
    const bool isHex = spManager->isHexDisplayEnabled();
    const bool addTimestamp = spManager->isTimestampEnabled();

    QString formattedData = isHex
                                ? QString::fromLatin1(packet.data.toHex(' ').toUpper())
                                : QString::fromUtf8(packet.data);
    QString displayText;
    if (addTimestamp)
    {
        QString timestamp = QDateTime::currentDateTime().toString("HH:mm:ss.zzz");
        displayText = QString("%1 %2").arg(timestamp).arg(formattedData);
    }
    else
    {
        displayText = formattedData;
    }
    emit serialPortReceiveDataChanged(displayText.toLocal8Bit());
}

void PacketProcessor::processTcpData(const DataPacket& packet)
{
    TcpNetworkManager* tcpManager = TcpNetworkManager::getInstance();
    // 假设TcpManager也将提供这些状态接口
    const bool isHex = tcpManager->isHexDisplayEnabled();
    const bool addTimestamp = tcpManager->isTimestampEnabled();

    QString formattedData = isHex
                                ? QString::fromLatin1(packet.data.toHex(' ').toUpper())
                                : QString::fromUtf8(packet.data);
    // 在最前面加上传输端信息
    formattedData.prepend("from " + packet.sourceInfo + ": ");
    QString displayText;
    if (addTimestamp)
    {
        QString timestamp = QDateTime::currentDateTime().toString("[HH:mm:ss.zzz] ");
        displayText = timestamp + formattedData;
    }
    else
    {
        displayText = formattedData;
    }
    emit tcpNetworkReceiveDataChanged(displayText.toLocal8Bit());

}
