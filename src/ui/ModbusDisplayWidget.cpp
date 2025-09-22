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
    int startAddress = m_pReadAddrComboBox->currentText().toInt();
    int quantity = m_pReadQtySpinBox->value();
    // 我们约定，保持寄存器的地址必须从40001开始
    if (startAddress < 40001)
    {
        // 在日志中给出清晰的错误提示
        m_pLogTextEdit->appendPlainText("错误: 读取地址无效，保持寄存器地址应从40001开始。");
        return; // 中断操作
    }
    // Modbus协议帧中的地址是从0开始的，所以需要减去偏移量
    int startAddressProtocol = startAddress - 40001; // 例如: 40001 -> 0, 40002 -> 1
    // 将转换后的【协议地址】传递给控制器
    m_pModbusController->readHoldingRegister(slaveId, startAddressProtocol, quantity);
    // 日志中仍然显示用户输入的UI地址，这样更直观
    QString logMsg = QString("发送读取请求 -> 从站: %1, 地址: %2, 数量: %3")
                     .arg(slaveId).arg(startAddress).arg(quantity);
    m_pLogTextEdit->appendPlainText(logMsg);
}

void ModbusDisplayWidget::onWriteButtonClicked()
{
    // 获取UI参数
    int slaveId = m_pWriteSlaveIdSpinBox->value();
    int startAddress = m_pWriteAddrComboBox->currentText().toInt();
    QString valueStr = m_pWriteValueLineEdit->text();
    // 获取功能码
    int functionCode = m_pWriteFuncCodeComboBox->currentData().toInt();
    if (startAddress < 40001)
    {
        m_pLogTextEdit->appendPlainText("错误: 写入地址无效，保持寄存器地址应从40001开始。");
        return;
    }
    int addressProtocol = startAddress - 40001; // 转换为协议地址
    // 根据数据类型，将输入的字符串转换为待发送的寄存器值列表
    bool ok = false;
    QList<quint16> valuesToSend; // 待发送的寄存器值列表
    // 判断数据类型
    int dataTypeIndex = m_pWriteDataTypeComboBox->currentIndex();
    if (dataTypeIndex == 0) // UInt16
    {
        quint16 value = valueStr.toUShort(&ok);
        if (!ok)
        {
            m_pLogTextEdit->appendPlainText("错误: 无效的输入值。请输入有效的16位无符号整数。");
            return;
        }
        valuesToSend.append(value);
    }
    else // 假设其他索引是32位数据类型 (Float, UInt32, etc.)
    {
        // 32位数据必须使用0x10功能码
        if (functionCode != 0x10)
        {
            m_pLogTextEdit->appendPlainText("错误: 写入32位数据类型时，功能码必须选择'10: 写多个寄存器'。");
            return;
        }
        quint32 rwa32Val = 0;
        // 此次需要根据具体的数据类型进行转换
        if (dataTypeIndex == 1)
        {
            rwa32Val = valueStr.toUInt(&ok);
            if (!ok)
            {
                m_pLogTextEdit->appendPlainText("错误: 无效的输入值。请输入有效的32位无符号整数。");
                return;
            }
        }
        else if (dataTypeIndex == 2)
        {
            float floatVal = valueStr.toFloat(&ok);
            if (!ok)
            {
                m_pLogTextEdit->appendPlainText("错误: 无效的输入值。请输入有效的32位浮点数。");
                return;
            }
            std::memcpy(&rwa32Val, &floatVal, sizeof(quint32));
        }
        quint16 highWord = (rwa32Val >> 16) & 0xFFFF;
        quint16 lowWord = rwa32Val & 0xFFFF;
        // 需要获取具体的字节排序
        ModbusTag::ByteOrder byteOrder = m_pWriteByteOrderComboBox->currentData().value<ModbusTag::ByteOrder>();
        // 根据字节序调整高低位的顺序
        switch (byteOrder)
        {
        case ModbusTag::ByteOrder::BigEndian: // ABCD
        case ModbusTag::ByteOrder::BigEndianByteSwap: // BADC (字节交换在Controller层处理更佳)
            valuesToSend.append(highWord);
            valuesToSend.append(lowWord);
            break;
        case ModbusTag::ByteOrder::LittleEndianByteSwap: // CDAB
        case ModbusTag::ByteOrder::LittleEndian: // DCBA
            valuesToSend.append(lowWord);
            valuesToSend.append(highWord);
            break;
        default:
            break;
        }
    }
    // 根据功能码调用对应的Controller函数
    if (functionCode == 0x06)
    {
        if (valuesToSend.size() == 1)
        {
            m_pModbusController->writeSingleRegister(slaveId, addressProtocol, valuesToSend.first());
            m_pLogTextEdit->appendPlainText(QString("发送写单个寄存器请求 -> 从站:%1, 地址:%2, 值:%3")
                                            .arg(slaveId).arg(startAddress).arg(valuesToSend.first()));
        }
        else
        {
            m_pLogTextEdit->appendPlainText("错误: 使用06功能码时只能写入一个16位值。");
        }
    }
    else if (functionCode == 0x10)
    {
        m_pModbusController->writeMultipleRegisters(slaveId, addressProtocol, valuesToSend);
        m_pLogTextEdit->appendPlainText(QString("发送写多个寄存器请求 -> 从站:%1, 地址:%2, 数量:%3")
                                        .arg(slaveId).arg(startAddress).arg(valuesToSend.size()));
    }
}

