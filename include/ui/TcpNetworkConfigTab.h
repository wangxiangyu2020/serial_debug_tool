/**
  ******************************************************************************
  * @file           : TcpNetworkConfigTab.h
  * @author         : wangxiangyu
  * @brief          : None
  * @attention      : None
  * @date           : 2025/8/13
  ******************************************************************************
  */

#ifndef TCPNETWORKCONFIGTAB_H
#define TCPNETWORKCONFIGTAB_H

#include <QWidget>
#include <QLabel>
#include <QComboBox>
#include <QVBoxLayout>
#include "utils/StyleLoader.h"
#include "ui/TcpNetworkClientWidget.h"
#include "ui/TcpNetworkServerWidget.h"
#include <QEvent>
#include <QAbstractItemView>
#include "utils/NetworkModeState.h"

class TcpNetworkConfigTab : public QWidget
{
    Q_OBJECT

public:
    explicit TcpNetworkConfigTab(QWidget* parent = nullptr);
    virtual ~TcpNetworkConfigTab() = default;

signals:
    // 通知客户端/服务端应用状态
    void applyClientState(const NetworkModeState& state);
    void applyServerState(const NetworkModeState& state);

protected:
    // 事件处理方法
    bool eventFilter(QObject* watched, QEvent* event) override;

private slots:
    // 接收状态更新
    void onClientStateUpdated(bool displayTimestamp, bool hexDisplay, bool hexSend);
    void onServerStateUpdated(bool displayTimestamp, bool hexDisplay, bool hexSend);

private:
    void setUI();
    void createComponents();
    void createLayout();
    void connectSignals();

    QLabel* m_pModeLabel = nullptr;
    QComboBox* m_pModeComboBox = nullptr;
    QHBoxLayout* m_pModeLayout = nullptr;
    QVBoxLayout* m_pMainLayout = nullptr;

    TcpNetworkClientWidget* m_pClientWidget = nullptr;
    TcpNetworkServerWidget* m_pServerWidget = nullptr;

    NetworkModeState m_clientState;
    NetworkModeState m_serverState;
};

#endif //TCPNETWORKCONFIGTAB_H
