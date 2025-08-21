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
    m_pFlushTimer->stop();
    if (m_currentMode == Mode::Client && m_pClientSocket)
    {
        m_readBuffers.remove(m_pClientSocket);
        m_pClientSocket->disconnectFromHost();
        m_pClientSocket->deleteLater();
        m_pClientSocket = nullptr;
        emit clientStatusChanged("状态: 未连接");
    }
    else if (m_currentMode == Mode::Server && m_pTcpServer)
    {
        for (QTcpSocket* client : qAsConst(m_connectedClients))
        {
            client->disconnectFromHost();
        }
        m_readBuffers.clear(); // 清空所有缓冲区
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
            if (!m_pFlushTimer->isActive()) m_pFlushTimer->start();
            this->setupNewSocket(clientSocket);
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
    if (socketState == QAbstractSocket::ConnectedState) m_pFlushTimer->start();
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
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    if (socket && m_readBuffers.contains(socket))
    {
        // 只负责将数据追加到缓冲区，不做任何其他事
        QMutexLocker locker(&m_bufferMutex);
        m_readBuffers[socket].append(socket->readAll());
    }
}

void TcpNetworkManager::onReadBufferTimeout()
{
    // 遍历所有已知的缓冲区
    for (auto it = m_readBuffers.begin(); it != m_readBuffers.end(); ++it)
    {
        QByteArray dataToProcess;
        {
            QMutexLocker locker(&m_bufferMutex);
            if (it.value().isEmpty()) continue;
            // 使用 swap 高效地取出数据，并清空原缓冲区
            dataToProcess.swap(it.value());
        } // 互斥锁在这里自动释放
        // 在无锁状态下，将取出的数据块发送给处理器
        DataPacket packet;
        packet.sourceInfo = QString("%1:%2").arg(it.key()->peerAddress().toString()).arg(it.key()->peerPort());
        packet.data = dataToProcess;
        PacketProcessor::getInstance()->enqueueData(packet);
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

        // 【关键】再次发射状态更新信号，包含最新的客户端数量
        if (m_pTcpServer && m_pTcpServer->isListening())
        {
            QString status = QString("监听中... 端口: %1").arg(m_pTcpServer->serverPort());
            emit serverStatusChanged(status, m_connectedClients.count());
        }
        {
            QMutexLocker locker(&m_bufferMutex);
            m_readBuffers.remove(socket);
        }
        if ((m_currentMode == Mode::Server && m_connectedClients.isEmpty()) || m_currentMode == Mode::Client)
            m_pFlushTimer->stop();
        socket->deleteLater();
    }
}

void TcpNetworkManager::setupNewSocket(QTcpSocket* socket)
{
    if (!socket) return;
    // 只负责为新socket关联一个空的缓冲区
    QMutexLocker locker(&m_bufferMutex);
    m_readBuffers.insert(socket, QByteArray());
}

TcpNetworkManager::TcpNetworkManager(QObject* parent)
    : QObject(parent), m_currentMode(Mode::Idle), m_pTimedSendTimer(nullptr)
{
    m_pFlushTimer = new QTimer(this);
    m_pFlushTimer->setInterval(20); // 设置20ms的清空周期
    this->connect(m_pFlushTimer, &QTimer::timeout, this, &TcpNetworkManager::onReadBufferTimeout);
}
