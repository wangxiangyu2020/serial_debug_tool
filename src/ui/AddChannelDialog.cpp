/**
  ******************************************************************************
  * @file           : AddChannelDialog.cpp
  * @author         : wangxiangyu
  * @brief          : None
  * @attention      : None
  * @date           : 2025/7/31
  ******************************************************************************
  */

#include "ui/AddChannelDialog.h"
#include "utils/StyleLoader.h"

AddChannelDialog::AddChannelDialog(QWidget* parent)
    : CDialogBase(parent, "添加通道", QSize(450, 350))
{
    this->setUI();
    // 加载专用样式
    StyleLoader::loadStyleFromFile(this, ":/resources/qss/add_channel_dialog.qss");
}

QString AddChannelDialog::getChannelName() const
{
    return m_pNameEdit->text();
}

QString AddChannelDialog::getChannelColor() const
{
    return m_pColorCombo->currentData().toString();
}

void AddChannelDialog::setExistingChannels(const QList<ChannelInfo>& channels)
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

        layout->addWidget(colorIndicator);
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

void AddChannelDialog::createComponents()
{
    // 输入区域组件
    m_pNameLabel = new QLabel("名称", this);
    m_pColorLabel = new QLabel("颜色", this);
    m_pNameEdit = new QLineEdit(this);
    m_pNameEdit->setPlaceholderText("请输入通道名称");
    m_pColorCombo = new QComboBox(this);
    // 添加颜色选项，使用颜色指示器
    QStringList colors = {"红色", "蓝色", "绿色", "橙色", "紫色", "青色", "黄色", "粉色"};
    for (const QString& colorName : colors)
    {
        // 创建更大更宽的颜色指示器图标
        QPixmap pixmap(48, 24);
        pixmap.fill(QColor(StyleLoader::getColorHex(colorName)));
        // 添加带图标的选项
        m_pColorCombo->addItem(QIcon(pixmap), "", colorName);
    }
    // 设置下拉框显示模式 - 进一步增大图标尺寸
    m_pColorCombo->setIconSize(QSize(40, 20));
    // 已添加通道列表 - 只显示一行高度
    m_pExistingLabel = new QLabel("已有通道", this);
    m_pChannelListWidget = new QListWidget(this);
    // 设置固定高度，只显示一行
    m_pChannelListWidget->setFixedHeight(40);
    m_pChannelListWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    m_pChannelListWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_pChannelListWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    // 设置按钮对象名称用于样式
    if (m_pCancelButton) m_pCancelButton->setObjectName("cancelButton");
}

void AddChannelDialog::createContentLayout()
{
    // 确保 m_pContentLayout 已经在基类中初始化
    if (!m_pContentLayout) return;
    // 设置更紧凑的间距
    m_pContentLayout->setSpacing(4);
    m_pContentLayout->addWidget(m_pNameLabel);
    m_pContentLayout->addWidget(m_pNameEdit);
    m_pContentLayout->addWidget(m_pColorLabel);
    m_pContentLayout->addWidget(m_pColorCombo);
    // 添加一点间距分隔输入区域和列表区域
    m_pContentLayout->addSpacing(8);
    // 已添加通道列表
    m_pContentLayout->addWidget(m_pExistingLabel);
    m_pContentLayout->addWidget(m_pChannelListWidget);
}

void AddChannelDialog::connectSignals()
{
}

void AddChannelDialog::onConfirmClicked()
{
    if (m_pNameEdit->text().isEmpty())
    {
        // 可以添加提示：请输入通道名称
        CMessageBox::showToast(this, tr("请输入通道名称"));
        return;
    }
    // 检查通道名称是否重复
    QString newName = m_pNameEdit->text().trimmed();
    for (int i = 0; i < m_pChannelListWidget->count(); ++i)
    {
        QListWidgetItem* item = m_pChannelListWidget->item(i);
        QWidget* widget = m_pChannelListWidget->itemWidget(item);
        if (widget)
        {
            QLabel* nameLabel = widget->findChild<QLabel*>();
            if (nameLabel && nameLabel->text() == newName)
            {
                // 可以添加提示：通道名称已存在
                CMessageBox::showToast(this, "通道名称已存在");
                return;
            }
        }
    }
    CDialogBase::onConfirmClicked();
}

void AddChannelDialog::setUI()
{
    // 手动调用初始化方法
    this->createComponents();
    this->createContentLayout();
    this->connectSignals();
}
