/**
  ******************************************************************************
  * @file           : SerialPortConnectConfigWidget.cpp
  * @author         : wangxiangyu
  * @brief          : None
  * @attention      : None
  * @date           : 2025/7/17
  ******************************************************************************
  */
#include "ui/SerialPortConnectConfigWidget.h"

// 构造函数和析构函数
SerialPortConnectConfigWidget::SerialPortConnectConfigWidget(QWidget* parent)
    : QWidget(parent)
{
    this->setUI();
    qRegisterMetaType<QSerialPortInfo>("QSerialPortInfo"); // 注册元类型
    StyleLoader::loadStyleFromFile(this, ":/resources/qss/serial_port_connect_config_widget.qss");
    ThreadPoolManager::addTask(&SerialPortConnectConfigWidget::detectionAvailablePorts, this);
}

// 事件处理方法
bool SerialPortConnectConfigWidget::eventFilter(QObject* watched, QEvent* event)
{
    if (event->type() == QEvent::Wheel)
    {
        if (auto* cb = qobject_cast<QComboBox*>(watched))
        {
            if (!cb->view()->isVisible())
            {
                return true;
            }
        }
    }
    return QWidget::eventFilter(watched, event);
}

// private slots
void SerialPortConnectConfigWidget::onConnectButtonClicked()
{
    if (m_isConnected)
    {
        m_pPortComboBox->setEnabled(true);
        m_pBaudRateComboBox->setEnabled(true);
        m_pDataBitsComboBox->setEnabled(true);
        m_pStopBitsComboBox->setEnabled(true);
        m_pParityComboBox->setEnabled(true);
        m_pFlowControlComboBox->setEnabled(true);
        emit stopConnectionRequested();
        return;
    }
    if (m_pPortComboBox->count() == 0)
    {
        CMessageBox::showToast("请选择端口");
        return;
    }
    QString baudRateString = m_pBaudRateComboBox->currentText();
    bool isNumber;
    qint32 baudRateValue = baudRateString.toInt(&isNumber);
    if (!isNumber || baudRateValue <= 0)
    {
        CMessageBox::showToast("请输入有效波特率");
        return;
    }
    QMap<QString, QVariant> serialParams
    {
        {"portInfo", QVariant::fromValue(m_pPortComboBox->currentData().value<QSerialPortInfo>())},
        {"baudRate", baudRateValue},
        {"dataBits", m_pDataBitsComboBox->currentData().value<QSerialPort::DataBits>()},
        {"stopBits", m_pStopBitsComboBox->currentData().value<QSerialPort::StopBits>()},
        {"parity", m_pParityComboBox->currentData().value<QSerialPort::Parity>()},
        {"flowControl", m_pFlowControlComboBox->currentData().value<QSerialPort::FlowControl>()}
    };

    m_pPortComboBox->setEnabled(false);
    m_pBaudRateComboBox->setEnabled(false);
    m_pDataBitsComboBox->setEnabled(false);
    m_pStopBitsComboBox->setEnabled(false);
    m_pParityComboBox->setEnabled(false);
    m_pFlowControlComboBox->setEnabled(false);

    emit startConnectionRequested(serialParams);
}

void SerialPortConnectConfigWidget::onStatusChanged(const QString& status, int connectStatus)
{
    CMessageBox::showToast(status);
    if (connectStatus != 1 && connectStatus != 0) return;
    m_isConnected = connectStatus == 1;
    m_pConnectButton->setProperty("connected", m_isConnected);
    m_pConnectButton->setText(m_isConnected ? "断开" : "连接");
    m_pConnectButton->style()->unpolish(m_pConnectButton);
    m_pConnectButton->style()->polish(m_pConnectButton);
    m_pConnectButton->update();
}

