/**
  ******************************************************************************
  * @file           : WaveformTab.cpp
  * @author         : wangxiangyu
  * @brief          : None
  * @attention      : None
  * @date           : 2025/7/17
  ******************************************************************************
  */

#include "ui/WaveformTab.h"

WaveformTab::WaveformTab(QWidget* parent)
    : QWidget(parent)
{
    this->setUI();
}

void WaveformTab::setUI()
{
    this->setAttribute(Qt::WA_StyledBackground);
    this->setStyleSheet("background-color: white;");
}
