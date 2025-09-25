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
    if (m_pScriptEditorDialog->exec() == QDialog::Accepted)
    {
        // 获取编辑后的脚本内容
        QString scriptContent = m_pScriptEditorDialog->getScriptContent();
        // 处理保存逻辑,在这里只获取脚本内容，然后通过信号发送给其他地方进行处理
        emit serialPortScriptSaved("serialPort", scriptContent);
    }
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
    m_pScriptReceiveCheckBox = new QCheckBox(this);
    m_pScriptReceiveCheckBox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
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

    m_pUseModbusCheckBox = new QCheckBox("启用Modbus模块", this);

    // 按钮
    m_pSaveDataButton = new QPushButton("保存数据", this);
    m_pSaveDataButton->setObjectName("m_pSaveDataButton");
    m_pClearDataButton = new QPushButton("清空数据", this);
    m_pClearDataButton->setObjectName("m_pClearDataButton");
    // 创建弹框实例
    m_pScriptEditorDialog = new ScriptEditorDialog(this);
    m_pScriptEditorDialog->hide();
}

void SerialPortReceiveSettingsWidget::createLayout()
{
    // 主垂直布局 - 最小间距
    m_pMainLayout = new QVBoxLayout(this);
    m_pMainLayout->setSpacing(5); // 稍微增加一点间距，避免过于拥挤
    m_pMainLayout->setContentsMargins(5, 0, 5, 0); // 设置左右边距，上下为0
    // 创建水平布局用于放置保存文件和时间戳复选框
    QHBoxLayout* pSaveAndTimestampLayout = new QHBoxLayout();
    pSaveAndTimestampLayout->addWidget(m_pSaveToFileCheckBox);
    pSaveAndTimestampLayout->addWidget(m_pDisplayTimestampCheckBox);
    pSaveAndTimestampLayout->addStretch(); // 添加弹性空间
    // 创建水平布局用于十六进制显示和接收脚本按钮
    QHBoxLayout* pHexAndScriptLayout = new QHBoxLayout();
    pHexAndScriptLayout->addWidget(m_pHexDisplayCheckBox);
    pHexAndScriptLayout->addWidget(m_pScriptReceiveCheckBox);
    pHexAndScriptLayout->setSpacing(0);
    pHexAndScriptLayout->addWidget(m_pScriptReceiveButton);
    pHexAndScriptLayout->addWidget(m_pScriptHelpButton);
    pHexAndScriptLayout->addStretch();
    // 按钮水平布局
    m_pButtonLayout = new QHBoxLayout();
    m_pButtonLayout->setAlignment(Qt::AlignCenter);
    m_pButtonLayout->setSpacing(8);
    m_pButtonLayout->setContentsMargins(0, 0, 0, 15);
    // 添加到按钮布局
    m_pButtonLayout->addWidget(m_pSaveDataButton);
    m_pButtonLayout->addWidget(m_pClearDataButton);
    // 添加到主布局 - 紧凑排列
    m_pMainLayout->addWidget(m_pTitleLabel, 0, Qt::AlignTop);
    m_pMainLayout->addLayout(pSaveAndTimestampLayout);
    m_pMainLayout->addLayout(pHexAndScriptLayout);
    m_pMainLayout->addWidget(m_pUseModbusCheckBox);
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
    this->connect(m_pScriptReceiveCheckBox, &QCheckBox::clicked, [this](bool checked)
    {
        // 只有启用脚本时才会获取脚本内容
        if (checked)
        {
            QString scriptContent = m_pScriptEditorDialog->getScriptContent();
            // 处理脚本启用逻辑，在这里是获取脚本内容后发送到其他地方并且将checked发送到管理器中
            emit serialPortScriptSaved("serialPort", scriptContent);
        }
        emit serialPortScriptEnabled(checked);
    });
    this->connect(this, &SerialPortReceiveSettingsWidget::serialPortScriptSaved, ScriptManager::getInstance(),
                  &ScriptManager::onScriptSaved);
    this->connect(this, &SerialPortReceiveSettingsWidget::serialPortScriptEnabled, ScriptManager::getInstance(),
                  &ScriptManager::onSerialPortScriptEnabled);
    this->connect(ScriptManager::getInstance(), &ScriptManager::saveStatusChanged,
                  [this](const QString& key, const QString& status)
                  {
                      if (key != "serialPort") return;
                      CMessageBox::showToast(this, status);
                  });
    this->connect(m_pUseModbusCheckBox, &QCheckBox::clicked, [this](bool checked)
    {
        emit useModbusChanged(checked);
    });
    this->connect(this, &SerialPortReceiveSettingsWidget::useModbusChanged, SerialPortManager::getInstance(),
                  &SerialPortManager::setUseModbusStatus);
}
