/**
  ******************************************************************************
  * @file           : SerialPortConfig.cpp
  * @author         : wangxiangyu
  * @brief          : None
  * @attention      : None
  * @date           : 2025/7/17
  ******************************************************************************
  */

#include "ui/SerialPortConfigTab.h"

SerialPortConfigTab::SerialPortConfigTab(QWidget* parent)
    : QWidget(parent)
{
    this->setUI();
}

void SerialPortConfigTab::setUI()
{
    this->setAttribute(Qt::WA_StyledBackground);
    // 设置背景颜色为白色
    this->setStyleSheet("background-color: white;");
    m_pSerialPortConfigWidget = new SerialPortConnectConfigWidget(this);
}
