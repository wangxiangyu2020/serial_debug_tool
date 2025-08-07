/**
  ******************************************************************************
  * @file           : RemoveChannelDialog.cpp
  * @author         : wangxiangyu
  * @brief          : None
  * @attention      : None
  * @date           : 2025/8/7
  ******************************************************************************
  */
#include "ui/RemoveChannelDialog.h"

RemoveChannelDialog::RemoveChannelDialog(QWidget* parent)
    : CDialogBase(parent, "移除通道", QSize(450, 300))
{
    this->setUI();
    // 加载专用样式
    StyleLoader::loadStyleFromFile(this, ":/resources/qss/remove_channel_dialog.qss");
}

void RemoveChannelDialog::setExistingChannels(const QList<ChannelInfo>& channels)
{
    m_pChannelListWidget->clear();

    for (const ChannelInfo& channel : channels)
    {
        QListWidgetItem* item = new QListWidgetItem(m_pChannelListWidget);

        // 创建自定义widget显示通道信息
        QWidget* itemWidget = new QWidget();
        itemWidget->setObjectName("channelItem");
        QHBoxLayout* layout = new QHBoxLayout(itemWidget);
        layout->setContentsMargins(8, 6, 8, 6);
        layout->setSpacing(10);

        // 颜色指示器 - 调整为更宽的矩形条
        QLabel* colorIndicator = new QLabel();
        colorIndicator->setObjectName("colorIndicator");
        colorIndicator->setFixedSize(40, 20); // 宽40px，高20px的矩形条
        colorIndicator->setStyleSheet(
            QString("background-color: %1; border-radius: 2px;").arg(StyleLoader::getColorHex(channel.color)));

        // 通道名称
        QLabel* nameLabel = new QLabel(channel.name);
        nameLabel->setObjectName("channelName");
        nameLabel->setWordWrap(false);
        nameLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

        // 通道标识
        QLabel* idLabel = new QLabel(channel.id);
        idLabel->setObjectName("channelName");

        layout->addWidget(colorIndicator);
        layout->addWidget(idLabel);
        layout->addWidget(nameLabel);
        layout->addStretch();

        // 设置每个项目的固定高度
        item->setSizeHint(QSize(0, 32));
        m_pChannelListWidget->setItemWidget(item, itemWidget);
    }

    // 如果没有通道，显示提示
    if (channels.isEmpty())
    {
        QListWidgetItem* item = new QListWidgetItem("暂无已添加的通道");
        item->setTextAlignment(Qt::AlignCenter);
        item->setFlags(Qt::NoItemFlags);
        QFont font = item->font();
        font.setItalic(true);
        item->setFont(font);
        item->setForeground(QColor("#6c757d"));
        m_pChannelListWidget->addItem(item);
    }
}

void RemoveChannelDialog::createComponents()
{
    // 已添加通道列表
    m_pChannelListWidget = new QListWidget(this);
    // 设置固定高度，只显示5行
    m_pChannelListWidget->setFixedHeight(160);
    m_pChannelListWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    m_pChannelListWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_pChannelListWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    // 设置按钮对象名称用于样式
    if (m_pCancelButton) m_pCancelButton->setObjectName("cancelButton");
    if (m_pConfirmButton)
    {
        m_pConfirmButton->setObjectName("removeButton");
        m_pConfirmButton->setText("移除");
    }
}

void RemoveChannelDialog::createContentLayout()
{
    if (!m_pContentLayout) return;
    m_pContentLayout->setSpacing(4);
    m_pContentLayout->addWidget(m_pChannelListWidget);
}

void RemoveChannelDialog::connectSignals()
{
}

void RemoveChannelDialog::onConfirmClicked()
{
    int selectedRow = m_pChannelListWidget->currentRow();
    if (selectedRow < 0)
    {
        CMessageBox::showToast(this, tr("请选择要移除的通道"));
        return;
    }
    // 获取选中的行
    QListWidgetItem* item = m_pChannelListWidget->item(selectedRow);
    // 获取行对应的widget
    QWidget* itemWidget = m_pChannelListWidget->itemWidget(item);
    // 通过findChild方法获取widget中的各个控件
    QList<QLabel*> labels = itemWidget->findChildren<QLabel*>("channelName");
    QLabel* idLabel = labels.size() > 0 ? labels[0] : nullptr;
    if (idLabel == nullptr) return;
    QString channelId = idLabel->text();
    if (channelId.isEmpty()) return;
    ChannelManager* channelManager = ChannelManager::getInstance();
    channelManager->removeChannel(channelId);
    CDialogBase::onConfirmClicked();
}

void RemoveChannelDialog::setUI()
{
    this->createComponents();
    this->createContentLayout();
    this->connectSignals();
}
