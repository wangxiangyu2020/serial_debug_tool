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

public slots:
    // 以客户端模式启动
    void startClient(const QString& address, quint16 port);
    // 以服务端模式启动
    void startServer(quint16 port);
    // 停止当前所有网络活动，并重置为空闲状态
    void stop();
    // 发送数据（客户端模式下）或向所有客户端广播（服务端模式下）
    void sendData(const QByteArray& data);
    // 向特定客户端发送数据（仅服务端模式）
    void sendDataToClient(QTcpSocket* client, const QByteArray& data);

signals:
    // 状态信息变化信号
    void statusChanged(const QString& status);
    // 收到数据信号
    void dataReceived(const QString& sourceInfo, const QByteArray& data);
    // 新客户端连接信号（仅服务端）
    void clientConnected(const QString& clientInfo, QTcpSocket* clientSocket);
    // 客户端断开连接信号（仅服务端）
    void clientDisconnected(const QString& clientInfo, QTcpSocket* clientSocket);

private slots:
    // 服务端：处理新连接
    void onNewConnection();
    // 客户端：处理Socket状态变化
    void onSocketStateChanged(QAbstractSocket::SocketState socketState);
    // 客户端/服务端：处理数据接收
    void onReadyRead();
    // 服务端：处理客户端断开连接
    void onClientDisconnected();

private:
    explicit TcpNetworkManager(QObject* parent = nullptr);
    ~TcpNetworkManager() = default;

    static TcpNetworkManager* m_pInstance;
    static QMutex m_mutex;

    Mode m_currentMode;
    QTcpSocket* m_pClientSocket;
    QTcpServer* m_pTcpServer;
    QList<QTcpSocket*> m_connectedClients; // 服务端模式下的客户端列表
};

#endif //TCPNETWORKMANAGER_H
