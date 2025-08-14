/**
  ******************************************************************************
  * @file           : TcpNetworkConfigTab.cpp
  * @author         : wangxiangyu
  * @brief          : None
  * @attention      : None
  * @date           : 2025/8/13
  ******************************************************************************
  */
#include "ui/TcpNetworkConfigTab.h"

TcpNetworkConfigTab::TcpNetworkConfigTab(QWidget* parent)
    : QWidget(parent)
{
    this->setUI();
}

void TcpNetworkConfigTab::setUI()
{
    this->setAttribute(Qt::WA_StyledBackground);
}