void ModbusDisplayWidget::onPollButtonToggled(bool checked)
{
    if (checked)
    {
        // 生成请求列表
        this->generateReadRequests();
        if (m_modbusTags.isEmpty())
        {
            m_pLogTextEdit->appendPlainText("提示: 没有配置点位。请先配置点位。");
            m_pPollButton->setChecked(false);
            return;
        }
        if (m_readRequests.isEmpty())
        {
            m_pLogTextEdit->appendPlainText("提示: 未生成有效的读取请求，请先配置点位。");
            m_pPollButton->setChecked(false);
            return;
        }
        m_isPolling = true;
        m_pPollButton->setText("停止轮询");
        m_pOperationsGroupBox->setEnabled(false);
        m_pConfigTagsButton->setEnabled(false);
        // 启动定时器
        int interval = m_pPollIntervalSpinBox->value();
        m_pPollTimer->start(interval);
        this->onPollTimerTimeout();
    }
    else
    {
        m_isPolling = false;
        m_pPollTimer->stop();
        m_pPollButton->setText("开始轮询");
        m_pOperationsGroupBox->setEnabled(true);
        m_pConfigTagsButton->setEnabled(true);
        m_currentRequestIndex = 0; // 重置索引
    }
}

void ModbusDisplayWidget::onPollTimerTimeout()
{
    if (!m_isPolling || m_readRequests.isEmpty())
    {
        m_pPollTimer->stop();
        return;
    }
    // 获取当前请求
    const ModbusReadRequest& request = m_readRequests[m_currentRequestIndex];
    // 调用Controller发送指令
    m_pModbusController->readHoldingRegister(request.slaveId, request.startAddress, request.quantity);
    // 循环发送
    m_currentRequestIndex = (m_currentRequestIndex + 1) % m_readRequests.size();
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
    // 正确的逻辑：遍历我们关心的“点位列表”，而不是“返回的值列表”
    for (int i = 0; i < m_modbusTags.size(); ++i)
    {
        ModbusTag& tag = m_modbusTags[i];
        // 跳过非读取操作
        if (tag.functionCode != 3) continue;
        // 步骤1: 将点位的UI地址(如40001)转换为协议地址(如0)
        int tagProtocolAddress = tag.address - 40001;
        // 步骤2: 判断该点位的协议地址，是否在本次返回的数据帧范围内
        // 例如，请求从地址0开始读10个，那么范围就是[0, 9]。我们要判断tagProtocolAddress是否在这个区间内。
        if (tagProtocolAddress >= startAddress && tagProtocolAddress < (startAddress + values.size()))
        {
            // 步骤3: 计算该点位在 `values` 列表中的准确索引
            int valueIndex = tagProtocolAddress - startAddress;
            // 步骤4: 根据点位的数据类型进行处理
            if (tag.dataType == ModbusTag::DataType::Float32
                || tag.dataType == ModbusTag::DataType::Int32
                || tag.dataType == ModbusTag::DataType::UInt32)
            {
                // 对于32位数据，需要确保有两个寄存器的空间
                if (valueIndex + 1 < values.size())
                {
                    quint16 reg1 = values[valueIndex]; // 低地址寄存器
                    quint16 reg2 = values[valueIndex + 1]; // 高地址寄存器
                    // 重新组合和解释数据
                    quint32 rawValue = ModbusUtils::reassemble32BitValue(reg1, reg2, tag.byteOrder);
                    QVariant finalVariant = ModbusUtils::interpretRaw32BitValue(rawValue, tag.dataType);
                    if (finalVariant.isValid())
                    {
                        double finalValue = finalVariant.toDouble();
                        tag.currentValue = finalValue * tag.gain + tag.offset; // 应用增益和偏移
                    }
                    // 记录原始值用于显示
                    tag.rawValue = QString("0x%1 0x%2")
                                   .arg(reg1, 4, 16, QChar('0')).toUpper()
                                   .arg(reg2, 4, 16, QChar('0')).toUpper();
                    m_pTagModel->valueUpdate(i); // 通知UI更新
                }
            }
            else if (tag.dataType == ModbusTag::DataType::UInt16 || tag.dataType == ModbusTag::DataType::Int16)
            {
                // 对于16位数据，直接取值
                quint16 rawValue = values[valueIndex];
                tag.rawValue = rawValue;
                tag.currentValue = static_cast<double>(rawValue) * tag.gain + tag.offset; // 应用增益和偏移
                m_pTagModel->valueUpdate(i); // 通知UI更新
            }
        }
    }
    // 在日志中显示成功信息
    m_pLogTextEdit->appendPlainText(QString("成功接收并解析了 %1 个寄存器值。").arg(values.size()));
}

