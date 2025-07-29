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
    StyleLoader::loadStyleFromFile(this, ":resources/qss/serial_prot_config_tab.qss");
}

void SerialPortConfigTab::setUI()
{
    this->setAttribute(Qt::WA_StyledBackground);

    // ==== 创建左侧设置区域 ====
    QWidget* settingsPanel = new QWidget(this); // 左侧容器
    m_pSettingsLayout = new QVBoxLayout(settingsPanel); // 注意父对象是settingsPanel
    m_pSettingsLayout->setSpacing(0);
    m_pSettingsLayout->setContentsMargins(2, 0, 0, 0);

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
    m_pContentLayout->setSpacing(2);
    m_pContentLayout->setContentsMargins(0, 0, 2, 0);

    m_pSerialPortRealTimeSaveWidget = new SerialPortRealTimeSaveWidget(settingsPanel);
    m_pSerialPortDataReceiveWidget = new SerialPortDataReceiveWidget(contentPanel);
    m_pSerialPortDataSendWidget = new SerialPortDataSendWidget(contentPanel);
    // 设置发送容器固定高度（重要！）
    m_pSerialPortDataSendWidget->setMinimumHeight(100); // 最小高度保证可见
    m_pContentLayout->addWidget(m_pSerialPortDataReceiveWidget);
    m_pContentLayout->addWidget(m_pSerialPortDataSendWidget);
    // 添加到布局并设置伸缩比例
    m_pContentLayout->addWidget(m_pSerialPortRealTimeSaveWidget, 1);
    m_pContentLayout->addWidget(m_pSerialPortDataReceiveWidget, 8); // 1: 可伸缩区域
    m_pContentLayout->addWidget(m_pSerialPortDataSendWidget, 1); // 0: 固定高度区域

    // ==== 主水平布局 ====
    m_pMainLayout = new QHBoxLayout(this); // 关键：设置this的顶层布局
    m_pMainLayout->setContentsMargins(0, 1, 0, 1); // 移除窗口边距
    m_pMainLayout->setSpacing(2);
    m_pMainLayout->addWidget(settingsPanel, 1); // 左侧占1份空间
    m_pMainLayout->addWidget(contentPanel, 3); // 右侧占3份空间（比例可调）
}
