/**
  ******************************************************************************
  * @file           : SerialPortReceiveSettingsWidget.cpp
  * @author         : wangxiangyu
  * @brief          : None
  * @attention      : None
  * @date           : 2025/7/18
  ******************************************************************************
  */
#include "ui/SerialPortReceiveSettingsWidget.h"

SerialPortReceiveSettingsWidget::SerialPortReceiveSettingsWidget(QWidget* parent)
    : QWidget(parent)
{
    this->setUI();
    StyleLoader::loadStyleFromFile(this, ":/resources/qss/serial_port_receive_settings_widget.qss");
}

void SerialPortReceiveSettingsWidget::setUI()
{
    this->setAttribute(Qt::WA_StyledBackground);
    this->createComponents();
    this->createLayout();
    this->connectSignals();
}

void SerialPortReceiveSettingsWidget::createComponents()
{
    // 标题标签 - 移除所有额外边距
    m_pTitleLabel = new QLabel("接收设置", this);
    m_pTitleLabel->setObjectName("m_pTitleLabel");
    m_pTitleLabel->setContentsMargins(4, 5, 0, 0); // 移除内部边距
    m_pTitleLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    // 复选框 - 紧凑模式
    m_pSaveToFileCheckBox = new QCheckBox("保存数据到文件", this);
    m_pSaveToFileCheckBox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    m_pDisplayTimestampCheckBox = new QCheckBox("显示时间戳", this);
    m_pDisplayTimestampCheckBox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    m_pHexDisplayCheckBox = new QCheckBox("十六进制显示", this);
    m_pHexDisplayCheckBox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    // 按钮水平布局
    m_pButtonLayout = new QHBoxLayout();
    m_pButtonLayout->setAlignment(Qt::AlignCenter);
    m_pButtonLayout->setSpacing(8);
    m_pButtonLayout->setContentsMargins(0, 0, 0, 15); // 移除布局边距
    // 按钮
    m_pSaveDataButton = new QPushButton("保存数据", this);
    m_pClearDataButton = new QPushButton("清空数据", this);
}

void SerialPortReceiveSettingsWidget::createLayout()
{
    // 主垂直布局 - 最小间距
    m_pMainLayout = new QVBoxLayout(this);
    m_pMainLayout->setSpacing(5); // 最小化间距
    m_pMainLayout->setContentsMargins(0, 0, 0, 0);
    // 创建水平布局用于放置保存文件和时间戳复选框
    QHBoxLayout* pSaveAndTimestampLayout = new QHBoxLayout();
    pSaveAndTimestampLayout->addWidget(m_pSaveToFileCheckBox);
    pSaveAndTimestampLayout->addWidget(m_pDisplayTimestampCheckBox);
    pSaveAndTimestampLayout->addStretch(); // 添加弹性空间
    // 添加到按钮布局
    m_pButtonLayout->addWidget(m_pSaveDataButton);
    m_pButtonLayout->addWidget(m_pClearDataButton);
    // 添加到主布局 - 紧凑排列
    m_pMainLayout->addWidget(m_pTitleLabel, 0, Qt::AlignTop);
    m_pMainLayout->addLayout(pSaveAndTimestampLayout); // 添加水平布局
    m_pMainLayout->addWidget(m_pHexDisplayCheckBox, 0, Qt::AlignTop);
    m_pMainLayout->addLayout(m_pButtonLayout);
}

void SerialPortReceiveSettingsWidget::connectSignals()
{
    this->connect(m_pClearDataButton, &QPushButton::clicked, [this]()
    {
        emit SerialPortDataReceiveWidget::getSerialPortDataReceiveWidget()->sigClearReceiveData();
    });
    this->connect(m_pSaveDataButton, &QPushButton::clicked, [this]()
    {
        emit SerialPortDataReceiveWidget::getSerialPortDataReceiveWidget()->sigSaveToFile();
    });
    this->connect(m_pHexDisplayCheckBox, &QCheckBox::toggled, [this](bool checked)
    {
        emit SerialPortConnectConfigWidget::getSerialPortManager()->sigHexDisplay(checked);
    });
    this->connect(m_pDisplayTimestampCheckBox, &QCheckBox::toggled, [this](bool checked)
    {
        emit SerialPortConnectConfigWidget::getSerialPortManager()->sigDisplayTimestamp(checked);
    });
}
