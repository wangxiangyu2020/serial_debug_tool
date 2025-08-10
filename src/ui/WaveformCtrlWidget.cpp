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
{
    this->setUI();
    StyleLoader::loadStyleFromFile(this, ":/resources/qss/wave_form_ctrl_widget.qss");
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
    m_pSampleRateButton = new QPushButton(this);
    m_pSampleRateButton->setObjectName("m_pSampleRateButton");
    m_pActionButton = new QPushButton(this);
    m_pActionButton->setObjectName("m_pActionButton");
    m_pActionButton->setProperty("actionClicked", false);

    m_pAddChannelButton->setToolTip("添加新的数据通道");
    m_pRemoveChannelButton->setToolTip("移除通道");
    m_pClearButton->setToolTip("清除所有数据");
    m_pImportButton->setToolTip("从文件导入数据");
    m_pExportButton->setToolTip("导出数据到文件");
    m_pSampleRateButton->setToolTip("设置采样间隔");
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
    m_pMainLayout->addWidget(m_pSampleRateButton);
    m_pMainLayout->addWidget(m_pActionButton);
}

void WaveformCtrlWidget::connectSignals()
{
    this->connect(m_pAddChannelButton, &QPushButton::clicked, this, &WaveformCtrlWidget::onAddChannelBtnClicked);
    this->connect(m_pRemoveChannelButton, &QPushButton::clicked, this, &WaveformCtrlWidget::onRemoveChannelBtnClicked);
    this->connect(m_pClearButton, &QPushButton::clicked, this, &WaveformCtrlWidget::onClearBtnClicked);
    this->connect(m_pImportButton, &QPushButton::clicked, this, &WaveformCtrlWidget::onImportBtnClicked);
    this->connect(m_pExportButton, &QPushButton::clicked, this, &WaveformCtrlWidget::onExportBtnClicked);
    this->connect(m_pSampleRateButton, &QPushButton::clicked, this, &WaveformCtrlWidget::onSampleRateBtnClicked);
    this->connect(m_pActionButton, &QPushButton::clicked, this, &WaveformCtrlWidget::onActionBtnClicked);
}

void WaveformCtrlWidget::setBtnStatus(bool actionClicked)
{
    m_pAddChannelButton->setEnabled(actionClicked);
    m_pRemoveChannelButton->setEnabled(actionClicked);
    m_pClearButton->setEnabled(actionClicked);
    m_pExportButton->setEnabled(actionClicked);
    m_pImportButton->setEnabled(actionClicked);
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

void WaveformCtrlWidget::onRemoveChannelBtnClicked()
{
    m_pRemoveChannelDialog = new RemoveChannelDialog(this);
    // 从单例管理器获取已有通道
    ChannelManager* manager = ChannelManager::getInstance();
    m_pRemoveChannelDialog->setExistingChannels(manager->getAllChannels());
    m_pRemoveChannelDialog->exec();
    m_pRemoveChannelDialog->deleteLater();
}

void WaveformCtrlWidget::onClearBtnClicked()
{
    ChannelManager* manager = ChannelManager::getInstance();
    manager->clearAllChannelData();
}

void WaveformCtrlWidget::onImportBtnClicked()
{
    ChannelManager* manager = ChannelManager::getInstance();
    emit manager->importChannelsData();
}

void WaveformCtrlWidget::onExportBtnClicked()
{
    ChannelManager* manager = ChannelManager::getInstance();
    emit manager->channelsExportData();
}

void WaveformCtrlWidget::onSampleRateBtnClicked()
{
    m_pSampleRateDialog = new SampleRateDialog(this);
    m_pSampleRateDialog->setSampleRate(ChannelManager::getInstance()->getSampleRate());
    if (m_pSampleRateDialog->exec() == QDialog::Accepted)
    {
        ChannelManager::getInstance()->setSampleRate(m_pSampleRateDialog->getSampleRate());
        QString message = tr("采样间隔已设置为%1ms").arg(m_pSampleRateDialog->getSampleRate());
        CMessageBox::showToast(this, message);
    }
    QTimer::singleShot(0, m_pSampleRateDialog, &SampleRateDialog::deleteLater);
}

void WaveformCtrlWidget::onActionBtnClicked()
{
    bool actionClicked = m_pActionButton->property("actionClicked").toBool();
    ChannelManager* manager = ChannelManager::getInstance();

    if (!actionClicked)
    {
        this->setBtnStatus(actionClicked);
        // 启动数据分发
        emit manager->channelDataProcess(!actionClicked);
        manager->startDataDispatch();
        m_pActionButton->setToolTip("结束数据采集");
    }
    else
    {
        this->setBtnStatus(actionClicked);
        emit manager->channelDataProcess(actionClicked);
        // 停止数据分发
        manager->stopDataDispatch();
        m_pActionButton->setToolTip("开始数据采集");
    }

    m_pActionButton->setProperty("actionClicked", !actionClicked);
    m_pActionButton->style()->unpolish(m_pActionButton);
    m_pActionButton->style()->polish(m_pActionButton);
    m_pActionButton->update();
}
