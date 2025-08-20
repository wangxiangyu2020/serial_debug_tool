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

#include "ui/TcpNetworkClientWidget.h"

TcpNetworkManager* TcpNetworkManager::m_pInstance = nullptr;
QMutex TcpNetworkManager::m_mutex;

TcpNetworkManager* TcpNetworkManager::getInstance()
{
    if (m_pInstance == nullptr)
    {
        QMutexLocker locker(&m_mutex);
        if (m_pInstance == nullptr) m_pInstance = new TcpNetworkManager();
    }
    return m_pInstance;
}

TcpNetworkManager::Mode TcpNetworkManager::getCurrentMode() const
{
    return m_currentMode;
}

bool TcpNetworkManager::isHexDisplayEnabled()
{
    return m_hexDisplay;
}

bool TcpNetworkManager::isTimestampEnabled()
{
    return m_displayTimestamp;
}

void TcpNetworkManager::startClient(const QString& address, quint16 port)
{
    if (m_currentMode != Mode::Idle)
    {
        emit clientStatusChanged("错误: 请先停止当前模式");
        return;
    }
    m_currentMode = Mode::Client;
    m_pClientSocket = new QTcpSocket(this);
    this->setupNewSocket(m_pClientSocket);
    this->connect(m_pClientSocket, &QTcpSocket::stateChanged, this, &TcpNetworkManager::onSocketStateChanged);
    this->connect(m_pClientSocket, &QTcpSocket::readyRead, this, &TcpNetworkManager::onReadyRead);
    emit clientStatusChanged(QString("正在连接到 %1:%2...").arg(address).arg(port));
    m_pClientSocket->connectToHost(address, port);
}

void TcpNetworkManager::startServer(const QString& address, quint16 port)
{
    if (m_currentMode != Mode::Idle)
    {
        emit serverStatusChanged("错误: 请先停止当前模式");
        return;
    }

    m_currentMode = Mode::Server;
    m_pTcpServer = new QTcpServer(this);
    this->connect(m_pTcpServer, &QTcpServer::newConnection, this, &TcpNetworkManager::onNewConnection);
    if (m_pTcpServer->listen(QHostAddress(address), port))
    {
        emit serverStatusChanged(QString("监听中... 端口: %1").arg(port));
    }
    else
    {
        emit serverStatusChanged(QString("监听失败... 端口: %1").arg(port));
        this->stop();
    }
}

void TcpNetworkManager::stop()
{
    if (m_currentMode == Mode::Client && m_pClientSocket)
    {
        if (m_readTimers.contains(m_pClientSocket))
        {
            QTimer* timer = m_readTimers.take(m_pClientSocket);
            timer->stop();
            timer->deleteLater();
        }
        m_readBuffers.remove(m_pClientSocket);
        m_pClientSocket->disconnectFromHost();
        m_pClientSocket->deleteLater();
        m_pClientSocket = nullptr;
        emit clientStatusChanged("状态: 未连接");
    }
    else if (m_currentMode == Mode::Server && m_pTcpServer)
    {
        // 断开所有客户端连接
        for (QTcpSocket* client : qAsConst(m_connectedClients))
        {
            if (m_readTimers.contains(client))
            {
                QTimer* timer = m_readTimers.take(client);
                timer->stop();
                timer->deleteLater();
            }
            m_readBuffers.remove(client);
            client->disconnectFromHost();
        }
        m_pTcpServer->close();
        m_pTcpServer->deleteLater();
        m_pTcpServer = nullptr;
        emit serverStatusChanged("状态: 未监听");
    }
    m_currentMode = Mode::Idle;
}

void TcpNetworkManager::handleWriteData(const QByteArray& data, QTcpSocket* clientSocket)
{
    if (clientSocket == nullptr) this->sendData(m_hexSend ? data.toHex() : data);
    else this->sendDataToClient(clientSocket, m_hexSend ? data.toHex() : data);
}

void TcpNetworkManager::setDisplayTimestampStatus(bool status)
{
    m_displayTimestamp = status;
}

void TcpNetworkManager::setHexDisplayStatus(bool status)
{
    m_hexDisplay = status;
}

void TcpNetworkManager::setHexSendStatus(bool status)
{
    m_hexSend = status;
}

