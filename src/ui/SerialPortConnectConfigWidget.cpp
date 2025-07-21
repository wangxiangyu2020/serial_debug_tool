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

SerialPortConnectConfigWidget::SerialPortConnectConfigWidget(QWidget* parent)
    : QWidget(parent), m_pSerialPortManager(new SerialPortManager(this))
{
    this->setUI();
    qRegisterMetaType<QSerialPortInfo>("QSerialPortInfo"); // 注册元类型
    StyleLoader::loadStyleFromFile(this, ":/resources/qss/serial_port_connect_config_widget.qss");
    // 添加自动检测可用端口任务到线程池
    ThreadPoolManager::addTask(
        [this]()
        {
            while (true)
            {
                QThread::sleep(3); // 延时3秒
                QMetaObject::invokeMethod(this, "detectionAvailablePorts", Qt::QueuedConnection);
            }
        },
        []()
        {
        } // 空回调
    );
}

void SerialPortConnectConfigWidget::detectionAvailablePorts()
{
    // 1. 获取当前可用端口
    const auto ports = QSerialPortInfo::availablePorts();
    // 2. 创建当前可用端口名称集合
    QSet<QString> availablePortNames;
    for (const QSerialPortInfo& port : ports)
    {
        availablePortNames.insert(port.portName());
    }
    // 3. 移除已断开的端口（从后往前遍历避免索引问题）
    for (int i = m_pPortComboBox->count() - 1; i >= 0; --i)
    {
        QString portName = m_pPortComboBox->itemText(i);
        if (!availablePortNames.contains(portName))
        {
            m_pPortComboBox->removeItem(i);
        }
    }
    // 4. 添加新端口（避免重复添加）
    for (const QSerialPortInfo& port : ports)
    {
        QString portName = port.portName();
        if (m_pPortComboBox->findText(portName, Qt::MatchExactly) == -1)
        {
            m_pPortComboBox->addItem(portName, QVariant::fromValue(port));
        }
    }
}

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

    // 属性设置
    this->componentPropertySettings();
}

void SerialPortConnectConfigWidget::componentPropertySettings()
{
    this->detectionAvailablePorts();
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
}

void SerialPortConnectConfigWidget::onConnectButtonClicked()
{
    bool isConnected = m_pConnectButton->property("connected").toBool();

    if (!isConnected)
    {
        if (m_pPortComboBox->count() == 0)
        {
            CMessageBox::showToast("请选择串口");
            return;
        }
        QMap<QString, QVariant> serialParams
        {
            {"portInfo", QVariant::fromValue(m_pPortComboBox->currentData().value<QSerialPortInfo>())},
            {"baudRate", m_pBaudRateComboBox->currentData().value<QSerialPort::BaudRate>()},
            {"dataBits", m_pDataBitsComboBox->currentData().value<QSerialPort::DataBits>()},
            {"stopBits", m_pStopBitsComboBox->currentData().value<QSerialPort::StopBits>()},
            {"parity", m_pParityComboBox->currentData().value<QSerialPort::Parity>()},
            {"flowControl", m_pFlowControlComboBox->currentData().value<QSerialPort::FlowControl>()}
        };
        isConnected = m_pSerialPortManager->openSerialPort(serialParams);
    }
    else
    {
        isConnected = !m_pSerialPortManager->closeSerialPort();
    }

    m_pConnectButton->setProperty("connected", isConnected);
    m_pConnectButton->setText(isConnected ? "断开" : "连接");
    m_pConnectButton->style()->unpolish(m_pConnectButton);
    m_pConnectButton->style()->polish(m_pConnectButton);
    m_pConnectButton->update();
}