void SerialPortConnectConfigWidget::onUpdatePortComboBox(const QList<QSerialPortInfo>& newPorts)
{
    // --- 准备数据 ---
    // a. 获取当前下拉框中已有的所有端口名
    QSet<QString> currentPortNames;
    for (int i = 0; i < m_pPortComboBox->count(); ++i)
    {
        currentPortNames.insert(m_pPortComboBox->itemText(i));
    }
    // b. 获取系统最新报告的所有可用端口名
    QSet<QString> newPortNames;
    QHash<QString, QSerialPortInfo> newPortInfoMap; // 用于快速查找端口信息
    for (const QSerialPortInfo& port : newPorts)
    {
        newPortNames.insert(port.portName());
        newPortInfoMap.insert(port.portName(), port);
    }
    // --- 计算差异 ---
    // c. 找出需要从下拉框中移除的端口 (存在于当前列表，但不存在于新列表中)
    QSet<QString> portsToRemove = currentPortNames - newPortNames;
    // d. 找出需要添加到下拉框的新端口 (存在于新列表，但不存在于当前列表中)
    QSet<QString> portsToAdd = newPortNames - currentPortNames;
    // --- 执行UI更新 ---
    // 在批量修改UI前，先阻止信号发射，防止不必要的副作用
    m_pPortComboBox->blockSignals(true);
    // e. 移除已消失的端口
    for (const QString& portName : portsToRemove)
    {
        // 查找要移除项的索引
        int index = m_pPortComboBox->findText(portName);
        if (index != -1)
        {
            m_pPortComboBox->removeItem(index);
        }
    }
    // f. 添加新出现的端口
    for (const QString& portName : portsToAdd)
    {
        m_pPortComboBox->addItem(portName, QVariant::fromValue(newPortInfoMap.value(portName)));
    }
    // 恢复信号发射
    m_pPortComboBox->blockSignals(false);
}

// 私有方法
void SerialPortConnectConfigWidget::setUI()
{
    this->setAttribute(Qt::WA_StyledBackground);
    // 创建控件
    this->createComponents();
    // 创建布局
    this->createLayout();
    // 信号处理
    this->connectSignals();
}

void SerialPortConnectConfigWidget::createComponents()
{
    // 初始化标签
    m_pPortLabel = new QLabel("端口:", this);
    m_pPortLabel->setObjectName("portLabel");
    m_pBaudRateLabel = new QLabel("波特率:", this);
    m_pBaudRateLabel->setObjectName("baudRateLabel");
    m_pDataBitsLabel = new QLabel("数据位:", this);
    m_pDataBitsLabel->setObjectName("dataBitsLabel");
    m_pStopBitsLabel = new QLabel("停止位:", this);
    m_pStopBitsLabel->setObjectName("stopBitsLabel");
    m_pParityLabel = new QLabel("校验位:", this);
    m_pParityLabel->setObjectName("parityLabel");
    m_pFlowControlLabel = new QLabel("流控制:", this);
    m_pFlowControlLabel->setObjectName("flowControlLabel");

    // 初始化下拉框
    m_pPortComboBox = new QComboBox(this);
    m_pPortComboBox->setObjectName("portComboBox");
    m_pPortComboBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_pBaudRateComboBox = new QComboBox(this);
    m_pBaudRateComboBox->setObjectName("baudRateComboBox");
    m_pBaudRateComboBox->setEditable(true);
    m_pBaudRateComboBox->setValidator(new QIntValidator(1, 4000000, m_pBaudRateComboBox));
    m_pDataBitsComboBox = new QComboBox(this);
    m_pDataBitsComboBox->setObjectName("dataBitsComboBox");
    m_pStopBitsComboBox = new QComboBox(this);
    m_pStopBitsComboBox->setObjectName("stopBitsComboBox");
    m_pParityComboBox = new QComboBox(this);
    m_pParityComboBox->setObjectName("parityComboBox");
    m_pFlowControlComboBox = new QComboBox(this);
    m_pFlowControlComboBox->setObjectName("flowControlComboBox");

    // 创建连接按钮
    m_pConnectButton = new QPushButton("连接", this);
    m_pConnectButton->setObjectName("connectButton");
    m_pConnectButton->setCursor(Qt::PointingHandCursor);

    // 安装事件过滤器
    m_pPortComboBox->installEventFilter(this);
    m_pBaudRateComboBox->installEventFilter(this);
    m_pDataBitsComboBox->installEventFilter(this);
    m_pStopBitsComboBox->installEventFilter(this);
    m_pParityComboBox->installEventFilter(this);
    m_pFlowControlComboBox->installEventFilter(this);

    // 属性设置
    this->componentPropertySettings();
}

