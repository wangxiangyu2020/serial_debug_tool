/**
  ******************************************************************************
  * @file           : ModbusDisplayWidget.cpp
  * @author         : wangxiangyu
  * @brief          : None
  * @attention      : None
  * @date           : 2025/9/12
  ******************************************************************************
  */

#include "ui/ModbusDisplayWidget.h"

ModbusDisplayWidget::ModbusDisplayWidget(QWidget* parent)
    : QWidget(parent)
{
    this->setUI();
    StyleLoader::loadStyleFromFile(this, ":/resources/qss/modbus_display_widget.qss");
}

void ModbusDisplayWidget::onReadButtonClicked()
{
    // 从UI获取参数
    int slaveId = m_pReadSlaveIdSpinBox->value();
    // 从QComboBox获取用户输入的地址文本并转为整数，例如 "40001" -> 40001
    int startAddr_UI = m_pReadAddrComboBox->currentText().toInt();
    int quantity = m_pReadQtySpinBox->value();
    // 我们约定，保持寄存器的地址必须从40001开始
    if (startAddr_UI < 40001) {
        // 在日志中给出清晰的错误提示
        m_pLogTextEdit->appendPlainText("错误: 读取地址无效，保持寄存器地址应从40001开始。");
        return; // 中断操作
    }
    // Modbus协议帧中的地址是从0开始的，所以需要减去偏移量
    int startAddr_Protocol = startAddr_UI - 40001; // 例如: 40001 -> 0, 40002 -> 1
    // 将转换后的【协议地址】传递给控制器
    m_pModbusController->readHoldingRegister(slaveId, startAddr_Protocol, quantity);
    // 日志中仍然显示用户输入的UI地址，这样更直观
    QString logMsg = QString("发送读取请求 -> 从站: %1, 地址: %2, 数量: %3")
                         .arg(slaveId).arg(startAddr_UI).arg(quantity);
    m_pLogTextEdit->appendPlainText(logMsg);
}

void ModbusDisplayWidget::onWriteButtonClicked()
{
    /* TODO: 实现写入逻辑 */
}

void ModbusDisplayWidget::onPollButtonToggled(bool checked)
{
    if (checked) { m_pPollButton->setText("停止轮询"); }
    else { m_pPollButton->setText("开始轮询"); }
    // TODO: 实现轮询控制逻辑
}

void ModbusDisplayWidget::onConfigTagsButtonClicked()
{
    TagManagerDialog dialog(m_modbusTags, this);
    if (dialog.exec() == QDialog::Accepted)
    {
        m_pTagModel->layoutRefresh();
    }
}

void ModbusDisplayWidget::onClearLogButtonClicked() { m_pLogTextEdit->clear(); }

void ModbusDisplayWidget::onModbusDataReady(int startAddress, const QList<quint16>& values)
{
    for (int i = 0; i < values.size(); ++i)
    {
        int currentAddress = startAddress + i;
        // 查找 m_modbusTags 中地址匹配的 tag
        for (int j = 0; j < m_modbusTags.size(); ++j)
        {
            if (m_modbusTags[j].address == currentAddress)
            {
                m_modbusTags[j].rawValue = values[i];
                // TODO: 根据数据类型、增益、偏移量等计算 currentValue
                m_modbusTags[j].currentValue = values[i] * m_modbusTags[j].gain + m_modbusTags[j].offset;
                // 通知模型更新这一行的数据
                m_pTagModel->valueUpdate(j);
                break; // 找到后就跳出内层循环
            }
        }
    }
    // 在日志中显示成功信息
    m_pLogTextEdit->appendPlainText(QString("成功接收并解析了 %1 个寄存器值。").arg(values.size()));
}

void ModbusDisplayWidget::setUI()
{
    // 初始化modbus控制器
    m_pModbusController = new ModbusController(SerialPortManager::getInstance(), this);
    this->setAttribute(Qt::WA_StyledBackground);
    this->createComponents();
    this->createLayout();
    this->connectSignals();
    m_pTagModel = new ModbusTagModel(&m_modbusTags, this);
    m_pDisplayTableView->setModel(m_pTagModel);
}

