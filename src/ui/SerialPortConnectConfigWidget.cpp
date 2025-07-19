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
#include "utils/StyleLoader.h"

SerialPortConnectConfigWidget::SerialPortConnectConfigWidget(QWidget* parent)
    : QWidget(parent)
{
    this->setUI();
    StyleLoader::loadStyleFromFile(this, ":/resources/qss/serial_port_connect_config_widget.qss");
}

void SerialPortConnectConfigWidget::setUI()
{
    this->setAttribute(Qt::WA_StyledBackground);
    // 创建网格布局
    m_pMainLayout = new QGridLayout(this);

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

    // // 添加端口选项 (自动检测可用串口)
    // const auto ports = QSerialPortInfo::availablePorts();
    // for (const QSerialPortInfo& port : ports)
    // {
    //     m_pPortComboBox->addItem(port.portName());
    // }

    // 添加波特率选项
    m_pBaudRateComboBox->addItem("1200", "QSerialPort::Baud1200");
    m_pBaudRateComboBox->addItem("2400", "QSerialPort::Baud2400");
    m_pBaudRateComboBox->addItem("4800", "QSerialPort::Baud4800");
    m_pBaudRateComboBox->addItem("9600", "QSerialPort::Baud9600");
    m_pBaudRateComboBox->addItem("19200", "QSerialPort::Baud19200");
    m_pBaudRateComboBox->addItem("38400", "QSerialPort::Baud38400");
    m_pBaudRateComboBox->addItem("57600", "QSerialPort::Baud57600");
    m_pBaudRateComboBox->addItem("115200", "QSerialPort::Baud115200");
    m_pBaudRateComboBox->setCurrentIndex(3); // 默认选择9600

    // 添加数据位选项
    m_pDataBitsComboBox->addItem("5", "QSerialPort::Data5");
    m_pDataBitsComboBox->addItem("6", "QSerialPort::Data6");
    m_pDataBitsComboBox->addItem("7", "QSerialPort::Data7");
    m_pDataBitsComboBox->addItem("8", "QSerialPort::Data8");
    m_pDataBitsComboBox->setCurrentIndex(3); // 默认选择8位

    // 添加停止位选项
    m_pStopBitsComboBox->addItem("1", "QSerialPort::OneStop");
    m_pStopBitsComboBox->addItem("1.5", "QSerialPort::OneAndHalfStop");
    m_pStopBitsComboBox->addItem("2", "QSerialPort::TwoStop");

    // 添加校验位选项
    m_pParityComboBox->addItem("无", "QSerialPort::NoParity");
    m_pParityComboBox->addItem("奇校验", "QSerialPort::OddParity");
    m_pParityComboBox->addItem("偶校验", "QSerialPort::EvenParity");

    // 添加流控制选项（5个选项）
    m_pFlowControlComboBox->addItem("无", "QSerialPort::NoFlowControl");
    m_pFlowControlComboBox->addItem("硬件RTS/CTS", "QSerialPort::HardwareControl");
    m_pFlowControlComboBox->addItem("硬件DSR/DTR", "QSerialPort::HardwareControl"); // 注意：Qt使用同一枚举
    m_pFlowControlComboBox->addItem("软件XON/XOFF", "QSerialPort::SoftwareControl");
    m_pFlowControlComboBox->addItem("自定义流控制", "QSerialPort::UnknownFlowControl");
    m_pFlowControlComboBox->setCurrentIndex(0); // 默认无流控制

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