void SerialPortConnectConfigWidget::componentPropertySettings()
{
    // 添加波特率选项
    SerialPortSettings::setSerialPortComboBox(m_pBaudRateComboBox, SerialPortSettings::getBaudRateOptions(),
                                              QVariant::fromValue(QSerialPort::Baud9600));
    // 添加数据位选项
    SerialPortSettings::setSerialPortComboBox(m_pDataBitsComboBox, SerialPortSettings::getDataBitsOptions(),
                                              QVariant::fromValue(QSerialPort::Data8));
    // 添加停止位选项
    SerialPortSettings::setSerialPortComboBox(m_pStopBitsComboBox, SerialPortSettings::getStopBitsOptions(),
                                              QVariant::fromValue(QSerialPort::OneStop));
    // 添加校验位选项
    SerialPortSettings::setSerialPortComboBox(m_pParityComboBox, SerialPortSettings::getParityOptions(),
                                              QVariant::fromValue(QSerialPort::NoParity));
    // 添加流控制选项
    SerialPortSettings::setSerialPortComboBox(m_pFlowControlComboBox, SerialPortSettings::getFlowControlOptions(),
                                              QVariant::fromValue(QSerialPort::NoFlowControl));
}

void SerialPortConnectConfigWidget::createLayout()
{
    // 创建网格布局
    m_pMainLayout = new QGridLayout(this);
    // 添加到布局
    int row = 0;
    m_pMainLayout->addWidget(m_pPortLabel, row, 0);
    m_pMainLayout->addWidget(m_pPortComboBox, row++, 1);

    m_pMainLayout->addWidget(m_pBaudRateLabel, row, 0);
    m_pMainLayout->addWidget(m_pBaudRateComboBox, row++, 1);

    m_pMainLayout->addWidget(m_pDataBitsLabel, row, 0);
    m_pMainLayout->addWidget(m_pDataBitsComboBox, row++, 1);

    m_pMainLayout->addWidget(m_pStopBitsLabel, row, 0);
    m_pMainLayout->addWidget(m_pStopBitsComboBox, row++, 1);

    m_pMainLayout->addWidget(m_pParityLabel, row, 0);
    m_pMainLayout->addWidget(m_pParityComboBox, row++, 1);

    // 流控制行
    m_pMainLayout->addWidget(m_pFlowControlLabel, row, 0);
    m_pMainLayout->addWidget(m_pFlowControlComboBox, row++, 1);

    // 连接按钮 (跨两列)
    m_pMainLayout->addWidget(m_pConnectButton, row, 0, 1, 2);

    // 设置布局
    m_pMainLayout->setVerticalSpacing(10);
    m_pMainLayout->setHorizontalSpacing(15);
    m_pMainLayout->setContentsMargins(15, 15, 15, 15);
}

void SerialPortConnectConfigWidget::connectSignals()
{
    this->connect(m_pConnectButton, &QPushButton::clicked, this,
                  &SerialPortConnectConfigWidget::onConnectButtonClicked);
    this->connect(this, &SerialPortConnectConfigWidget::startConnectionRequested, SerialPortManager::getInstance(),
                  &SerialPortManager::openSerialPort);
    this->connect(this, &SerialPortConnectConfigWidget::stopConnectionRequested, SerialPortManager::getInstance(),
                  &SerialPortManager::closeSerialPort);
    this->connect(SerialPortManager::getInstance(), &SerialPortManager::statusChanged, this,
                  &SerialPortConnectConfigWidget::onStatusChanged);
    this->connect(this, &SerialPortConnectConfigWidget::availablePortsUpdated, this,
                  &SerialPortConnectConfigWidget::onUpdatePortComboBox);
}

void SerialPortConnectConfigWidget::detectionAvailablePorts()
{
    while (!ThreadPoolManager::isShutdownRequested())
    {
        // 1. 在后台线程中获取当前系统所有可用的串口
        const auto ports = QSerialPortInfo::availablePorts();
        // 2. 发射信号，将最新的端口列表作为“包裹”发送给主线程
        emit availablePortsUpdated(ports);
        // 3. 每隔1秒检查一次
        QThread::msleep(1000);
    }
}
