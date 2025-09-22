/**
  ******************************************************************************
  * @file           : AddEditModbusTagDialog.cpp
  * @author         : wangxiangyu
  * @brief          : None
  * @attention      : None
  * @date           : 2025/9/13
  ******************************************************************************
  */

#include "ui/AddEditModbusTagDialog.h"

AddEditModbusTagDialog::AddEditModbusTagDialog(ModbusTag& tag, QWidget* parent)
    : QDialog(parent), m_tag(tag)
{
    this->setUI();
}

void AddEditModbusTagDialog::onSaveButtonClicked()
{
    // 在关闭对话框前，将UI上的数据保存回 m_tag 引用
    this->saveTagData();
    // accept() 会关闭对话框并返回 QDialog::Accepted
    this->accept();
}

void AddEditModbusTagDialog::onDataTypeChanged(int index)
{
    auto dataType = m_pDataTypeComboBox->itemData(index).value<ModbusTag::DataType>();

    // 只有当数据类型大于16位时，字节序设置才有意义
    bool isMultiByte = (dataType == ModbusTag::DataType::UInt32
                        || dataType == ModbusTag::DataType::Int32
                        || dataType == ModbusTag::DataType::Float32);

    m_pByteOrderComboBox->setEnabled(isMultiByte);
}

void AddEditModbusTagDialog::setUI()
{
    this->setMinimumSize(350, 364);
    this->createComponents();
    this->createLayout();
    this->connectSignals();
    this->loadTagData(); // 初始化UI显示
}

void AddEditModbusTagDialog::createComponents()
{
    this->setWindowTitle(m_tag.name == "New Tag" ? "添加新点位" : "编辑点位");

    m_pNameLineEdit = new QLineEdit(this);
    m_pSlaveIdSpinBox = new QSpinBox(this);
    m_pSlaveIdSpinBox->setRange(1, 247);
    m_pFunctionCodeComboBox = new QComboBox(this);
    m_pFunctionCodeComboBox->addItem("03: 读保持寄存器", 3);
    m_pFunctionCodeComboBox->addItem("04: 读输入寄存器", 4);
    m_pAddressSpinBox = new QSpinBox(this);
    m_pAddressSpinBox->setRange(0, 65535);

    m_pDataTypeComboBox = new QComboBox(this);
    m_pDataTypeComboBox->addItem("16位无符号整数 (UInt16)", QVariant::fromValue(ModbusTag::DataType::UInt16));
    m_pDataTypeComboBox->addItem("16位有符号整数 (Int16)", QVariant::fromValue(ModbusTag::DataType::Int16));
    m_pDataTypeComboBox->addItem("32位无符号整数 (UInt32)", QVariant::fromValue(ModbusTag::DataType::UInt32));
    m_pDataTypeComboBox->addItem("32位有符号整数 (Int32)", QVariant::fromValue(ModbusTag::DataType::Int32));
    m_pDataTypeComboBox->addItem("32位浮点数 (Float32)", QVariant::fromValue(ModbusTag::DataType::Float32));

    m_pByteOrderComboBox = new QComboBox(this);
    m_pByteOrderComboBox->addItem("大端模式 (ABCD)", QVariant::fromValue(ModbusTag::ByteOrder::BigEndian));
    m_pByteOrderComboBox->addItem("小端模式 (DCBA)", QVariant::fromValue(ModbusTag::ByteOrder::LittleEndian));
    m_pByteOrderComboBox->addItem("大端字节交换 (BADC)", QVariant::fromValue(ModbusTag::ByteOrder::BigEndianByteSwap));
    m_pByteOrderComboBox->addItem("字交换 (CDAB)", QVariant::fromValue(ModbusTag::ByteOrder::LittleEndianByteSwap));

    m_pGainSpinBox = new QDoubleSpinBox(this);
    m_pGainSpinBox->setDecimals(4);
    m_pGainSpinBox->setRange(-1000000, 1000000);
    m_pOffsetSpinBox = new QDoubleSpinBox(this);
    m_pOffsetSpinBox->setDecimals(4);
    m_pOffsetSpinBox->setRange(-1000000, 1000000);

    m_pSaveButton = new QPushButton("保存", this);
    m_pCancelButton = new QPushButton("取消", this);
}

void AddEditModbusTagDialog::createLayout()
{
    auto mainLayout = new QVBoxLayout(this);
    m_pFormLayout = new QFormLayout();

    m_pFormLayout->addRow("点位名称:", m_pNameLineEdit);
    m_pFormLayout->addRow("从站地址:", m_pSlaveIdSpinBox);
    m_pFormLayout->addRow("功能码:", m_pFunctionCodeComboBox);
    m_pFormLayout->addRow("寄存器地址:", m_pAddressSpinBox);
    m_pFormLayout->addRow("数据类型:", m_pDataTypeComboBox);
    m_pFormLayout->addRow("字节序:", m_pByteOrderComboBox);
    m_pFormLayout->addRow("乘法系数 (Gain):", m_pGainSpinBox);
    m_pFormLayout->addRow("加法偏移 (Offset):", m_pOffsetSpinBox);

    auto buttonBox = new QDialogButtonBox();
    buttonBox->addButton(m_pSaveButton, QDialogButtonBox::AcceptRole);
    buttonBox->addButton(m_pCancelButton, QDialogButtonBox::RejectRole);

    mainLayout->addLayout(m_pFormLayout);
    mainLayout->addWidget(buttonBox);
}

void AddEditModbusTagDialog::connectSignals()
{
    this->connect(m_pSaveButton, &QPushButton::clicked, this, &AddEditModbusTagDialog::onSaveButtonClicked);
    this->connect(m_pCancelButton, &QPushButton::clicked, this, &AddEditModbusTagDialog::reject);
    this->connect(m_pDataTypeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
                  &AddEditModbusTagDialog::onDataTypeChanged);
}

void AddEditModbusTagDialog::loadTagData()
{
    m_pNameLineEdit->setText(m_tag.name);
    m_pSlaveIdSpinBox->setValue(m_tag.slaveId);
    m_pFunctionCodeComboBox->setCurrentIndex(m_pFunctionCodeComboBox->findData(m_tag.functionCode));
    m_pAddressSpinBox->setValue(m_tag.address);
    m_pDataTypeComboBox->setCurrentIndex(m_pDataTypeComboBox->findData(QVariant::fromValue(m_tag.dataType)));
    m_pByteOrderComboBox->setCurrentIndex(m_pByteOrderComboBox->findData(QVariant::fromValue(m_tag.byteOrder)));
    m_pGainSpinBox->setValue(m_tag.gain);
    m_pOffsetSpinBox->setValue(m_tag.offset);

    // 初始状态下触发一次，以正确设置字节序控件的可用性
    this->onDataTypeChanged(m_pDataTypeComboBox->currentIndex());
}

void AddEditModbusTagDialog::saveTagData()
{
    m_tag.name = m_pNameLineEdit->text();
    m_tag.slaveId = m_pSlaveIdSpinBox->value();
    m_tag.functionCode = m_pFunctionCodeComboBox->currentData().toInt();
    m_tag.address = m_pAddressSpinBox->value();
    m_tag.dataType = m_pDataTypeComboBox->currentData().value<ModbusTag::DataType>();
    m_tag.byteOrder = m_pByteOrderComboBox->currentData().value<ModbusTag::ByteOrder>();
    m_tag.gain = m_pGainSpinBox->value();
    m_tag.offset = m_pOffsetSpinBox->value();
}
