/**
  ******************************************************************************
  * @file           : SerialPortSendSettingsWidget.cpp
  * @author         : wangxiangyu
  * @brief          : None
  * @attention      : None
  * @date           : 2025/7/18
  ******************************************************************************
  */

#include "ui/SerialPortSendSettingsWidget.h"


// 构造函数和析构函数
SerialPortSendSettingsWidget::SerialPortSendSettingsWidget(QWidget* parent)
    : QWidget(parent)
{
    this->setUI();
    StyleLoader::loadStyleFromFile(this, ":/resources/qss/serial_port_send_settings_widget.qss");
}

void SerialPortSendSettingsWidget::onTimedSendCheckBoxClicked(bool status)
{
    if (!status)
    {
        m_pIntervalEdit->setEnabled(true);
        emit stopTimedSendRequested();
        return;
    }
    if (!SerialPortManager::getInstance()->getSerialPort()->isOpen())
    {
        CMessageBox::showToast(this, tr("请先连接串口"));
        m_pTimedSendCheckBox->setChecked(false); // 恢复复选框状态
        return;
    }
    QString textToSend = SerialPortDataSendWidget::getSendTextEdit()->toPlainText(); // 从UI获取要发送的文本
    if (textToSend.trimmed().isEmpty())
    {
        CMessageBox::showToast(this, "请输入要发送的数据");
        m_pTimedSendCheckBox->setChecked(false);
        return;
    }
    m_pIntervalEdit->setEnabled(false);
    double interval = m_pIntervalEdit->text().toDouble();
    emit startTimedSendRequested(interval, textToSend);
}

void SerialPortSendSettingsWidget::setUI()
{
    this->setAttribute(Qt::WA_StyledBackground);
    this->createComponents();
    this->createLayout();
    this->connectSignals();
}

void SerialPortSendSettingsWidget::createComponents()
{
    // 标题标签
    m_pTitleLabel = new QLabel("发送设置", this);
    m_pTitleLabel->setObjectName("m_pTitleLabel");
    m_pTitleLabel->setContentsMargins(4, 5, 0, 0); // 移除内部边距
    m_pTitleLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    // 复选框 - 紧凑模式
    m_pHexSendCheckBox = new QCheckBox("十六进制发送", this);
    m_pHexSendCheckBox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    // 新增：显示发送字符串复选框（位置提前）
    m_pShowSendStringCheckBox = new QCheckBox("显示发送字符串", this);
    m_pShowSendStringCheckBox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    // 定时发送相关组件的水平布局
    m_pTimedSendLayout = new QHBoxLayout();
    m_pTimedSendLayout->setAlignment(Qt::AlignLeft);
    m_pTimedSendLayout->setSpacing(8);
    m_pTimedSendLayout->setContentsMargins(0, 0, 0, 0);
    m_pTimedSendCheckBox = new QCheckBox("定时发送", this);
    m_pTimedSendCheckBox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    // 减小时间输入框尺寸
    m_pIntervalLabel = new QLabel("(秒)", this);
    m_pIntervalLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_pIntervalEdit = new QLineEdit(this);
    m_pIntervalEdit->setFixedWidth(50); // 从60减小到50
    m_pIntervalEdit->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_pIntervalEdit->setValidator(new QDoubleValidator(0.1, 60.0, 1, this));
    m_pIntervalEdit->setText("1.0");
    m_pIntervalEdit->setEnabled(true);
}

void SerialPortSendSettingsWidget::createLayout()
{
    // 主垂直布局 - 与接收设置完全一致
    m_pMainLayout = new QVBoxLayout(this);
    m_pMainLayout->setSpacing(2); // 最小化间距
    m_pMainLayout->setContentsMargins(0, 0, 0, 0); // 完全移除边距
    // 添加到布局
    m_pTimedSendLayout->addWidget(m_pTimedSendCheckBox);
    m_pTimedSendLayout->addWidget(m_pIntervalEdit);
    m_pTimedSendLayout->addWidget(m_pIntervalLabel);
    // 添加到主布局 - 按新顺序排列
    m_pMainLayout->addWidget(m_pTitleLabel, 0, Qt::AlignTop);
    m_pMainLayout->addWidget(m_pHexSendCheckBox, 0, Qt::AlignTop);
    m_pMainLayout->addWidget(m_pShowSendStringCheckBox, 0, Qt::AlignTop); // 位置提前
    m_pMainLayout->addLayout(m_pTimedSendLayout, 0); // 定时发送布局
    // 设置布局底部边距
    m_pMainLayout->setContentsMargins(0, 0, 0, 15); // 底部3px边距
}

void SerialPortSendSettingsWidget::connectSignals()
{
    this->connect(m_pHexSendCheckBox, &QCheckBox::clicked, [this](bool status)
    {
        emit hexSendChanged(status);
    });
    this->connect(this, &SerialPortSendSettingsWidget::hexSendChanged, SerialPortManager::getInstance(),
                  &SerialPortManager::setHexSendStatus);
    this->connect(m_pShowSendStringCheckBox, &QCheckBox::clicked, [this](bool status)
    {
        emit showSendStringChanged(status);
    });
    this->connect(this, &SerialPortSendSettingsWidget::showSendStringChanged, SerialPortManager::getInstance(),
                  &SerialPortManager::setSendStringDisplayStatus);
    this->connect(m_pTimedSendCheckBox, &QCheckBox::clicked, this,
                  &SerialPortSendSettingsWidget::onTimedSendCheckBoxClicked);
    this->connect(this, &SerialPortSendSettingsWidget::startTimedSendRequested, SerialPortManager::getInstance(),
                  &SerialPortManager::startTimedSend);
    this->connect(this, &SerialPortSendSettingsWidget::stopTimedSendRequested, SerialPortManager::getInstance(),
                  &SerialPortManager::stopTimedSend);
}
