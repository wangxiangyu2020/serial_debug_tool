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

void TcpNetworkServerWidget::onListenButtonClicked()
{
    if (isListen)
    {
        m_pServerIpComboBox->setEnabled(true);
        m_pPortLineEdit->setEnabled(true);
        m_pConnectedClientTextEdit->clear();
        while (m_pSendClientComboBox->count() > 1)
        {
            m_pSendClientComboBox->removeItem(1); // 总是移除索引为1的项
        }
        emit stopListenRequested();
        return;
    }

    QString ipStr = m_pServerIpComboBox->currentText().trimmed();
    QString portStr = m_pPortLineEdit->text().trimmed();
    QHostAddress address(ipStr);
    if (address.isNull() && ipStr.toLower() != "localhost")
    {
        CMessageBox::showToast(this, "IP地址或主机名无效");
        return;
    }
    bool isPortOk;
    quint16 port = portStr.toUShort(&isPortOk);
    if (!isPortOk || port == 0)
    {
        CMessageBox::showToast(this, "端口号必须是 1-65535 之间的数字");
        return;
    }

    m_pServerIpComboBox->setEnabled(false);
    m_pPortLineEdit->setEnabled(false);
    emit startListenRequested(ipStr, port);
}

void TcpNetworkServerWidget::onStatusChanged(const QString& status, int connectionCount)
{
    m_pStatusTextLabel->setText(status);
    m_pConnectionCountLabel->setText(QString("当前连接数：%1").arg(connectionCount));
    isListen = status.contains("监听中");
    if (!isListen && (!status.contains("监听失败") && !status.contains("未监听"))) return;
    m_pStartListenButton->setProperty("connected", isListen);
    m_pStartListenButton->setText(isListen ? "停止监听" : "开始监听");
    m_pStartListenButton->style()->unpolish(m_pStartListenButton);
    m_pStartListenButton->style()->polish(m_pStartListenButton);
    m_pStartListenButton->update();
}

void TcpNetworkServerWidget::onClientConnected(const QString& clientInfo, QTcpSocket* clientSocket)
{
    m_pSendClientComboBox->addItem(clientInfo, QVariant::fromValue(clientSocket));
    // 暂停重绘以提高性能
    m_pConnectedClientTextEdit->setUpdatesEnabled(false);
    // 获取当前滚动条位置
    QScrollBar* vScroll = m_pConnectedClientTextEdit->verticalScrollBar();
    bool atBottom = vScroll->value() == vScroll->maximum();
    m_pConnectedClientTextEdit->appendPlainText(clientInfo);
    // 恢复自动滚动（如果启用且之前已在底部）
    if (atBottom) vScroll->setValue(vScroll->maximum());
    // 恢复重绘
    m_pConnectedClientTextEdit->setUpdatesEnabled(true);
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
    m_pServerIpLabel = new QLabel("监听地址: ", m_pNetworkConfigGroupBox);
    m_pServerIpComboBox = new QComboBox(m_pNetworkConfigGroupBox);
    m_pServerIpComboBox->setEditable(true);
    m_pServerIpComboBox->lineEdit()->setPlaceholderText("请选择或输入服务器IP");
    m_pServerIpComboBox->addItem("127.0.0.1");
    m_pServerIpComboBox->addItem("0.0.0.0");
    m_pServerIpComboBox->addItem("192.168.1.1");
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
    m_pConnectionCountLabel = new QLabel(" 当前连接数: 0", this);
    m_pConnectionCountLabel->setObjectName("m_pConnectionCountLabel");
}

void TcpNetworkServerWidget::createLayout()
{
    QHBoxLayout* networkCnfgLayout = new QHBoxLayout();
    networkCnfgLayout->addWidget(m_pServerIpLabel);
    networkCnfgLayout->addWidget(m_pServerIpComboBox);
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
    this->connect(m_pStartListenButton, &QPushButton::clicked, this, &TcpNetworkServerWidget::onListenButtonClicked);
    this->connect(this, &TcpNetworkServerWidget::startListenRequested, TcpNetworkManager::getInstance(),
                  &TcpNetworkManager::startServer);
    this->connect(this, &TcpNetworkServerWidget::stopListenRequested, TcpNetworkManager::getInstance(),
                  &TcpNetworkManager::stop);
    this->connect(TcpNetworkManager::getInstance(), &TcpNetworkManager::serverStatusChanged, this,
                  &TcpNetworkServerWidget::onStatusChanged);
    this->connect(TcpNetworkManager::getInstance(), &TcpNetworkManager::clientConnected, this,
                  &TcpNetworkServerWidget::onClientConnected);
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
