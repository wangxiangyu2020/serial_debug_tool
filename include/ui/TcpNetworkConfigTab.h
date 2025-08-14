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
#include <QEvent>
#include <QAbstractItemView>

class TcpNetworkConfigTab : public QWidget
{
    Q_OBJECT

public:
    explicit TcpNetworkConfigTab(QWidget* parent = nullptr);
    virtual ~TcpNetworkConfigTab() = default;

protected:
    // 事件处理方法
    bool eventFilter(QObject* watched, QEvent* event) override;

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
    QWidget* m_pServerWidget = nullptr;
};

#endif //TCPNETWORKCONFIGTAB_H
