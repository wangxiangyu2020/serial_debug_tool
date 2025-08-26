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

QCheckBox* TcpNetworkServerWidget::requestScriptReceiveCheckBox()
{
    return m_pScriptReceiveCheckBox;
}

void TcpNetworkServerWidget::onApplyState(const NetworkModeState& state)
{
    m_currentState = state;

    // 应用状态到UI控件
    m_pDisplayTimestampCheckBox->setChecked(state.displayTimestamp);
    m_pHexDisplayCheckBox->setChecked(state.hexDisplay);
    m_pHexSendCheckBox->setChecked(state.hexSend);

    emit displayTimestamp(state.displayTimestamp);
    emit hexDisplay(state.hexDisplay);
    emit hexSend(state.hexSend);
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
        m_pTimedSendCheckBox->setChecked(false); // 恢复复选框状态
        m_pIntervalEdit->setEnabled(true);
        m_pSendClientComboBox->setEnabled(true);
        m_pScriptReceiveCheckBox->setChecked(false);
        emit stopTimedSendRequested();
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
    // 监听状态通知数据管理器
    emit tcpNetworkServerListen(isListen);
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

void TcpNetworkServerWidget::onClientDisconnected(const QString& clientInfo, QTcpSocket* clientSocket)
{
    int index = m_pSendClientComboBox->findData(QVariant::fromValue(clientSocket));
    if (index != -1) m_pSendClientComboBox->removeItem(index);
    // 从文本显示区移除
    // 这比添加要复杂一些，因为需要查找并删除特定行
    QString currentText = m_pConnectedClientTextEdit->toPlainText();
    // 使用 QRegularExpression 确保整行匹配，避免部分匹配的错误
    // 例如，避免 "192.168.1.1" 错误地匹配到 "192.168.1.10"
    QRegularExpression lineRegex(QRegularExpression::escape(clientInfo) + "(\\r\\n|\\n|\\r|$)");
    currentText.remove(lineRegex);
    // 清理可能产生的多余空行
    currentText = currentText.trimmed();
    m_pConnectedClientTextEdit->setPlainText(currentText);
}

void TcpNetworkServerWidget::onDisplayTimestampChanged(bool status)
{
    m_currentState.displayTimestamp = status;
    emit displayTimestamp(status);
    emit stateChanged(m_currentState.displayTimestamp, m_currentState.hexDisplay, m_currentState.hexSend);
}

void TcpNetworkServerWidget::onHexDisplayChanged(bool status)
{
    m_currentState.hexDisplay = status;
    emit hexDisplay(status);
    emit stateChanged(m_currentState.displayTimestamp, m_currentState.hexDisplay, m_currentState.hexSend);
}

void TcpNetworkServerWidget::onHexSendChanged(bool status)
{
    m_currentState.hexSend = status;
    emit hexSend(status);
    emit stateChanged(m_currentState.displayTimestamp, m_currentState.hexDisplay, m_currentState.hexSend);
}

void TcpNetworkServerWidget::onDisplayReceiveData(const QByteArray& data)
{
    if (!isListen) return;
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

void TcpNetworkServerWidget::onSendButtonClicked()
{
    if (!isListen)
    {
        CMessageBox::showToast(tr("请先启动监听！"));
        return;
    }
    if (m_pSendTextEdit->toPlainText().isEmpty())
    {
        CMessageBox::showToast(tr("请输入要发送的数据！"));
        return;
    }
    QString text = m_pSendClientComboBox->currentText();
    if (text == tr("所有客户端"))
    {
        emit sendDataRequested(m_pSendTextEdit->toPlainText().toLocal8Bit());
    }
    else
    {
        QVariant userData = m_pSendClientComboBox->currentData();
        QTcpSocket* clientSocket = userData.value<QTcpSocket*>();
        emit sendDataRequested(m_pSendTextEdit->toPlainText().toLocal8Bit(), clientSocket);
    }
}

void TcpNetworkServerWidget::onTimedSendCheckBoxClicked(bool status)
{
    if (!status)
    {
        m_pIntervalEdit->setEnabled(true);
        m_pSendClientComboBox->setEnabled(true);
        emit stopTimedSendRequested();
        return;
    }
    if (!isListen)
    {
        CMessageBox::showToast(this, tr("请先启动监听"));
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
    m_pSendClientComboBox->setEnabled(false);
    double interval = m_pIntervalEdit->text().toDouble();
    QString text = m_pSendClientComboBox->currentText();
    if (text == tr("所有客户端"))
    {
        emit startTimedSendRequested(interval, textToSend.toLocal8Bit());
    }
    else
    {
        QVariant userData = m_pSendClientComboBox->currentData();
        QTcpSocket* clientSocket = userData.value<QTcpSocket*>();
        emit startTimedSendRequested(interval, textToSend.toLocal8Bit(), clientSocket);
    }
}

void TcpNetworkServerWidget::onSaveDataButtonClicked()
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

void TcpNetworkServerWidget::onShowScriptEditor()
{
    if (m_pScriptEditorDialog->exec() == QDialog::Accepted)
    {
        // 获取编辑后的脚本内容
        QString scriptContent = m_pScriptEditorDialog->getScriptContent();
        // 处理保存逻辑,在这里只获取脚本内容，然后通过信号发送给其他地方进行处理
        emit tcpNetworkServerScriptSaved("server", scriptContent);
    }
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
    m_pSaveDataButton = new QPushButton(tr("保存数据"));
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
    m_pSaveDataButton->setObjectName("m_pSaveDataButton");
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

    // 创建弹框实例
    m_pScriptEditorDialog = new ScriptEditorDialog(this);
    m_pScriptEditorDialog->hide();
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
    receiveToolLayout->addWidget(m_pScriptReceiveCheckBox);
    receiveToolLayout->addWidget(m_pScriptReceiveButton);
    receiveToolLayout->addWidget(m_pScriptHelpButton);
    receiveToolLayout->addWidget(m_pSaveDataButton);
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
    this->connect(TcpNetworkManager::getInstance(), &TcpNetworkManager::clientDisconnected, this,
                  &TcpNetworkServerWidget::onClientDisconnected);
    this->connect(m_pDisplayTimestampCheckBox, &QCheckBox::clicked, this,
                  &TcpNetworkServerWidget::onDisplayTimestampChanged);
    this->connect(m_pHexDisplayCheckBox, &QCheckBox::clicked, this, &TcpNetworkServerWidget::onHexDisplayChanged);
    this->connect(m_pHexSendCheckBox, &QCheckBox::clicked, this, &TcpNetworkServerWidget::onHexSendChanged);
    this->connect(this, &TcpNetworkServerWidget::displayTimestamp, TcpNetworkManager::getInstance(),
                  &TcpNetworkManager::setDisplayTimestampStatus);
    this->connect(this, &TcpNetworkServerWidget::hexDisplay, TcpNetworkManager::getInstance(),
                  &TcpNetworkManager::setHexDisplayStatus);
    this->connect(this, &TcpNetworkServerWidget::hexSend, TcpNetworkManager::getInstance(),
                  &TcpNetworkManager::setHexSendStatus);
    this->connect(PacketProcessor::getInstance(), &PacketProcessor::tcpNetworkReceiveDataChanged, this,
                  &TcpNetworkServerWidget::onDisplayReceiveData);
    this->connect(m_pClearDataButton, &QPushButton::clicked, [this]()
    {
        m_pReceiveTextEdit->clear();
    });
    this->connect(m_pSendButton, &QPushButton::clicked, this, &TcpNetworkServerWidget::onSendButtonClicked);
    this->connect(this, &TcpNetworkServerWidget::sendDataRequested, TcpNetworkManager::getInstance(),
                  &TcpNetworkManager::handleWriteData);
    this->connect(m_pTimedSendCheckBox, &QCheckBox::clicked, this, &TcpNetworkServerWidget::onTimedSendCheckBoxClicked);
    this->connect(this, &TcpNetworkServerWidget::startTimedSendRequested, TcpNetworkManager::getInstance(),
                  &TcpNetworkManager::startTimedSend);
    this->connect(this, &TcpNetworkServerWidget::stopTimedSendRequested, TcpNetworkManager::getInstance(),
                  &TcpNetworkManager::stopTimedSend);
    this->connect(m_pSaveDataButton, &QPushButton::clicked, this, &TcpNetworkServerWidget::onSaveDataButtonClicked);
    this->connect(m_pScriptReceiveButton, &QPushButton::clicked, this, &TcpNetworkServerWidget::onShowScriptEditor);
    this->connect(m_pScriptReceiveCheckBox, &QCheckBox::clicked, [this](bool checked)
    {
        // 只有启用脚本时才会获取脚本内容
        if (checked)
        {
            QString scriptContent = m_pScriptEditorDialog->getScriptContent();
            // 处理脚本启用逻辑，在这里是获取脚本内容后发送到其他地方并且将checked发送到管理器中
            emit tcpNetworkServerScriptSaved("server", scriptContent);
        }
        emit tcpNetworkServerScriptEnabled(checked);
    });
    this->connect(this, &TcpNetworkServerWidget::tcpNetworkServerScriptSaved, ScriptManager::getInstance(),
                  &ScriptManager::onScriptSaved);
    this->connect(this, &TcpNetworkServerWidget::tcpNetworkServerScriptEnabled, ScriptManager::getInstance(),
                  &ScriptManager::onTcpNetworkServerScriptEnabled);
    this->connect(ScriptManager::getInstance(), &ScriptManager::saveStatusChanged,
                  [this](const QString& key, const QString& status)
                  {
                      if (key != "server") return;
                      CMessageBox::showToast(this, status);
                  });
    this->connect(this, &TcpNetworkServerWidget::tcpNetworkServerListen, ScriptManager::getInstance(),
                  &ScriptManager::onTcpNetworkServerListen);
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
