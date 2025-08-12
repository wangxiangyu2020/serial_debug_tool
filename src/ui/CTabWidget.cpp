/**
  ******************************************************************************
  * @file           : TabContainer.cpp
  * @author         : wangxiangyu
  * @brief          : None
  * @attention      : None
  * @date           : 2025/7/16
  ******************************************************************************
  */

#include "ui/CTabWidget.h"

// 构造函数和析构函数
CTabWidget::CTabWidget(QWidget* parent)
    : QTabWidget(parent)
{
    this->setUI();
    StyleLoader::loadStyleFromFile(this, ":/resources/qss/tab_bar.qss");
}

// 私有方法
void CTabWidget::setUI()
{
    this->setAttribute(Qt::WA_StyledBackground);
    this->setTabPosition(QTabWidget::North);
    this->setTabShape(QTabWidget::Rounded);
    this->setDocumentMode(true);
    this->setMinimumSize(1000, 700);

    m_pSerialPortConfigTab = new SerialPortConfigTab();
    m_pWaveformTab = new WaveformTab();
    m_pSettingsTab = new SettingsTab();

    this->addTab(m_pSerialPortConfigTab, QIcon(":/resources/icons/serial.svg"), "");
    this->addTab(m_pWaveformTab, QIcon(":/resources/icons/waves.svg"), "");
    this->addTab(m_pSettingsTab, QIcon(":/resources/icons/settings.svg"), "");
    this->setIconSize(QSize(24, 24));
}
