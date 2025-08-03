/**
  ******************************************************************************
  * @file           : WaveformCtrlWidget.cpp
  * @author         : wangxiangyu
  * @brief          : None
  * @attention      : None
  * @date           : 2025/7/31
  ******************************************************************************
  */
#include <ui/WaveformCtrlWidget.h>

WaveformCtrlWidget::WaveformCtrlWidget(QWidget* parent)
    : QWidget(parent)
      , m_pTestDataTimer(nullptr)
      , m_testDataTimestamp(0)
      , m_isGeneratingTestData(false)
{
    this->setUI();
    StyleLoader::loadStyleFromFile(this, ":resources/qss/wave_form_ctrl_widget.qss");
}

void WaveformCtrlWidget::setUI()
{
    this->setAttribute(Qt::WA_StyledBackground);
    this->createComponents();
    this->createLayout();
    this->connectSignals();
}

void WaveformCtrlWidget::createComponents()
{
    m_pAddChannelButton = new QPushButton(this);
    m_pAddChannelButton->setObjectName("m_pAddChannelButton");
    m_pRemoveChannelButton = new QPushButton(this);
    m_pRemoveChannelButton->setObjectName("m_pRemoveChannelButton");
    m_pClearButton = new QPushButton(this);
    m_pClearButton->setObjectName("m_pClearButton");
    m_pImportButton = new QPushButton(this);
    m_pImportButton->setObjectName("m_pImportButton");
    m_pExportButton = new QPushButton(this);
    m_pExportButton->setObjectName("m_pExportButton");
    m_pActionButton = new QPushButton(this);
    m_pActionButton->setObjectName("m_pActionButton");
    m_pActionButton->setProperty("actionClicked", false);

    m_pAddChannelButton->setToolTip("添加新的数据通道");
    m_pRemoveChannelButton->setToolTip("移除通道");
    m_pClearButton->setToolTip("清除所有数据");
    m_pImportButton->setToolTip("从文件导入数据");
    m_pExportButton->setToolTip("导出数据到文件");
    m_pActionButton->setToolTip("开始数据采集");
}

void WaveformCtrlWidget::createLayout()
{
    m_pMainLayout = new QHBoxLayout(this);
    m_pMainLayout->addStretch();
    m_pMainLayout->addWidget(m_pAddChannelButton);
    m_pMainLayout->addWidget(m_pRemoveChannelButton);
    m_pMainLayout->addWidget(m_pClearButton);
    m_pMainLayout->addWidget(m_pImportButton);
    m_pMainLayout->addWidget(m_pExportButton);
    m_pMainLayout->addWidget(m_pActionButton);
}

void WaveformCtrlWidget::connectSignals()
{
    this->connect(m_pAddChannelButton, &QPushButton::clicked, this, &WaveformCtrlWidget::onAddChannelBtnClicked);
    this->connect(m_pActionButton, &QPushButton::clicked, this, &WaveformCtrlWidget::onActionBtnClicked);

    // 初始化测试数据定时器
    m_pTestDataTimer = new QTimer(this);
    connect(m_pTestDataTimer, &QTimer::timeout, this, &WaveformCtrlWidget::generateTestData);
}

void WaveformCtrlWidget::onAddChannelBtnClicked()
{
    m_pAddChannelDialog = new AddChannelDialog(this);
    // 从单例管理器获取已有通道
    ChannelManager* manager = ChannelManager::getInstance();
    m_pAddChannelDialog->setExistingChannels(manager->getAllChannels());

    if (m_pAddChannelDialog->exec() == QDialog::Accepted)
    {
        QString name = m_pAddChannelDialog->getChannelName();
        QString id = m_pAddChannelDialog->getChannelId();
        QString color = m_pAddChannelDialog->getChannelColor();
        // 添加到管理器
        if (manager->addChannel(id, name, color))
        {
            QString message = tr("通道%1添加成功").arg(name);
            CMessageBox::showToast(this, message);
        }
        else
        {
            CMessageBox::showToast(this, tr("标识%1已存在").arg(id));
            m_pAddChannelDialog->deleteLater();
            return;
        }
    }
    m_pAddChannelDialog->deleteLater();
}

void WaveformCtrlWidget::onActionBtnClicked()
{
    bool actionClicked = m_pActionButton->property("actionClicked").toBool();
    ChannelManager* manager = ChannelManager::getInstance();

    if (!actionClicked)
    {
        // 开始生成测试数据
        m_isGeneratingTestData = true;
        m_testDataTimestamp = 0; // 重置时间戳
        m_pTestDataTimer->start(50); // 每50ms生成一次数据

        // 启动数据分发
        manager->startDataDispatch();

        m_pActionButton->setToolTip("结束数据采集");
        qDebug() << "开始生成测试数据";
    }
    else
    {
        // 停止生成测试数据
        m_isGeneratingTestData = false;
        m_pTestDataTimer->stop();

        // 停止数据分发
        manager->stopDataDispatch();

        m_pActionButton->setToolTip("开始数据采集");
        qDebug() << "停止生成测试数据";
    }

    m_pActionButton->setProperty("actionClicked", !actionClicked);
    m_pActionButton->style()->unpolish(m_pActionButton);
    m_pActionButton->style()->polish(m_pActionButton);
    m_pActionButton->update();
}

void WaveformCtrlWidget::generateTestData()
{
    // 双重检查确保在停止状态下不生成数据
    if (!m_isGeneratingTestData)
    {
        if (m_pTestDataTimer && m_pTestDataTimer->isActive())
        {
            m_pTestDataTimer->stop();
        }
        return;
    }

    ChannelManager* manager = ChannelManager::getInstance();
    if (!manager) return;

    auto channels = manager->getAllChannels();

    if (channels.isEmpty()) return;

    // 为每个通道生成测试数据
    for (const auto& channel : channels)
    {
        // 生成测试数据 - 这里使用简单的正弦波形作为示例
        double value = 50 * sin(0.01 * m_testDataTimestamp) +
            30 * cos(0.02 * m_testDataTimestamp) +
            10 * sin(0.03 * m_testDataTimestamp);

        // 添加一些随机噪声 (使用 QRandomGenerator 替代 qrand)
        int noise = m_randomGenerator.bounded(10) - 5; // 生成 -5 到 4 之间的随机数
        value += noise;

        // 创建数据点 [时间戳, 值]
        QVariantList dataPoint;
        dataPoint << (double)m_testDataTimestamp << value;

        // 添加到通道数据中
        manager->addChannelData(channel.id, QVariant(std::move(dataPoint)));
    }

    // 更新时间戳
    m_testDataTimestamp += 50; // 与定时器间隔保持一致

    // 可选：限制时间范围，避免数值过大
    if (m_testDataTimestamp > 1000000)
    {
        // 1000秒后重置
        m_testDataTimestamp = 0;
    }
}
