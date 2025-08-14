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

class TcpNetworkClientWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TcpNetworkClientWidget(QWidget* parent = nullptr);
    virtual ~TcpNetworkClientWidget() = default;

protected:
    // 事件处理方法
    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    void setUI();
    void createComponents();
    void createLayout();
    void connectSignals();

    QGroupBox* m_pNetworkConfigGroupBox = nullptr;
    QLabel* m_pServerIpLabel = nullptr;
    QLineEdit* m_pServerIpLineEdit = nullptr;
    QLabel* m_pPortLabel = nullptr;
    QLineEdit* m_pPortLineEdit = nullptr;
    QIntValidator* m_pPortValidator = nullptr;
    QPushButton* m_pConnectButton = nullptr;

    QGroupBox* m_pReceiveDataGroupBox = nullptr;
    QPlainTextEdit* m_pReceiveTextEdit = nullptr;

    QCheckBox* m_pDisplayTimestampCheckBox = nullptr;
    QCheckBox* m_pHexDisplayCheckBox = nullptr;
    QPushButton* m_pClearDataButton = nullptr;

    QGroupBox* m_pSendDataGroupBox = nullptr;
    QPlainTextEdit* m_pSendTextEdit = nullptr;

    QCheckBox* m_pHexSendCheckBox = nullptr;
    QCheckBox* m_pTimedSendCheckBox = nullptr;
    QLineEdit* m_pIntervalEdit = nullptr;
    QPushButton* m_pSendButton = nullptr;

    QLabel* m_pStatusLabel = nullptr;


    QVBoxLayout* m_pMainLayout = nullptr;
};

#endif //TCPNETWORKCLIENTWIDGET_H
