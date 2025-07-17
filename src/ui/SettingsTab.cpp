/**
  ******************************************************************************
  * @file           : SettingsTab.cpp
  * @author         : wangxiangyu
  * @brief          : None
  * @attention      : None
  * @date           : 2025/7/17
  ******************************************************************************
  */

#include "ui/SettingsTab.h"

SettingsTab::SettingsTab(QWidget* parent)
    : QWidget(parent)
{
    this->setUI();
}

void SettingsTab::setUI()
{
    this->setAttribute(Qt::WA_StyledBackground);
    // 设置背景颜色为白色
    this->setStyleSheet("background-color: white;");
}