void TcpNetworkManager::startTimedSend(double interval, const QByteArray& data, QTcpSocket* clientSocket)
{
    this->stopTimedSend();

    // 保存循环发送的数据
    m_timedSendData = data;

    // 创建并配置新的定时器
    m_pTimedSendTimer = new QTimer(this);
    this->connect(m_pTimedSendTimer, &QTimer::timeout, this, [this, clientSocket]()
    {
        // 定时器触发时，只做一件事：发送已保存的数据
        // 注意：这里不再需要 sender()，也不再访问任何UI元素
        if (!m_timedSendData.isEmpty()) this->handleWriteData(m_timedSendData, clientSocket);
    });

    int intervalMs = static_cast<int>(interval * 1000);
    m_pTimedSendTimer->start(intervalMs);
}

void TcpNetworkManager::stopTimedSend()
{
    if (m_pTimedSendTimer)
    {
        m_pTimedSendTimer->stop();
        m_pTimedSendTimer->deleteLater(); // 使用 deleteLater 安全删除
        m_pTimedSendTimer = nullptr;
        m_timedSendData.clear();
    }
}

void TcpNetworkManager::onNewConnection()
{
    while (m_pTcpServer->hasPendingConnections())
    {
        if (QTcpSocket* clientSocket = m_pTcpServer->nextPendingConnection())
        {
            this->setupNewSocket(clientSocket); // 为这个新客户端设置缓冲区和定时器
            m_connectedClients.append(clientSocket);
            this->connect(clientSocket, &QTcpSocket::readyRead, this, &TcpNetworkManager::onReadyRead);
            this->connect(clientSocket, &QTcpSocket::disconnected, this, &TcpNetworkManager::onClientDisconnected);
            QString clientInfo = QString("%1:%2").arg(clientSocket->peerAddress().toString(),
                                                      QString::number(clientSocket->peerPort()));
            emit clientConnected(clientInfo, clientSocket);
            if (m_pTcpServer && m_pTcpServer->isListening())
            {
                QString status = QString("监听中... 端口: %1").arg(m_pTcpServer->serverPort());
                emit serverStatusChanged(status, m_connectedClients.count());
            }
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
    emit clientStatusChanged(status);
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

void TcpNetworkManager::onReadyRead()
{
    // 通过 sender() 获取是哪个socket触发了信号
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) return;
    // 1. 从 Map 中找到该 socket 对应的缓冲区和定时器
    if (m_readBuffers.contains(socket) && m_readTimers.contains(socket))
    {
        // 2. 将新数据追加到对应的缓冲区
        m_readBuffers[socket].append(socket->readAll());
        // 3. 重置对应的定时器
        m_readTimers[socket]->start();
    }
}

void TcpNetworkManager::onReadBufferTimeout(QTcpSocket* socket)
{
    if (!socket || !m_readBuffers.contains(socket)) return;

    QByteArray& buffer = m_readBuffers[socket];
    if (buffer.isEmpty()) return;
    // 暂停已经发生，我们认为缓冲区里是一个完整的数据包
    DataPacket packet;
    packet.sourceInfo = QString("%1:%2").arg(socket->peerAddress().toString()).arg(socket->peerPort());
    packet.data = buffer; // 使用整个缓冲区的数据
    // 【关键】处理完后，清空该socket的缓冲区
    buffer.clear();
    PacketProcessor::getInstance()->enqueueData(packet);
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

        // 【关键】再次发射状态更新信号，包含最新的客户端数量
        if (m_pTcpServer && m_pTcpServer->isListening())
        {
            QString status = QString("监听中... 端口: %1").arg(m_pTcpServer->serverPort());
            emit serverStatusChanged(status, m_connectedClients.count());
        }
        // 【关键】从 Map 中移除并删除关联的定时器和缓冲区
        if (m_readTimers.contains(socket))
        {
            QTimer* timer = m_readTimers.take(socket);
            timer->stop();
            timer->deleteLater();
        }
        m_readBuffers.remove(socket);
        socket->deleteLater();
    }
}

void TcpNetworkManager::setupNewSocket(QTcpSocket* socket)
{
    if (!socket) return;

    // 1. 为新socket创建并关联一个定时器
    QTimer* timer = new QTimer(this);
    timer->setSingleShot(true);
    timer->setInterval(20); // 20毫秒超时
    m_readTimers.insert(socket, timer);

    // 2. 为新socket关联一个空的缓冲区
    m_readBuffers.insert(socket, QByteArray());

    // 3. Lambda 捕获了当前的 socket 指针，这样超时后我们就知道是谁超时了。
    this->connect(timer, &QTimer::timeout, this, [this, socket]()
    {
        this->onReadBufferTimeout(socket);
    });
}

TcpNetworkManager::TcpNetworkManager(QObject* parent)
    : QObject(parent), m_currentMode(Mode::Idle), m_pTimedSendTimer(nullptr)
{
}
