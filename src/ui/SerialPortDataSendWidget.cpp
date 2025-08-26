/**
  ******************************************************************************
  * @file           : SerialPortDataSendWidget.cpp
  * @author         : wangxiangyu
  * @brief          : None
  * @attention      : None
  * @date           : 2025/7/19
  ******************************************************************************
  */

#include "ui/SerialPortDataSendWidget.h"

static QPlainTextEdit* pSendTextEdit = nullptr;

// 构造函数和析构函数
SerialPortDataSendWidget::SerialPortDataSendWidget(QWidget* parent)
    : QWidget(parent)
{
    this->setUI();
    StyleLoader::loadStyleFromFile(this, ":/resources/qss/serial_port_data_send_widget.qss");
}

QPlainTextEdit* SerialPortDataSendWidget::getSendTextEdit()
{
    return pSendTextEdit;
}

// 重写事件处理函数
void SerialPortDataSendWidget::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    // 设置按钮高度
    int textEditHeight = m_pSendTextEdit->height();
    m_pSendButton->setFixedHeight(textEditHeight);
}

// private slots
void SerialPortDataSendWidget::onSendButtonClicked()
{
    auto serialPort = SerialPortManager::getInstance()->getSerialPort();
    if (!serialPort->isOpen())
    {
        CMessageBox::showToast(tr("串口未打开"));
        return;
    }
    // 获取发送数据并传给串口管理器进行处理
    auto sendByteArray = m_pSendTextEdit->toPlainText().toLocal8Bit();
    emit sendDataRequested(sendByteArray);
}

// 私有方法
void SerialPortDataSendWidget::setUI()
{
    this->setAttribute(Qt::WA_StyledBackground);
    this->createComponents();
    this->createLayout();
    this->connectSignals();
}

void SerialPortDataSendWidget::createComponents()
{
    // 发送文本编辑框
    m_pSendTextEdit = new QPlainTextEdit(this);
    m_pSendTextEdit->setPlaceholderText("输入要发送的数据...");
    // 设置文本框高度策略
    const int minHeight = 80;
    m_pSendTextEdit->setMinimumHeight(minHeight);
    m_pSendTextEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    // 使用等宽字体
    QFont fixedFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    fixedFont.setPointSize(10);
    m_pSendTextEdit->setFont(fixedFont);
    pSendTextEdit = m_pSendTextEdit;
    // 发送按钮
    m_pSendButton = new QPushButton(this);
    m_pSendButton->setFixedWidth(100); // 固定宽度
    // 设置按钮文本和图标
    QIcon sendIcon(":/resources/icons/send.svg");
    m_pSendButton->setIcon(sendIcon);
    m_pSendButton->setIconSize(QSize(40, 40));
    // 初始设置按钮高度
    QTimer::singleShot(0, this, [this]()
    {
        // 设置按钮高度
        int textEditHeight = m_pSendTextEdit->height();
        m_pSendButton->setFixedHeight(textEditHeight);
    });
}

void SerialPortDataSendWidget::createLayout()
{
    // 主水平布局
    m_pMainLayout = new QHBoxLayout(this);
    m_pMainLayout->setSpacing(10);
    m_pMainLayout->setContentsMargins(0, 0, 0, 0);
    // 添加到布局
    m_pMainLayout->addWidget(m_pSendTextEdit, 1);
    m_pMainLayout->setSpacing(0);
    m_pMainLayout->addWidget(m_pSendButton, 0);
}

void SerialPortDataSendWidget::connectSignals()
{
    this->connect(m_pSendButton, &QPushButton::clicked, this, &SerialPortDataSendWidget::onSendButtonClicked);
    this->connect(this, &SerialPortDataSendWidget::sendDataRequested, SerialPortManager::getInstance(),
                  &SerialPortManager::handleWriteData);
}
