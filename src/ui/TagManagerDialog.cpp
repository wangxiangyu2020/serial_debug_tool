/**
  ******************************************************************************
  * @file           : TagManagerDialog.cpp
  * @author         : wangxiangyu
  * @brief          : None
  * @attention      : None
  * @date           : 2025/9/13
  ******************************************************************************
  */

#include "ui/TagManagerDialog.h"

TagManagerDialog::TagManagerDialog(QList<ModbusTag>& tags, QWidget* parent)
    : QDialog(parent), m_tags(tags)
{
    this->setUI();
    StyleLoader::loadStyleFromFile(this, ":/resources/qss/tag_manager_dialog.qss");
}

void TagManagerDialog::mousePressEvent(QMouseEvent* event)
{
    // 检查点击位置是否在QListWidget之外
    if (m_pListWidget && !m_pListWidget->geometry().contains(event->pos()))
    {
        // 清除QListWidget的选择和聚焦
        m_pListWidget->clearSelection();
        m_pListWidget->clearFocus();

        // 将焦点设置到对话框本身
        this->setFocus();
    }

    // 调用父类的事件处理
    QDialog::mousePressEvent(event);
}

void TagManagerDialog::onAddButtonClicked()
{
    ModbusTag newTag;
    AddEditModbusTagDialog editDialog(newTag, this);
    if (editDialog.exec() == QDialog::Accepted)
    {
        m_tags.append(newTag);
        this->populateList();
        m_pListWidget->setCurrentItem(nullptr);
    }
    // 如果用户点击了“取消”，则什么也不做
}

void TagManagerDialog::onEditButtonClicked()
{
    QListWidgetItem* currentItem = m_pListWidget->currentItem();
    if (!currentItem)
    {
        // 如果用户没有选择任何项，则弹出提示并返回
        CMessageBox::showToast("请先选择一个要编辑的点位。");
        return;
    }
    int tagIndex = currentItem->data(Qt::UserRole).toInt();
    if (tagIndex < 0 || tagIndex >= m_tags.size())
    {
        CMessageBox::showToast("错误：无效的项目索引。");
        return;
    }
    ModbusTag& tagToEdit = m_tags[tagIndex];
    AddEditModbusTagDialog editDialog(tagToEdit, this);
    if (editDialog.exec() == QDialog::Accepted)
    {
        this->populateList();
        m_pListWidget->setCurrentItem(nullptr);
    }
}

void TagManagerDialog::onDeleteButtonClicked()
{
    QListWidgetItem* currentItem = m_pListWidget->currentItem();
    if (!currentItem)
    {
        // 如果用户没有选择任何项，则弹出提示并返回
        CMessageBox::showToast("请先选择一个要删除的点位。");
        return;
    }
    int tagIndex = currentItem->data(Qt::UserRole).toInt();
    if (tagIndex < 0 || tagIndex >= m_tags.size())
    {
        CMessageBox::showToast("错误：无效的项目索引。");
        return;
    }
    const ModbusTag& tagToDelete = m_tags.at(tagIndex);
    QString question = QString("您确定要删除点位 “%1” 吗？\n(ID:%2, Addr:%3)")
                       .arg(tagToDelete.name)
                       .arg(tagToDelete.slaveId)
                       .arg(tagToDelete.address);
    if (CMessageBox::confirm(this, "确认删除", question))
    {
        m_tags.removeAt(tagIndex);
        populateList();
        m_pListWidget->setCurrentItem(nullptr);
    }
    // 如果用户点击“取消”，则什么也不做
}

void TagManagerDialog::setUI()
{
    this->createComponents();
    this->createLayout();
    this->connectSignals();
    this->populateList();
}

void TagManagerDialog::createComponents()
{
    // 设置窗口属性
    this->setWindowTitle("点位配置表");
    this->setMinimumSize(600, 400);
    // 添加窗口控制按钮
    this->setWindowFlags(Qt::Window
        | Qt::WindowCloseButtonHint
        | Qt::WindowMinimizeButtonHint
        | Qt::WindowMaximizeButtonHint);

    m_pListWidget = new QListWidget(this);
    // 优化QListWidget的聚焦行为
    m_pListWidget->setFocusPolicy(Qt::ClickFocus);
    m_pListWidget->setAttribute(Qt::WA_MacShowFocusRect, false); // 在Mac上隐藏聚焦矩形

    m_pAddButton = new QPushButton("添加", this);
    m_pEditButton = new QPushButton("编辑", this);
    m_pDeleteButton = new QPushButton("删除", this);
    m_pDeleteButton->setObjectName("m_pDeleteButton");
    m_pSaveButton = new QPushButton("保存", this);
}

void TagManagerDialog::createLayout()
{
    m_pMainLayout = new QVBoxLayout(this);
    m_pMainLayout->addWidget(m_pListWidget);
    QHBoxLayout* pButtonLayout = new QHBoxLayout();
    pButtonLayout->addStretch();
    pButtonLayout->addWidget(m_pAddButton);
    pButtonLayout->addWidget(m_pEditButton);
    pButtonLayout->addWidget(m_pDeleteButton);
    pButtonLayout->addWidget(m_pSaveButton);
    m_pMainLayout->addLayout(pButtonLayout);
}

void TagManagerDialog::connectSignals()
{
    this->connect(m_pAddButton, &QPushButton::clicked, this, &TagManagerDialog::onAddButtonClicked);
    this->connect(m_pEditButton, &QPushButton::clicked, this, &TagManagerDialog::onEditButtonClicked);
    // 双击列表项的事件也连接到编辑功能
    this->connect(m_pListWidget, &QListWidget::itemDoubleClicked, this, &TagManagerDialog::onEditButtonClicked);
    this->connect(m_pDeleteButton, &QPushButton::clicked, this, &TagManagerDialog::onDeleteButtonClicked);
    this->connect(m_pSaveButton, &QPushButton::clicked, this, &TagManagerDialog::accept);
}

void TagManagerDialog::populateList()
{
    m_pListWidget->clear(); // 刷新前总是先清空列表

    for (int i = 0; i < m_tags.size(); ++i)
    {
        const ModbusTag& tag = m_tags.at(i);

        // 1. 创建要在列表项中显示的摘要文本
        QString summary = QString("%1 (ID:%2, Addr:%3, Type:%4)")
                          .arg(tag.name)
                          .arg(tag.slaveId)
                          .arg(tag.address)
                          .arg(dataTypeToString(tag.dataType)); // 使用一个辅助函数将枚举转为字符串

        // 2. 创建列表项
        QListWidgetItem* item = new QListWidgetItem(summary);

        // 3. 将这个点位在 m_tags 列表中的【索引】作为隐藏数据存入列表项
        //    这对于后续的“编辑”和“删除”操作至关重要
        item->setData(Qt::UserRole, i);

        m_pListWidget->addItem(item);
    }
}

QString TagManagerDialog::dataTypeToString(ModbusTag::DataType type)
{
    switch (type)
    {
    case ModbusTag::DataType::Int16: return "Int16";
    case ModbusTag::DataType::UInt16: return "UInt16";
    case ModbusTag::DataType::Float32: return "Float";
    case ModbusTag::DataType::Int32: return "Int32";
    case ModbusTag::DataType::UInt32: return "UInt32";
    case ModbusTag::DataType::Double64: return "Double";
    default: return "Unknown";
    }
}