void ModbusDisplayWidget::onWriteSuccessful(int functionCode, int address)
{
    QString funcStr = QString::number(functionCode, 16).toUpper(); // 转为大写十六进制
    QString logMsg = QString("从站写入成功! 功能码: 0x%1, 地址: %2").arg(funcStr).arg(address);
    m_pLogTextEdit->appendPlainText(logMsg);
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
    m_pReadAddrComboBox->addItems({"40001", "40002"});
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
    m_pWriteAddrComboBox->addItems({"40001", "40002"});
    m_pWriteAddrComboBox->setFixedWidth(120);
    m_pWriteValueLabel = new QLabel("值:");
    m_pWriteValueLineEdit = new QLineEdit(this);
    m_pWriteFuncCodeLabel = new QLabel("功能码:");
    m_pWriteFuncCodeComboBox = new QComboBox(this);
    m_pWriteFuncCodeComboBox->addItem("06: 写单个寄存器", 0x06);
    m_pWriteFuncCodeComboBox->addItem("10: 写多个寄存器", 0x10);
    m_pWriteDataTypeLabel = new QLabel("类型:");
    m_pWriteDataTypeComboBox = new QComboBox(this);
    m_pWriteDataTypeComboBox->addItem("16位整数"); // 示例
    m_pWriteDataTypeComboBox->addItem("32位整数");
    m_pWriteDataTypeComboBox->addItem("32位小数");
    m_pWriteByteOrderLabel = new QLabel("字节序:");
    m_pWriteByteOrderLabel->hide();
    m_pWriteByteOrderComboBox = new QComboBox(this);
    m_pWriteByteOrderComboBox->addItem("大端模式 (ABCD)", QVariant::fromValue(ModbusTag::ByteOrder::BigEndian));
    m_pWriteByteOrderComboBox->addItem("小端模式 (DCBA)", QVariant::fromValue(ModbusTag::ByteOrder::LittleEndian));
    m_pWriteByteOrderComboBox->addItem("大端字节交换 (BADC)", QVariant::fromValue(ModbusTag::ByteOrder::BigEndianByteSwap));
    m_pWriteByteOrderComboBox->addItem("字交换 (CDAB)", QVariant::fromValue(ModbusTag::ByteOrder::LittleEndianByteSwap));
    m_pWriteByteOrderComboBox->hide();
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
    writeLayout->addWidget(m_pWriteByteOrderLabel);
    writeLayout->addWidget(m_pWriteByteOrderComboBox);
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
    m_pPollTimer = new QTimer(this);
    // 连接定时器的timeout信号到我们新增的槽函数
    this->connect(m_pPollTimer, &QTimer::timeout, this, &ModbusDisplayWidget::onPollTimerTimeout);
    this->connect(m_pReadButton, &QPushButton::clicked, this, &ModbusDisplayWidget::onReadButtonClicked);
    this->connect(m_pWriteButton, &QPushButton::clicked, this, &ModbusDisplayWidget::onWriteButtonClicked);
    this->connect(m_pPollButton, &QPushButton::toggled, this, &ModbusDisplayWidget::onPollButtonToggled);
    this->connect(m_pConfigTagsButton, &QPushButton::clicked, this, &ModbusDisplayWidget::onConfigTagsButtonClicked);
    this->connect(m_pClearLogButton, &QPushButton::clicked, this, &ModbusDisplayWidget::onClearLogButtonClicked);
    this->connect(m_pModbusController, &ModbusController::dataReady, this, &ModbusDisplayWidget::onModbusDataReady);
    this->connect(m_pModbusController, &ModbusController::writeSuccessful, this,
                  &ModbusDisplayWidget::onWriteSuccessful);
    this->connect(m_pModbusController, &ModbusController::errorOccurred, [this](const QString& errorString)
    {
        CMessageBox::showToast(this, errorString);
        if (m_isPolling)
        {
            m_pPollButton->setChecked(false);
            m_isPolling = false;
            m_pPollTimer->stop();
            m_pPollButton->setText("开始轮询");
            m_pOperationsGroupBox->setEnabled(true);
            m_pConfigTagsButton->setEnabled(true);
            m_currentRequestIndex = 0; // 重置索引
        }
    });
    this->connect(m_pWriteDataTypeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [this](int index)
    {
        if (index != 0)
        {
            m_pWriteByteOrderLabel->show();
            m_pWriteByteOrderComboBox->show();
        }
        else
        {
            m_pWriteByteOrderLabel->hide();
            m_pWriteByteOrderComboBox->hide();
        }
    });
}

