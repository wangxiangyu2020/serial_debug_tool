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


// 构造函数和析构函数
SerialPortReceiveSettingsWidget::SerialPortReceiveSettingsWidget(QWidget* parent)
    : QWidget(parent)
{
    this->setUI();
    StyleLoader::loadStyleFromFile(this, ":/resources/qss/serial_port_receive_settings_widget.qss");
}

QCheckBox* SerialPortReceiveSettingsWidget::getSaveToFileCheckBox()
{
    return m_pSaveToFileCheckBox;
}

void SerialPortReceiveSettingsWidget::onShowScriptEditor()
{
    // 创建实例
    ScriptEditorDialog* dialog = new ScriptEditorDialog(this);
    // 显示对话框
    if (dialog->exec() == QDialog::Accepted) {
        // 获取编辑后的脚本内容
        QString scriptContent = dialog->getScriptContent();
        // 处理保存逻辑
        qDebug() << "保存的脚本内容：" << scriptContent;
    }
    // 清理资源
    dialog->deleteLater();
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
    m_pSaveToFileCheckBox = new QCheckBox("读取数据至文件", this);
    m_pSaveToFileCheckBox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    m_pDisplayTimestampCheckBox = new QCheckBox("显示时间戳", this);
    m_pDisplayTimestampCheckBox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    m_pHexDisplayCheckBox = new QCheckBox("十六进制显示", this);
    m_pHexDisplayCheckBox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    m_pScriptReceiveButton = new QPushButton(this);
    m_pScriptReceiveButton->setObjectName("m_pScriptReceiveButton");
    m_pScriptReceiveButton->setFixedSize(45, 25);
    m_pScriptReceiveButton->setToolTip("使用脚本解析数据");
    m_pScriptReceiveButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    // 添加帮助标签
    m_pScriptHelpButton = new QPushButton(this);
    m_pScriptHelpButton->setObjectName("m_pScriptHelpButton");
    m_pScriptHelpButton->setFixedSize(15, 15);
    m_pScriptHelpButton->setToolTip("目前默认的接收超时为20ms,如果不符合你的需求,\r\n可以使用自定义脚本来对接收数据进行断帧和数据解析。");
    // 按钮
    m_pSaveDataButton = new QPushButton("保存数据", this);
    m_pSaveDataButton->setObjectName("m_pSaveDataButton");
    m_pClearDataButton = new QPushButton("清空数据", this);
    m_pClearDataButton->setObjectName("m_pClearDataButton");
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
    // 创建水平布局用于十六进制显示和接收脚本按钮
    QHBoxLayout* pHexAndScriptLayout = new QHBoxLayout();
    pHexAndScriptLayout->addWidget(m_pHexDisplayCheckBox);
    pHexAndScriptLayout->addWidget(m_pScriptReceiveButton);
    pHexAndScriptLayout->addWidget(m_pScriptHelpButton);
    pHexAndScriptLayout->addStretch();

    // 按钮水平布局
    m_pButtonLayout = new QHBoxLayout();
    m_pButtonLayout->setAlignment(Qt::AlignCenter);
    m_pButtonLayout->setSpacing(8);
    m_pButtonLayout->setContentsMargins(0, 0, 0, 15); // 移除布局边距
    // 添加到按钮布局
    m_pButtonLayout->addWidget(m_pSaveDataButton);
    m_pButtonLayout->addWidget(m_pClearDataButton);
    // 添加到主布局 - 紧凑排列
    m_pMainLayout->addWidget(m_pTitleLabel, 0, Qt::AlignTop);
    m_pMainLayout->addLayout(pSaveAndTimestampLayout); // 添加水平布局
    m_pMainLayout->addLayout(pHexAndScriptLayout);
    m_pMainLayout->addLayout(m_pButtonLayout);
}

void SerialPortReceiveSettingsWidget::connectSignals()
{
    this->connect(m_pClearDataButton, &QPushButton::clicked, [this]()
    {
        emit clearDataRequested();
    });
    this->connect(m_pSaveDataButton, &QPushButton::clicked, [this]()
    {
        emit saveDataRequested();
    });
    this->connect(m_pHexDisplayCheckBox, &QCheckBox::clicked, [this](bool status)
    {
        emit hexDisplayChanged(status);
    });
    this->connect(this, &SerialPortReceiveSettingsWidget::hexDisplayChanged, SerialPortManager::getInstance(),
                  &SerialPortManager::setHexDisplayStatus);
    this->connect(m_pDisplayTimestampCheckBox, &QCheckBox::clicked, [this](bool status)
    {
        emit timestampDisplayChanged(status);
    });
    this->connect(this, &SerialPortReceiveSettingsWidget::timestampDisplayChanged, SerialPortManager::getInstance(),
                  &SerialPortManager::setTimestampStatus);
    this->connect(m_pSaveToFileCheckBox, &QCheckBox::clicked, [this](bool status)
    {
        emit saveToFileChanged(status);
    });
    this->connect(m_pScriptReceiveButton, &QPushButton::clicked, this,
                  &SerialPortReceiveSettingsWidget::onShowScriptEditor);
}
