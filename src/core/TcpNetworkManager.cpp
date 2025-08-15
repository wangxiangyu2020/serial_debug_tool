/**
  ******************************************************************************
  * @file           : TcpNetworkManager.cpp
  * @author         : wangxiangyu
  * @brief          : None
  * @attention      : None
  * @date           : 2025/8/15
  ******************************************************************************
  */

#include "core/TcpNetworkManager.h"

TcpNetworkManager* TcpNetworkManager::m_pInstance = nullptr;
QMutex TcpNetworkManager::m_mutex;

TcpNetworkManager* TcpNetworkManager::getInstance()
{
    if (m_pInstance == nullptr)
    {
        QMutexLocker locker(&m_mutex);
        if (m_pInstance == nullptr) m_pInstance = new TcpNetworkManager();
    }
}

TcpNetworkManager::Mode TcpNetworkManager::getCurrentMode() const
{
    return m_currentMode;
}

void TcpNetworkManager::startClient(const QString& address, quint16 port)
{
    if (m_currentMode != Mode::Idle)
    {
        emit statusChanged("错误: 请先停止当前模式");
        return;
    }
    m_currentMode = Mode::Client;
    m_pClientSocket = new QTcpSocket(this);
    this->connect(m_pClientSocket, &QTcpSocket::stateChanged, this, &TcpNetworkManager::onSocketStateChanged);
    this->connect(m_pClientSocket, &QTcpSocket::readyRead, this, &TcpNetworkManager::onReadyRead);
    emit statusChanged(QString("正在连接到 %1:%2...").arg(address).arg(port));
    m_pClientSocket->connectToHost(address, port);
}

void TcpNetworkManager::startServer(quint16 port)
{
    if (m_currentMode != Mode::Idle)
    {
        emit statusChanged("错误: 请先停止当前模式");
        return;
    }

    m_currentMode = Mode::Server;
    m_pTcpServer = new QTcpServer(this);
    this->connect(m_pTcpServer, &QTcpServer::newConnection, this, &TcpNetworkManager::onNewConnection);
    if (m_pTcpServer->listen(QHostAddress::Any, port))
    {
        emit statusChanged(QString("监听中... 端口: %1").arg(port));
    }
    else
    {
        emit statusChanged(QString("监听失败... 端口: %1").arg(port));
        this->stop();
    }
}

void TcpNetworkManager::stop()
{
    if (m_currentMode == Mode::Client && m_pClientSocket)
    {
        m_pClientSocket->disconnectFromHost();
        m_pClientSocket->deleteLater();
        m_pClientSocket = nullptr;
    }
    else if (m_currentMode == Mode::Server && m_pTcpServer)
    {
        // 断开所有客户端连接
        for (QTcpSocket* client : qAsConst(m_connectedClients))
        {
            client->disconnectFromHost();
        }
        m_pTcpServer->close();
        m_pTcpServer->deleteLater();
        m_pTcpServer = nullptr;
    }
    m_currentMode = Mode::Idle;
    emit statusChanged("状态: 未连接");
}

void TcpNetworkManager::sendData(const QByteArray& data)
{
    if (m_currentMode == Mode::Client && m_pClientSocket && m_pClientSocket->state() == QAbstractSocket::ConnectedState)
    {
        m_pClientSocket->write(data);
    }
    else if (m_currentMode == Mode::Server)
    {
        for (QTcpSocket* client : qAsConst(m_connectedClients))
        {
            client->write(data);
        }
    }
}

void TcpNetworkManager::sendDataToClient(QTcpSocket* client, const QByteArray& data)
{
    if (m_currentMode == Mode::Server && client && m_connectedClients.contains(client))
    {
        client->write(data);
    }
}

void TcpNetworkManager::onNewConnection()
{
    while (m_pTcpServer->hasPendingConnections())
    {
        if (QTcpSocket* clientSocket = m_pTcpServer->nextPendingConnection())
        {
            m_connectedClients.append(clientSocket);
            this->connect(clientSocket, &QTcpSocket::readyRead, this, &TcpNetworkManager::onReadyRead);
            this->connect(clientSocket, &QTcpSocket::disconnected, this, &TcpNetworkManager::clientDisconnected);
            QString clientInfo = QString("%1:%2").arg(clientSocket->peerAddress().toString(),
                                                      QString::number(clientSocket->peerPort()));
            emit clientConnected(clientInfo, clientSocket);
        }
    }
}

void TcpNetworkManager::onSocketStateChanged(QAbstractSocket::SocketState socketState)
{
    if (m_currentMode != Client) return;

    QString status;
    switch (socketState)
    {
    case QAbstractSocket::UnconnectedState:
        status = "状态: 已断开";
        this->stop();
        break;
    case QAbstractSocket::HostLookupState:
        status = "状态: 正在查找主机...";
        break;
    case QAbstractSocket::ConnectingState:
        status = "状态: 正在连接...";
        break;
    case QAbstractSocket::ConnectedState:
        status = QString("状态: 已连接到 %1:%2")
                 .arg(m_pClientSocket->peerName())
                 .arg(m_pClientSocket->peerPort());
        break;
    case QAbstractSocket::ClosingState:
        status = "状态: 正在关闭连接...";
        break;
    default:
        status = "状态: 未知";
    }
    emit statusChanged(status);
}

void TcpNetworkManager::onReadyRead()
{
    // 通过 sender() 获取是哪个socket触发了信号
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    if (socket)
    {
        QByteArray data = socket->readAll();
        QString sourceInfo = QString("%1:%2").arg(socket->peerAddress().toString()).arg(socket->peerPort());
        emit dataReceived(sourceInfo, data);
    }
}

void TcpNetworkManager::onClientDisconnected()
{
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    if (socket)
    {
        QString clientInfo = QString("%1:%2").arg(socket->peerAddress().toString()).arg(socket->peerPort());
        m_connectedClients.removeAll(socket);
        socket->deleteLater(); // 安全地删除socket
        emit clientDisconnected(clientInfo, socket); // 传递socket指针用于UI移除
    }
}

TcpNetworkManager::TcpNetworkManager(QObject* parent)
    : QObject(parent), m_currentMode(Mode::Idle)
{
}