void ModbusDisplayWidget::generateReadRequests()
{
    m_readRequests.clear();
    if (m_modbusTags.isEmpty()) return;
    // 为了能正确合并，先按从站ID和地址对点位列表进行排序
    std::sort(m_modbusTags.begin(), m_modbusTags.end(), [](const ModbusTag& a, const ModbusTag& b)
    {
        if (a.slaveId != b.slaveId) return a.slaveId < b.slaveId;
        return a.address < b.address;
    });
    // 遍历排序后的点位列表，合并请求
    // 定义一个小的地址间隙，如果两个点位地址差在这个范围内就合并读取
    const int MAX_ADDRESS_GAP = 5;
    // 限制一次读取125个寄存器
    const int MAX_READ_QUANTITY = 125;
    for (const auto& tag : m_modbusTags)
    {
        // 计算点位的协议地址和所需寄存器数量
        int tagProtocolAddr = tag.address - 40001;
        int tagRegsCount = (tag.dataType == ModbusTag::DataType::Float32
                               || tag.dataType == ModbusTag::DataType::Int32
                               || tag.dataType == ModbusTag::DataType::UInt32)
                               ? 2
                               : 1;
        if (m_readRequests.isEmpty() || tag.slaveId != m_readRequests.last().slaveId
            || tagProtocolAddr > (m_readRequests.last().startAddress + m_readRequests.last().quantity + MAX_ADDRESS_GAP)
            || (m_readRequests.last().quantity + (tagProtocolAddr - (m_readRequests.last().startAddress + m_readRequests
                .last().quantity)) + tagRegsCount) > MAX_READ_QUANTITY)
        {
            // 如果满足以下任一条件，就创建一个新的请求：
            // a. 这是第一个点位
            // b. 点位的从站ID与上一个请求不同
            // c. 点位的地址与上一个请求的地址范围差距太大
            // d. 合并后会导致总读取数量超过Modbus协议限制
            m_readRequests.append({tag.slaveId, tagProtocolAddr, tagRegsCount});
        }
        else
        {
            // 否则，扩展上一个请求的范围，以包含当前点位
            int newQuantity = tagProtocolAddr - m_readRequests.last().startAddress + tagRegsCount;
            m_readRequests.last().quantity = newQuantity;
        }
    }
    // 打印出优化后的请求列表，用于调试
    for (const auto& req : m_readRequests)
    {
        qDebug() << QString("Optimized Request -> Slave: %1, StartAddr: %2, Quantity: %3")
                    .arg(req.slaveId).arg(req.startAddress).arg(req.quantity);
    }
}
