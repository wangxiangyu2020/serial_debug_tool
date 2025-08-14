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

class TcpNetworkConfigTab : public QWidget
{
    Q_OBJECT

public:
    explicit TcpNetworkConfigTab(QWidget* parent = nullptr);
    virtual ~TcpNetworkConfigTab() = default;

private:
    void setUI();
};

#endif //TCPNETWORKCONFIGTAB_H
