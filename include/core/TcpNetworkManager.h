/**
  ******************************************************************************
  * @file           : TcpNetworkManager.h
  * @author         : wangxiangyu
  * @brief          : None
  * @attention      : None
  * @date           : 2025/8/15
  ******************************************************************************
  */

#ifndef TCPNETWORKMANAGER_H
#define TCPNETWORKMANAGER_H

#include <QObject>
#include <QMutex>
#include <QTcpSocket>
#include <QTcpServer>
#include <QList>
#include <QDebug>
#include <QDateTime>
#include <QPlainTextEdit>
#include <QTimer>
#include <utils/DataPacket.h>
#include <utils/PacketProcessor.h>

class TcpNetworkManager : public QObject
{
    Q_OBJECT

public:
    enum Mode
    {
        Idle, // 空闲模式
        Client, // 客户端模式
        Server // 服务器模式
    };

    static TcpNetworkManager* getInstance();

    TcpNetworkManager(const TcpNetworkManager&) = delete;
    TcpNetworkManager& operator=(const TcpNetworkManager&) = delete;

    // 获取当前模式
    Mode getCurrentMode() const;
    bool isHexDisplayEnabled();
    bool isTimestampEnabled();

public slots:
    // 以客户端模式启动
    void startClient(const QString& address, quint16 port);
    // 以服务端模式启动
    void startServer(const QString& address, quint16 port);
    // 停止当前所有网络活动，并重置为空闲状态
    void stop();
    // 发送数据处理器
    void handleWriteData(const QByteArray& data, QTcpSocket* clientSocket);
    void setDisplayTimestampStatus(bool status);
    void setHexDisplayStatus(bool status);
    void setHexSendStatus(bool status);
    void startTimedSend(double interval, const QByteArray& data, QTcpSocket* clientSocket);
    void stopTimedSend();

signals:
    // 专用于客户端的状态信息变化信号
    void clientStatusChanged(const QString& status);
    // 专用于服务端的状态信息变化信号
    void serverStatusChanged(const QString& status, int connectionCount = 0);
    // 新客户端连接信号（仅服务端）
    void clientConnected(const QString& clientInfo, QTcpSocket* clientSocket);
    // 客户端断开连接信号（仅服务端）
    void clientDisconnected(const QString& clientInfo, QTcpSocket* clientSocket);

private slots:
    // 服务端：处理新连接
    void onNewConnection();
    // 客户端：处理Socket状态变化
    void onSocketStateChanged(QAbstractSocket::SocketState socketState);
    // 发送数据（客户端模式下）或向所有客户端广播（服务端模式下）
    void sendData(const QByteArray& data);
    // 向特定客户端发送数据（仅服务端模式）
    void sendDataToClient(QTcpSocket* client, const QByteArray& data);
    // 客户端/服务端：处理数据接收
    void onReadyRead();
    // 超时槽函数
    void onReadBufferTimeout();
    // 服务端：处理客户端断开连接
    void onClientDisconnected();
    void setupNewSocket(QTcpSocket* socket);

private:
    explicit TcpNetworkManager(QObject* parent = nullptr);
    ~TcpNetworkManager() = default;

    static TcpNetworkManager* m_pInstance;
    static QMutex m_mutex;

    Mode m_currentMode;
    QTcpSocket* m_pClientSocket;
    QTcpServer* m_pTcpServer;
    QList<QTcpSocket*> m_connectedClients; // 服务端模式下的客户端列表

    bool m_displayTimestamp = false;
    bool m_hexDisplay = false;
    bool m_hexSend = false;

    QTimer* m_pTimedSendTimer;
    QByteArray m_timedSendData;

    QTimer* m_pFlushTimer;
    QMutex m_bufferMutex;
    QMap<QTcpSocket*, QByteArray> m_readBuffers;
};

#endif //TCPNETWORKMANAGER_H
