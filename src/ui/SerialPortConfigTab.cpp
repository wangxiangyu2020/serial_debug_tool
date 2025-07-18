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
    this->setStyleSheet("background-color: white;");

    // ==== 创建左侧设置区域 ====
    QWidget* settingsPanel = new QWidget(this); // 左侧容器
    m_pSettingsLayout = new QVBoxLayout(settingsPanel); // 注意父对象是settingsPanel
    m_pSettingsLayout->setSpacing(0);
    m_pSettingsLayout->setContentsMargins(0, 0, 0, 0);

    m_pSerialPortConfigWidget = new SerialPortConnectConfigWidget(settingsPanel);
    m_pSerialPortReceiveSettingsWidget = new SerialPortReceiveSettingsWidget(settingsPanel);
    m_pSerialPortSendSettingsWidget = new SerialPortSendSettingsWidget(settingsPanel);

    m_pSettingsLayout->addWidget(m_pSerialPortConfigWidget);
    m_pSettingsLayout->addSpacing(1);
    m_pSettingsLayout->addWidget(m_pSerialPortSendSettingsWidget);
    m_pSettingsLayout->addSpacing(1);
    m_pSettingsLayout->addWidget(m_pSerialPortReceiveSettingsWidget);

    // ==== 创建右侧内容区域 ====
    QWidget* contentPanel = new QWidget(this); // 右侧容器
    m_pContentLayout = new QVBoxLayout(contentPanel); // 父对象为contentPanel
    m_pContentLayout->setSpacing(0);
    m_pContentLayout->setContentsMargins(0, 0, 0, 0);

    QWidget* receiveWidget = new QWidget(contentPanel);
    QWidget* sendWidget = new QWidget(contentPanel);
    m_pContentLayout->addWidget(receiveWidget);
    m_pContentLayout->addWidget(sendWidget);

    // ==== 主水平布局 ====
    m_pMainLayout = new QHBoxLayout(this); // 关键：设置this的顶层布局
    m_pMainLayout->setContentsMargins(0, 1, 0, 1); // 移除窗口边距
    m_pMainLayout->addWidget(settingsPanel, 1);    // 左侧占1份空间
    m_pMainLayout->addWidget(contentPanel, 3);     // 右侧占3份空间（比例可调）
}
