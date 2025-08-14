/**
  ******************************************************************************
  * @file           : TcpNetworkClientWidget.cpp
  * @author         : wangxiangyu
  * @brief          : None
  * @attention      : None
  * @date           : 2025/8/14
  ******************************************************************************
  */

#include "ui/TcpNetworkClientWidget.h"

TcpNetworkClientWidget::TcpNetworkClientWidget(QWidget* parent)
    : QWidget(parent)
{
    this->setUI();
    StyleLoader::loadStyleFromFile(this, ":/resources/qss/tcp_network_client_wdiget.qss");
}

bool TcpNetworkClientWidget::eventFilter(QObject* watched, QEvent* event)
{
    if (watched == m_pReceiveTextEdit)
    {
        // 拦截输入法事件
        if (event->type() == QEvent::InputMethod)
        {
            return true; // 阻止输入法输入
        }
        // 拦截按键事件
        if (event->type() == QEvent::KeyPress)
        {
            QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
            // 允许功能键和快捷键操作（如Ctrl+C复制）
            if (keyEvent->key() == Qt::Key_Control ||
                keyEvent->key() == Qt::Key_Shift ||
                keyEvent->key() == Qt::Key_Alt ||
                keyEvent->matches(QKeySequence::Copy) ||
                keyEvent->matches(QKeySequence::SelectAll) ||
                keyEvent->matches(QKeySequence::Delete))
            {
                return false; // 放行功能键和快捷键
            }
            return true; // 阻止其他按键输入
        }
    }
    return QWidget::eventFilter(watched, event);
}

void TcpNetworkClientWidget::setUI()
{
    this->setAttribute(Qt::WA_StyledBackground);
    this->createComponents();
    this->createLayout();
    this->connectSignals();
}

void TcpNetworkClientWidget::createComponents()
{
    // 1. 创建 GroupBox 作为容器
    m_pNetworkConfigGroupBox = new QGroupBox("网络配置", this);
    // 2. 创建内部的所有控件
    m_pServerIpLabel = new QLabel("服务器IP:", m_pNetworkConfigGroupBox);
    m_pServerIpLineEdit = new QLineEdit(m_pNetworkConfigGroupBox);
    m_pServerIpLineEdit->setPlaceholderText("请输入服务器IP地址"); // 设置提示文字
    m_pPortLabel = new QLabel("端口:", m_pNetworkConfigGroupBox);
    m_pPortLineEdit = new QLineEdit(m_pNetworkConfigGroupBox);
    m_pPortLineEdit->setMaximumWidth(100);
    m_pPortLineEdit->setPlaceholderText("请输入端口号");
    // 为端口号输入框添加校验器，只允许输入数字
    m_pPortValidator = new QIntValidator(0, 65535, m_pNetworkConfigGroupBox);
    m_pPortLineEdit->setValidator(m_pPortValidator);
    m_pConnectButton = new QPushButton("连接", m_pNetworkConfigGroupBox);
    m_pConnectButton->setObjectName("m_pConnectButton");

    m_pReceiveDataGroupBox = new QGroupBox("数据接收区", this);
    // 接收文本编辑框
    m_pReceiveTextEdit = new QPlainTextEdit(m_pReceiveDataGroupBox);
    m_pReceiveTextEdit->setReadOnly(true); // 设置为只读
    m_pReceiveTextEdit->setUndoRedoEnabled(false); // 禁用撤销/重做
    m_pReceiveTextEdit->setLineWrapMode(QPlainTextEdit::WidgetWidth); // 设置自动换行模式为按窗口宽度换行
    // 替换系统等宽字体调用
    QFont fixedFont("Consolas", 10);
    if (!QFontDatabase().families().contains("Consolas")) fixedFont = QFont("Courier New", 10);
    fixedFont.setStyleHint(QFont::Monospace);
    m_pReceiveTextEdit->setFont(fixedFont);
    // 安装事件过滤器
    m_pReceiveTextEdit->installEventFilter(this);

    m_pDisplayTimestampCheckBox = new QCheckBox(tr("显示时间戳"));
    m_pHexDisplayCheckBox = new QCheckBox(tr("十六进制显示"));
    m_pClearDataButton = new QPushButton(tr("清空数据"));
    m_pClearDataButton->setObjectName("m_pClearDataButton");

    m_pSendDataGroupBox = new QGroupBox(tr("发送数据"));
    m_pSendTextEdit = new QPlainTextEdit(m_pSendDataGroupBox);
    m_pSendTextEdit->setPlaceholderText("输入要发送的数据...");
    fixedFont.setPointSize(10);
    m_pSendTextEdit->setFont(fixedFont);

    m_pHexSendCheckBox = new QCheckBox(tr("十六进制发送"));
    m_pTimedSendCheckBox = new QCheckBox(tr("定时发送(秒)"));
    m_pIntervalEdit = new QLineEdit();
    m_pIntervalEdit->setText("1.0");
    m_pIntervalEdit->setEnabled(true);
    m_pIntervalEdit->setMaximumWidth(100);
    m_pSendButton = new QPushButton(tr("发送"));
    m_pSendButton->setObjectName("m_pSendButton");

    m_pStatusLabel = new QLabel("状态: 未连接", this);
    m_pStatusLabel->setObjectName("m_pStatusLabel");
}

void TcpNetworkClientWidget::createLayout()
{
    QHBoxLayout* networkCnfgLayout = new QHBoxLayout();
    networkCnfgLayout->addWidget(m_pServerIpLabel);
    networkCnfgLayout->addWidget(m_pServerIpLineEdit);
    networkCnfgLayout->addSpacing(10); // 添加一点固定间距
    networkCnfgLayout->addWidget(m_pPortLabel);
    networkCnfgLayout->addWidget(m_pPortLineEdit);
    networkCnfgLayout->addWidget(m_pConnectButton);
    networkCnfgLayout->addStretch();

    QHBoxLayout* receiveLayout = new QHBoxLayout();
    receiveLayout->addWidget(m_pReceiveTextEdit);

    QHBoxLayout* receiveToolLayout = new QHBoxLayout();
    receiveToolLayout->addWidget(m_pDisplayTimestampCheckBox);
    receiveToolLayout->addWidget(m_pHexDisplayCheckBox);
    receiveToolLayout->addStretch();
    receiveToolLayout->addWidget(m_pClearDataButton);

    QHBoxLayout* sendToolLayout = new QHBoxLayout();
    sendToolLayout->addWidget(m_pHexSendCheckBox);
    sendToolLayout->addWidget(m_pTimedSendCheckBox);
    sendToolLayout->addWidget(m_pIntervalEdit);
    sendToolLayout->addStretch();
    sendToolLayout->addWidget(m_pSendButton);

    QHBoxLayout* sendLayout = new QHBoxLayout();
    sendLayout->addWidget(m_pSendTextEdit);

    m_pNetworkConfigGroupBox->setLayout(networkCnfgLayout);
    m_pReceiveDataGroupBox->setLayout(receiveLayout);
    m_pSendDataGroupBox->setLayout(sendLayout);

    m_pMainLayout = new QVBoxLayout(this);
    m_pMainLayout->addWidget(m_pNetworkConfigGroupBox);
    m_pMainLayout->addWidget(m_pReceiveDataGroupBox);
    m_pMainLayout->addLayout(receiveToolLayout);
    m_pMainLayout->addWidget(m_pSendDataGroupBox);
    m_pMainLayout->addLayout(sendToolLayout);
    m_pMainLayout->addWidget(m_pStatusLabel);
}

void TcpNetworkClientWidget::connectSignals()
{
}
