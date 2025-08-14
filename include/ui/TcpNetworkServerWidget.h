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

#include "utils/StyleLoader.h"

class TcpNetworkServerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TcpNetworkServerWidget(QWidget* parent = nullptr);
    virtual ~TcpNetworkServerWidget() = default;

protected:
    // 事件处理方法
    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    void setUI();
    void createComponents();
    void createLayout();
    void connectSignals();
    void setTextEditProperty(QPlainTextEdit* textEdit);

    QGroupBox* m_pNetworkConfigGroupBox = nullptr;
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
    QLabel* m_pStatusPortLabel = nullptr;
    QLabel* m_pConnectionCountLabel = nullptr;

    QVBoxLayout* m_pMainLayout = nullptr;
};

#endif //TCPNETWORKSERVERWIDGET_H
