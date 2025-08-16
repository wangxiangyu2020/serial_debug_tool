/**
  ******************************************************************************
  * @file           : TcpNetworkClientWidget.h
  * @author         : wangxiangyu
  * @brief          : None
  * @attention      : None
  * @date           : 2025/8/14
  ******************************************************************************
  */

#ifndef TCPNETWORKCLIENTWIDGET_H
#define TCPNETWORKCLIENTWIDGET_H

#include <QWidget>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QIntValidator>
#include <QPushButton>
#include <QPlainTextEdit>
#include <QCheckBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "utils/StyleLoader.h"
#include "ui/CMessageBox.h"
#include <QHostAddress>
#include <core/TcpNetworkManager.h>
#include <QStyle>
#include <QScrollBar>
#include <QComboBox>
#include <QFileDialog>

class TcpNetworkClientWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TcpNetworkClientWidget(QWidget* parent = nullptr);
    virtual ~TcpNetworkClientWidget() = default;

signals:
    void startConnectionRequested(const QString&, quint16);
    void stopConnectionRequested();
    void sendDataRequested(const QByteArray& data);
    void displayTimestamp(bool status);
    void hexDisplay(bool status);
    void hexSend(bool status);
    void startTimedSendRequested(double interval, const QByteArray& data);
    void stopTimedSendRequested();

protected:
    // 事件处理方法
    bool eventFilter(QObject* watched, QEvent* event) override;

private slots:
    void onConnectButtonClicked();
    void onStatusChanged(const QString& status);
    void onSendButtonClicked();
    void onDisplayReceiveData(const QString& sourceInfo, const QByteArray& data);
    void onSaveDataButtonClicked();
    void onTimedSendCheckBoxClicked(bool status);

private:
    void setUI();
    void createComponents();
    void createLayout();
    void connectSignals();

    QGroupBox* m_pNetworkConfigGroupBox = nullptr;
    QLabel* m_pServerIpLabel = nullptr;
    QComboBox* m_pServerIpComboBox = nullptr;
    QLabel* m_pPortLabel = nullptr;
    QLineEdit* m_pPortLineEdit = nullptr;
    QIntValidator* m_pPortValidator = nullptr;
    QPushButton* m_pConnectButton = nullptr;

    QGroupBox* m_pReceiveDataGroupBox = nullptr;
    QPlainTextEdit* m_pReceiveTextEdit = nullptr;

    QCheckBox* m_pDisplayTimestampCheckBox = nullptr;
    QCheckBox* m_pHexDisplayCheckBox = nullptr;
    QPushButton* m_pSaveDataButton = nullptr;
    QPushButton* m_pClearDataButton = nullptr;

    QGroupBox* m_pSendDataGroupBox = nullptr;
    QPlainTextEdit* m_pSendTextEdit = nullptr;

    QCheckBox* m_pHexSendCheckBox = nullptr;
    QCheckBox* m_pTimedSendCheckBox = nullptr;
    QLineEdit* m_pIntervalEdit = nullptr;
    QPushButton* m_pSendButton = nullptr;

    QLabel* m_pStatusLabel = nullptr;


    QVBoxLayout* m_pMainLayout = nullptr;

    bool isConnected = false;
};

#endif //TCPNETWORKCLIENTWIDGET_H
