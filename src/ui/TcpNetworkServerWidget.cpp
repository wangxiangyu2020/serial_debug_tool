/**
  ******************************************************************************
  * @file           : TcpNetworkServerWidget.cpp
  * @author         : wangxiangyu
  * @brief          : None
  * @attention      : None
  * @date           : 2025/8/14
  ******************************************************************************
  */

#include "ui/TcpNetworkServerWidget.h"

TcpNetworkServerWidget::TcpNetworkServerWidget(QWidget* parent)
    : QWidget(parent)
{
    this->setUI();
    StyleLoader::loadStyleFromFile(this, ":/resources/qss/tcp_network_server_wdiget.qss");
}

bool TcpNetworkServerWidget::eventFilter(QObject* watched, QEvent* event)
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

void TcpNetworkServerWidget::setUI()
{
    this->setAttribute(Qt::WA_StyledBackground);
    this->createComponents();
    this->createLayout();
    this->connectSignals();
}

void TcpNetworkServerWidget::createComponents()
{
    m_pNetworkConfigGroupBox = new QGroupBox("网络配置", this);
    m_pPortLabel = new QLabel("监听端口: ", m_pNetworkConfigGroupBox);
    m_pPortLineEdit = new QLineEdit(m_pNetworkConfigGroupBox);
    m_pPortLineEdit->setPlaceholderText("请输入端口号");
    // 为端口号输入框添加校验器，只允许输入数字
    m_pPortValidator = new QIntValidator(0, 65535, m_pNetworkConfigGroupBox);
    m_pPortLineEdit->setValidator(m_pPortValidator);
    m_pPortLineEdit->setMaximumWidth(100);
    m_pStartListenButton = new QPushButton("开始监听", m_pNetworkConfigGroupBox);
    m_pStartListenButton->setObjectName("m_pStartListenButton");

    m_pConnectedClientGroupBox = new QGroupBox("已连接客户端", this);
    m_pConnectedClientTextEdit = new QPlainTextEdit(m_pConnectedClientGroupBox);
    this->setTextEditProperty(m_pConnectedClientTextEdit);

    m_pReceiveDataGroupBox = new QGroupBox("数据接收区", this);
    m_pReceiveTextEdit = new QPlainTextEdit(m_pReceiveDataGroupBox);
    this->setTextEditProperty(m_pReceiveTextEdit);

    m_pDisplayTimestampCheckBox = new QCheckBox("显示时间戳", this);
    m_pHexDisplayCheckBox = new QCheckBox("十六进制显示", this);
    m_pClearDataButton = new QPushButton("清空数据", this);
    m_pClearDataButton->setObjectName("m_pClearDataButton");

    m_pSendDataGroupBox = new QGroupBox("数据发送区", this);
    m_pSendLabel = new QLabel("发送至: ", m_pSendDataGroupBox);
    m_pSendClientComboBox = new QComboBox(m_pSendDataGroupBox);
    m_pSendClientComboBox->addItem("所有客户端");
    m_pHexSendCheckBox = new QCheckBox("十六进制发送", m_pSendDataGroupBox);
    m_pTimedSendCheckBox = new QCheckBox(tr("定时发送(秒)"));
    m_pIntervalEdit = new QLineEdit(m_pSendDataGroupBox);
    m_pIntervalEdit->setText("1.0");
    m_pIntervalEdit->setEnabled(true);
    m_pIntervalEdit->setMaximumWidth(100);

    m_pSendTextEdit = new QPlainTextEdit(m_pSendDataGroupBox);
    m_pSendTextEdit->setPlaceholderText("输入要发送的数据...");

    m_pSendButton = new QPushButton(tr("发送"));
    m_pSendButton->setObjectName("m_pSendButton");

    m_pStatusTextLabel = new QLabel("状态: 未监听", this);
    m_pStatusTextLabel->setObjectName("m_pStatusTextLabel");
    m_pStatusPortLabel = new QLabel("端口:",this);
    m_pStatusPortLabel->setObjectName("m_pStatusPortLabel");
    m_pConnectionCountLabel = new QLabel(" | 当前连接数: ",this);
    m_pConnectionCountLabel->setObjectName("m_pConnectionCountLabel");
}

