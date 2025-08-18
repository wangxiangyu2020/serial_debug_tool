/**
  ******************************************************************************
  * @file           : TcpNetworkServerWidget.h
  * @author         : wangxiangyu
  * @brief          : None
  * @attention      : None
  * @date           : 2025/8/14
  ******************************************************************************
  */

#ifndef TCPNETWORKSERVERWIDGET_H
#define TCPNETWORKSERVERWIDGET_H

#include <QWidget>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QIntValidator>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QComboBox>
#include <QHostAddress>
#include "ui/CMessageBox.h"
#include "core/TcpNetworkManager.h"
#include <QScrollBar>
#include "utils/NetworkModeState.h"
#include "utils/StyleLoader.h"
#include <QFileDialog>

class TcpNetworkServerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TcpNetworkServerWidget(QWidget* parent = nullptr);
    virtual ~TcpNetworkServerWidget() = default;

public slots:
    void onApplyState(const NetworkModeState& state);

signals:
    void startListenRequested(const QString&, quint16);
    void stopListenRequested();
    void displayTimestamp(bool status);
    void hexDisplay(bool status);
    void hexSend(bool status);
    void stateChanged(bool displayTimestamp, bool hexDisplay, bool hexSend);
    void sendDataRequested(const QByteArray& data, QTcpSocket* clientSocket = nullptr);
    void startTimedSendRequested(double interval, const QByteArray& data, QTcpSocket* clientSocket = nullptr);
    void stopTimedSendRequested();

protected:
    // 事件处理方法
    bool eventFilter(QObject* watched, QEvent* event) override;

private slots:
    void onListenButtonClicked();
    void onStatusChanged(const QString& status, int connectionCount);
    void onClientConnected(const QString& clientInfo, QTcpSocket* clientSocket);
    void onClientDisconnected(const QString& clientInfo, QTcpSocket* clientSocket);
    void onDisplayTimestampChanged(bool status);
    void onHexDisplayChanged(bool status);
    void onHexSendChanged(bool status);
    void onDisplayReceiveData(const QByteArray& data);
    void onSendButtonClicked();
    void onTimedSendCheckBoxClicked(bool status);
    void onSaveDataButtonClicked();

private:
    void setUI();
    void createComponents();
    void createLayout();
    void connectSignals();
    void setTextEditProperty(QPlainTextEdit* textEdit);

    QGroupBox* m_pNetworkConfigGroupBox = nullptr;
    QLabel* m_pServerIpLabel = nullptr;
    QComboBox* m_pServerIpComboBox = nullptr;
    QLabel* m_pPortLabel = nullptr;
    QLineEdit* m_pPortLineEdit = nullptr;
    QIntValidator* m_pPortValidator = nullptr;
    QPushButton* m_pStartListenButton = nullptr;

    QGroupBox* m_pConnectedClientGroupBox = nullptr;
    QPlainTextEdit* m_pConnectedClientTextEdit = nullptr;

    QGroupBox* m_pReceiveDataGroupBox = nullptr;
    QPlainTextEdit* m_pReceiveTextEdit = nullptr;

    QCheckBox* m_pDisplayTimestampCheckBox = nullptr;
    QCheckBox* m_pHexDisplayCheckBox = nullptr;
    QPushButton* m_pSaveDataButton = nullptr;
    QPushButton* m_pClearDataButton = nullptr;

    QGroupBox* m_pSendDataGroupBox = nullptr;
    QLabel* m_pSendLabel = nullptr;
    QComboBox* m_pSendClientComboBox = nullptr;
    QCheckBox* m_pHexSendCheckBox = nullptr;
    QCheckBox* m_pTimedSendCheckBox = nullptr;
    QLineEdit* m_pIntervalEdit = nullptr;
    QPlainTextEdit* m_pSendTextEdit = nullptr;

    QPushButton* m_pSendButton = nullptr;

    QLabel* m_pStatusTextLabel = nullptr;
    QLabel* m_pConnectionCountLabel = nullptr;

    QVBoxLayout* m_pMainLayout = nullptr;

    bool isListen = false;
    NetworkModeState m_currentState;
};

#endif //TCPNETWORKSERVERWIDGET_H
