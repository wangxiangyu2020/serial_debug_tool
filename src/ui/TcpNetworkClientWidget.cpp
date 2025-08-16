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

#include "core/TcpNetworkManager.h"

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

void TcpNetworkClientWidget::onConnectButtonClicked()
{
    if (isConnected)
    {
        m_pServerIpComboBox->setEnabled(true);
        m_pPortLineEdit->setEnabled(true);
        emit stopConnectionRequested();
        return;
    }

    QString ipStr = m_pServerIpComboBox->currentText().trimmed();
    QString portStr = m_pPortLineEdit->text().trimmed();
    // 校验IP地址/主机名
    QHostAddress address(ipStr);
    if (address.isNull() && ipStr.toLower() != "localhost")
    {
        CMessageBox::showToast(this, "IP地址或主机名无效");
        return;
    }
    // 校验端口号
    bool isPortOk;
    quint16 port = portStr.toUShort(&isPortOk);
    if (!isPortOk || port == 0)
    {
        CMessageBox::showToast(this, "端口号必须是 1-65535 之间的数字");
        return;
    }

    m_pServerIpComboBox->setEnabled(false);
    m_pPortLineEdit->setEnabled(false);
    emit startConnectionRequested(ipStr, port);
}

void TcpNetworkClientWidget::onStatusChanged(const QString& status)
{
    m_pStatusLabel->setText(status);
    // 直接根据状态文本判断连接状态
    isConnected = status.contains("已连接");
    if (!isConnected && !status.contains("已断开")) return; // 既不是已连接也不是已断开，直接返回
    m_pConnectButton->setProperty("connected", isConnected);
    m_pConnectButton->setText(isConnected ? "断开" : "连接");
    m_pConnectButton->style()->unpolish(m_pConnectButton);
    m_pConnectButton->style()->polish(m_pConnectButton);
    m_pConnectButton->update();
}

void TcpNetworkClientWidget::onSendButtonClicked()
{
    if (!isConnected)
    {
        CMessageBox::showToast(this, "请先连接服务器");
        return;
    }
    if (m_pSendTextEdit->toPlainText().trimmed().isEmpty())
    {
        CMessageBox::showToast(this, "请输入要发送的数据");
        return;
    }
    emit sendDataRequested(m_pSendTextEdit->toPlainText().toLocal8Bit());
}

void TcpNetworkClientWidget::onDisplayReceiveData(const QString& sourceInfo, const QByteArray& data)
{
    // 暂停重绘以提高性能
    m_pReceiveTextEdit->setUpdatesEnabled(false);
    // 获取当前滚动条位置
    QScrollBar* vScroll = m_pReceiveTextEdit->verticalScrollBar();
    bool atBottom = vScroll->value() == vScroll->maximum();
    QString receivedString = QString::fromUtf8(data).trimmed();
    m_pReceiveTextEdit->appendPlainText(receivedString);
    // 恢复自动滚动（如果启用且之前已在底部）
    if (atBottom) vScroll->setValue(vScroll->maximum());
    // 恢复重绘
    m_pReceiveTextEdit->setUpdatesEnabled(true);
}

void TcpNetworkClientWidget::onSaveDataButtonClicked()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("保存数据"),
                                                    QDir::homePath(),
                                                    tr("文本文件 (*.txt)"));
    if (fileName.isEmpty()) return;
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return;
    QTextStream out(&file);
    out << m_pReceiveTextEdit->toPlainText();
    file.close();
    CMessageBox::showToast(this, "数据已保存至" + fileName);
}