void ModbusDisplayWidget::createComponents()
{
    // --- 1. 创建分组框 (图标已移除) ---
    m_pOperationsGroupBox = new QGroupBox("读写操作");
    m_pControllerGroupBox = new QGroupBox("控制与配置");
    m_pDisplayGroupBox = new QGroupBox("点位数据显示");
    m_pLogGroupBox = new QGroupBox("通信日志");

    // --- 2. 创建"操作区"内的控件 (图标已移除) ---
    // 读取
    m_pReadLabel = new QLabel("读:");
    m_pReadSlaveIdSpinBox = new QSpinBox(this);
    m_pReadSlaveIdSpinBox->setRange(1, 247);
    m_pReadSlaveIdSpinBox->setFixedWidth(60);
    m_pReadSlaveIdSpinBox->setFocusPolicy(Qt::StrongFocus);
    m_pReadAddrLabel = new QLabel("起始:");
    m_pReadAddrComboBox = new QComboBox(this);
    m_pReadAddrComboBox->setEditable(true);
    m_pReadAddrComboBox->addItems({"0", "1", "10", "100", "1000", "40001", "40002"});
    m_pReadAddrComboBox->setFixedWidth(120);
    m_pReadQtyLabel = new QLabel("数量:");
    m_pReadQtySpinBox = new QSpinBox(this);
    m_pReadQtySpinBox->setRange(1, 125);
    m_pReadQtySpinBox->setFixedWidth(60);
    m_pReadQtySpinBox->setFocusPolicy(Qt::StrongFocus);
    m_pReadButton = new QPushButton("手动读取", this);

    m_pSeparatorLine = new QFrame(this);
    m_pSeparatorLine->setFrameShape(QFrame::HLine);
    m_pSeparatorLine->setFrameShadow(QFrame::Sunken);

    // 写入
    m_pWriteLabel = new QLabel("写:");
    m_pWriteSlaveIdSpinBox = new QSpinBox(this);
    m_pWriteSlaveIdSpinBox->setRange(1, 247);
    m_pWriteSlaveIdSpinBox->setFixedWidth(60);
    m_pWriteSlaveIdSpinBox->setFocusPolicy(Qt::StrongFocus);
    m_pWriteAddrLabel = new QLabel("地址:");
    m_pWriteAddrComboBox = new QComboBox(this);
    m_pWriteAddrComboBox->setEditable(true);
    m_pWriteAddrComboBox->addItems({"0", "1", "10", "100", "1000", "40001", "40002"});
    m_pWriteAddrComboBox->setFixedWidth(120);
    m_pWriteValueLabel = new QLabel("值:");
    m_pWriteValueLineEdit = new QLineEdit(this);
    m_pWriteFuncCodeLabel = new QLabel("功能码:");
    m_pWriteFuncCodeComboBox = new QComboBox(this);
    m_pWriteFuncCodeComboBox->addItem("06: 写单个寄存器", 0x06);
    m_pWriteFuncCodeComboBox->addItem("10: 写多个寄存器", 0x10);
    m_pWriteDataTypeLabel = new QLabel("类型:");
    m_pWriteDataTypeComboBox = new QComboBox(this);
    m_pWriteDataTypeComboBox->addItem("16位整数(x10)"); // 示例
    m_pWriteButton = new QPushButton("写入", this);

    // --- 3. 创建"控制与配置区"内的控件 (图标已移除) ---
    m_pPollButton = new QPushButton("开始轮询", this);
    m_pPollButton->setCheckable(true);
    m_pPollButton->setObjectName("pollButton");
    m_pPollIntervalLabel = new QLabel("轮询间隔:");
    m_pPollIntervalSpinBox = new QSpinBox(this);
    m_pPollIntervalSpinBox->setRange(100, 60000);
    m_pPollIntervalSpinBox->setValue(1000);
    m_pPollIntervalSpinBox->setSuffix(" ms");
    m_pPollIntervalSpinBox->setFixedWidth(90);
    m_pPollIntervalSpinBox->setFocusPolicy(Qt::StrongFocus);
    m_pConfigTagsButton = new QPushButton("配置点位表", this);

    // --- 4. 创建"数据显示区"内的控件 ---
    m_pDisplayTableView = new QTableView(this);
    m_pDisplayTableView->horizontalHeader()->setStretchLastSection(true);
    QHeaderView* verticalHeader = m_pDisplayTableView->verticalHeader();
    verticalHeader->setSectionResizeMode(QHeaderView::ResizeToContents);

    // --- 5. 创建"日志区"内的控件 ---
    m_pLogTextEdit = new QPlainTextEdit(this);
    m_pLogTextEdit->setReadOnly(true);
    m_pClearLogButton = new QPushButton("清除日志", this);
}