void TcpNetworkServerWidget::createLayout()
{
    QHBoxLayout* networkCnfgLayout = new QHBoxLayout();
    networkCnfgLayout->addWidget(m_pPortLabel);
    networkCnfgLayout->addWidget(m_pPortLineEdit);
    networkCnfgLayout->addWidget(m_pStartListenButton);
    networkCnfgLayout->addStretch();

    m_pNetworkConfigGroupBox->setLayout(networkCnfgLayout);

    QHBoxLayout* connectedLayout = new QHBoxLayout();
    connectedLayout->addWidget(m_pConnectedClientTextEdit);
    QHBoxLayout* receiveLayout = new QHBoxLayout();
    receiveLayout->addWidget(m_pReceiveTextEdit);

    m_pConnectedClientGroupBox->setLayout(connectedLayout);
    m_pReceiveDataGroupBox->setLayout(receiveLayout);

    QHBoxLayout* connectedAndReceiveLayout = new QHBoxLayout();
    connectedAndReceiveLayout->addWidget(m_pConnectedClientGroupBox, 3);
    connectedAndReceiveLayout->addWidget(m_pReceiveDataGroupBox, 7);

    QHBoxLayout* receiveToolLayout = new QHBoxLayout();
    receiveToolLayout->addStretch();
    receiveToolLayout->addWidget(m_pDisplayTimestampCheckBox);
    receiveToolLayout->addWidget(m_pHexDisplayCheckBox);
    receiveToolLayout->addWidget(m_pClearDataButton);

    QHBoxLayout* sendFirstHLayout = new QHBoxLayout();
    sendFirstHLayout->addWidget(m_pSendLabel);
    sendFirstHLayout->addWidget(m_pSendClientComboBox);
    QHBoxLayout* sendSecondHLayout = new QHBoxLayout();
    sendSecondHLayout->addWidget(m_pTimedSendCheckBox);
    sendSecondHLayout->addWidget(m_pIntervalEdit);
    QVBoxLayout* sendVLayout = new QVBoxLayout();
    sendVLayout->addLayout(sendFirstHLayout);
    sendVLayout->addLayout(sendSecondHLayout);
    sendVLayout->addWidget(m_pHexSendCheckBox);
    QHBoxLayout* sendLayout = new QHBoxLayout();
    sendLayout->addLayout(sendVLayout, 2);
    sendLayout->addWidget(m_pSendTextEdit, 8);

    m_pSendDataGroupBox->setLayout(sendLayout);

    QHBoxLayout* sendToolLayout = new QHBoxLayout();
    sendToolLayout->addStretch();
    sendToolLayout->addWidget(m_pSendButton);

    QHBoxLayout* statusLayout = new QHBoxLayout();
    statusLayout->addWidget(m_pStatusTextLabel);
    statusLayout->addWidget(m_pStatusPortLabel);
    statusLayout->addWidget(m_pConnectionCountLabel);
    statusLayout->addStretch();


    m_pMainLayout = new QVBoxLayout(this);
    m_pMainLayout->addWidget(m_pNetworkConfigGroupBox);
    m_pMainLayout->addLayout(connectedAndReceiveLayout);
    m_pMainLayout->addLayout(receiveToolLayout);
    m_pMainLayout->addWidget(m_pSendDataGroupBox);
    m_pMainLayout->addLayout(sendToolLayout);
    m_pMainLayout->addLayout(statusLayout);
}

void TcpNetworkServerWidget::connectSignals()
{
}

void TcpNetworkServerWidget::setTextEditProperty(QPlainTextEdit* textEdit)
{
    textEdit->setReadOnly(true); // 设置为只读
    textEdit->setUndoRedoEnabled(false); // 禁用撤销/重做
    textEdit->setLineWrapMode(QPlainTextEdit::WidgetWidth); // 设置自动换行模式为按窗口宽度换行
    // 替换系统等宽字体调用
    QFont fixedFont("Consolas", 10);
    if (!QFontDatabase().families().contains("Consolas")) fixedFont = QFont("Courier New", 10);
    fixedFont.setStyleHint(QFont::Monospace);
    textEdit->setFont(fixedFont);
    // 安装事件过滤器
    textEdit->installEventFilter(this);
}