void TcpNetworkClientWidget::onTimedSendCheckBoxClicked(bool status)
{
    if (!status)
    {
        m_pIntervalEdit->setEnabled(true);
        emit stopTimedSendRequested();
        return;
    }
    if (!isConnected)
    {
        CMessageBox::showToast(this, tr("请先连接服务器"));
        m_pTimedSendCheckBox->setChecked(false); // 恢复复选框状态
        return;
    }
    QString textToSend = m_pSendTextEdit->toPlainText(); // 从UI获取要发送的文本
    if (textToSend.trimmed().isEmpty())
    {
        CMessageBox::showToast(this, "请输入要发送的数据");
        m_pTimedSendCheckBox->setChecked(false);
        return;
    }
    m_pIntervalEdit->setEnabled(false);
    double interval = m_pIntervalEdit->text().toDouble();
    emit startTimedSendRequested(interval, textToSend.toLocal8Bit());
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
    m_pServerIpComboBox = new QComboBox(m_pNetworkConfigGroupBox);
    m_pServerIpComboBox->setEditable(true);
    m_pServerIpComboBox->lineEdit()->setPlaceholderText("请选择或输入服务器IP");
    m_pServerIpComboBox->addItem("127.0.0.1");
    m_pServerIpComboBox->addItem("0.0.0.0");
    m_pServerIpComboBox->addItem("192.168.1.1");
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
    m_pSaveDataButton = new QPushButton(tr("保存数据"));
    m_pSaveDataButton->setObjectName("m_pSaveDataButton");
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
    networkCnfgLayout->addWidget(m_pServerIpComboBox);
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
    receiveToolLayout->addWidget(m_pSaveDataButton);
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
    this->connect(m_pConnectButton, &QPushButton::clicked, this, &TcpNetworkClientWidget::onConnectButtonClicked);
    this->connect(this, &TcpNetworkClientWidget::startConnectionRequested, TcpNetworkManager::getInstance(),
                  &TcpNetworkManager::startClient);
    this->connect(this, &TcpNetworkClientWidget::stopConnectionRequested, TcpNetworkManager::getInstance(),
                  &TcpNetworkManager::stop);
    this->connect(TcpNetworkManager::getInstance(), &TcpNetworkManager::clientStatusChanged, this,
                  &TcpNetworkClientWidget::onStatusChanged);
    this->connect(m_pSendButton, &QPushButton::clicked, this, &TcpNetworkClientWidget::onSendButtonClicked);
    this->connect(this, &TcpNetworkClientWidget::sendDataRequested, TcpNetworkManager::getInstance(),
                  &TcpNetworkManager::handleWriteData);
    this->connect(TcpNetworkManager::getInstance(), &TcpNetworkManager::dataReceived, this,
                  &TcpNetworkClientWidget::onDisplayReceiveData);
    this->connect(m_pDisplayTimestampCheckBox, &QCheckBox::clicked, [this](bool status)
    {
        emit displayTimestamp(status);
    });
    this->connect(this, &TcpNetworkClientWidget::displayTimestamp, TcpNetworkManager::getInstance(),
                  &TcpNetworkManager::setDisplayTimestampStatus);
    this->connect(m_pHexDisplayCheckBox, &QCheckBox::clicked, [this](bool status)
    {
        emit hexDisplay(status);
    });
    this->connect(this, &TcpNetworkClientWidget::hexDisplay, TcpNetworkManager::getInstance(),
                  &TcpNetworkManager::setHexDisplayStatus);
    this->connect(m_pSaveDataButton, &QPushButton::clicked, this, &TcpNetworkClientWidget::onSaveDataButtonClicked);
    this->connect(m_pClearDataButton, &QPushButton::clicked, [this]()
    {
        m_pReceiveTextEdit->clear();
    });
    this->connect(m_pHexSendCheckBox, &QCheckBox::clicked, [this](bool status)
    {
        emit hexSend(status);
    });
    this->connect(this, &TcpNetworkClientWidget::hexSend, TcpNetworkManager::getInstance(),
                  &TcpNetworkManager::setHexSendStatus);
    this->connect(m_pTimedSendCheckBox, &QCheckBox::clicked, this, &TcpNetworkClientWidget::onTimedSendCheckBoxClicked);
    this->connect(this, &TcpNetworkClientWidget::startTimedSendRequested, TcpNetworkManager::getInstance(),
                  &TcpNetworkManager::startTimedSend);
    this->connect(this, &TcpNetworkClientWidget::stopTimedSendRequested, TcpNetworkManager::getInstance(),
                  &TcpNetworkManager::stopTimedSend);
}
