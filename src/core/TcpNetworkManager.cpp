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

void TcpNetworkManager::startClient(const QString& address, quint16 port)
{
    if (m_currentMode != Mode::Idle)
    {
        emit clientStatusChanged("错误: 请先停止当前模式");
        return;
    }
    m_currentMode = Mode::Client;
    m_pClientSocket = new QTcpSocket(this);
    this->connect(m_pClientSocket, &QTcpSocket::stateChanged, this, &TcpNetworkManager::onSocketStateChanged);
    this->connect(m_pClientSocket, &QTcpSocket::readyRead, this, &TcpNetworkManager::onReadyRead);
    emit clientStatusChanged(QString("正在连接到 %1:%2...").arg(address).arg(port));
    m_pClientSocket->connectToHost(address, port);
}

void TcpNetworkManager::startServer(quint16 port)
{
    if (m_currentMode != Mode::Idle)
    {
        emit serverStatusChanged("错误: 请先停止当前模式");
        return;
    }

    m_currentMode = Mode::Server;
    m_pTcpServer = new QTcpServer(this);
    this->connect(m_pTcpServer, &QTcpServer::newConnection, this, &TcpNetworkManager::onNewConnection);
    if (m_pTcpServer->listen(QHostAddress::Any, port))
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
            client->disconnectFromHost();
        }
        m_pTcpServer->close();
        m_pTcpServer->deleteLater();
        m_pTcpServer = nullptr;
        emit serverStatusChanged("状态: 未连接");
    }
    m_currentMode = Mode::Idle;
}

void TcpNetworkManager::handleWriteData(const QByteArray& data)
{
    this->sendData(m_hexSend ? data.toHex() : data);
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

void TcpNetworkManager::startTimedSend(double interval, const QByteArray& data)
{
    this->stopTimedSend();

    // 保存循环发送的数据
    m_timedSendData = data;

    // 创建并配置新的定时器
    m_pTimedSendTimer = new QTimer(this);
    this->connect(m_pTimedSendTimer, &QTimer::timeout, this, [this]()
    {
        // 定时器触发时，只做一件事：发送已保存的数据
        // 注意：这里不再需要 sender()，也不再访问任何UI元素
        if (!m_timedSendData.isEmpty()) this->handleWriteData(m_timedSendData);
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
            m_connectedClients.append(clientSocket);
            this->connect(clientSocket, &QTcpSocket::readyRead, this, &TcpNetworkManager::onReadyRead);
            this->connect(clientSocket, &QTcpSocket::disconnected, this, &TcpNetworkManager::onClientDisconnected);
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
    if (socket)
    {
        QByteArray data = socket->readAll();
        QString sourceInfo = QString("%1:%2").arg(socket->peerAddress().toString()).arg(socket->peerPort());
        this->handleReadData(sourceInfo, data);
    }
}

void TcpNetworkManager::handleReadData(const QString& sourceInfo, const QByteArray& data)
{
    QString formattedData = m_hexDisplay
                                ? QString::fromLatin1(data.toHex(' ').toUpper())
                                : QString::fromUtf8(data);
    QByteArray showByteArray = m_displayTimestamp
                                   ? this->generateTimestamp(formattedData)
                                   : formattedData.toLocal8Bit();
    emit dataReceived(sourceInfo, showByteArray);
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
    : QObject(parent), m_currentMode(Mode::Idle), m_pTimedSendTimer(nullptr)
{
}

QByteArray& TcpNetworkManager::generateTimestamp(const QString& data)
{
    static QString timestamp;
    static QByteArray array;
    timestamp = QDateTime::currentDateTime().toString("[HH:mm:ss.zzz] ");
    array = (timestamp + data).toUtf8();
    return array;
}