void ModbusDisplayWidget::createLayout()
{
    // --- 1. 设置顶层主布局 ---
    m_pMainLayout = new QVBoxLayout(this);
    m_pMainLayout->addWidget(m_pOperationsGroupBox);
    m_pMainLayout->addWidget(m_pControllerGroupBox);
    m_pMainLayout->addWidget(m_pDisplayGroupBox);
    m_pMainLayout->addWidget(m_pLogGroupBox);
    // 设置拉伸因子，让数据显示区和日志区可以自动拉伸
    m_pMainLayout->setStretchFactor(m_pDisplayGroupBox, 1);
    m_pMainLayout->setStretchFactor(m_pLogGroupBox, 1);


    // --- 2. 填充"操作区" GroupBox (布局已调整) ---
    m_pOperationsLayout = new QGridLayout(m_pOperationsGroupBox);

    // 读操作行
    auto readLayout = new QHBoxLayout();
    readLayout->addWidget(m_pReadLabel);
    readLayout->addWidget(m_pReadSlaveIdSpinBox);
    readLayout->addWidget(m_pReadAddrLabel);
    readLayout->addWidget(m_pReadAddrComboBox);
    readLayout->addWidget(m_pReadQtyLabel);
    readLayout->addWidget(m_pReadQtySpinBox);
    readLayout->addStretch(); // 添加弹簧
    readLayout->addWidget(m_pReadButton);
    m_pOperationsLayout->addLayout(readLayout, 0, 0);

    // 分隔线
    m_pOperationsLayout->addWidget(m_pSeparatorLine, 1, 0);

    // 写操作行
    auto writeLayout = new QHBoxLayout();
    writeLayout->addWidget(m_pWriteLabel);
    writeLayout->addWidget(m_pWriteSlaveIdSpinBox);
    writeLayout->addWidget(m_pWriteAddrLabel);
    writeLayout->addWidget(m_pWriteAddrComboBox);
    writeLayout->addWidget(m_pWriteValueLabel);
    writeLayout->addWidget(m_pWriteValueLineEdit);
    writeLayout->addWidget(m_pWriteFuncCodeLabel);
    writeLayout->addWidget(m_pWriteFuncCodeComboBox);
    writeLayout->addWidget(m_pWriteDataTypeLabel);
    writeLayout->addWidget(m_pWriteDataTypeComboBox);
    writeLayout->addStretch(); // 添加弹簧
    writeLayout->addWidget(m_pWriteButton);
    m_pOperationsLayout->addLayout(writeLayout, 2, 0);


    // --- 3. 填充"控制与配置区" GroupBox (重新设计布局) ---
    m_pControllerLayout = new QHBoxLayout(m_pControllerGroupBox);
    m_pControllerLayout->addWidget(m_pPollButton);
    m_pControllerLayout->addWidget(m_pPollIntervalLabel);
    m_pControllerLayout->addWidget(m_pPollIntervalSpinBox);
    m_pControllerLayout->addStretch(); // 弹性空间
    m_pControllerLayout->addWidget(m_pConfigTagsButton);


    // --- 4. 填充"数据显示区" GroupBox (导出按钮已移除) ---
    m_pDisplayLayout = new QGridLayout(m_pDisplayGroupBox);
    m_pDisplayLayout->addWidget(m_pDisplayTableView, 0, 0);


    // --- 5. 填充"日志区" GroupBox (保持不变) ---
    m_pLogLayout = new QVBoxLayout(m_pLogGroupBox);
    m_pLogLayout->addWidget(m_pLogTextEdit);
    m_pLogLayout->addWidget(m_pClearLogButton, 0, Qt::AlignRight);
}

void ModbusDisplayWidget::connectSignals()
{
    this->connect(m_pReadButton, &QPushButton::clicked, this, &ModbusDisplayWidget::onReadButtonClicked);
    this->connect(m_pWriteButton, &QPushButton::clicked, this, &ModbusDisplayWidget::onWriteButtonClicked);
    this->connect(m_pPollButton, &QPushButton::toggled, this, &ModbusDisplayWidget::onPollButtonToggled);
    this->connect(m_pConfigTagsButton, &QPushButton::clicked, this, &ModbusDisplayWidget::onConfigTagsButtonClicked);
    this->connect(m_pClearLogButton, &QPushButton::clicked, this, &ModbusDisplayWidget::onClearLogButtonClicked);
    this->connect(m_pModbusController, &ModbusController::errorOccurred, [this](const QString& errorString)
    {
        CMessageBox::showToast(this, errorString);
    });
    this->connect(m_pModbusController, &ModbusController::dataReady, this, &ModbusDisplayWidget::onModbusDataReady);
}
