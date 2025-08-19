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

// 构造函数和析构函数
WaveformCtrlWidget::WaveformCtrlWidget(QWidget* parent)
    : QWidget(parent)
{
    this->setUI();
    StyleLoader::loadStyleFromFile(this, ":/resources/qss/wave_form_ctrl_widget.qss");
}

// private slots
void WaveformCtrlWidget::onAddChannelBtnClicked()
{
    this->requestChannels(ChannelOperation::AddChannel);
}

void WaveformCtrlWidget::onRemoveChannelBtnClicked()
{
    this->requestChannels(ChannelOperation::RemoveChannel);
}

void WaveformCtrlWidget::onChannelsReceived(const QList<ChannelInfo>& channels)
{
    switch (m_pendingOperation)
    {
    case ChannelOperation::AddChannel:
        this->processChannelsForAdd(channels);
        break;
    case ChannelOperation::RemoveChannel:
        this->processChannelsForRemove(channels);
        break;
    }
}

void WaveformCtrlWidget::onClearBtnClicked()
{
    emit clearAllChannelDataRequested();
}

void WaveformCtrlWidget::onImportBtnClicked()
{
    emit importChannelsDataRequested();
}

void WaveformCtrlWidget::onExportBtnClicked()
{
    emit exportChannelsDataRequested();
}

void WaveformCtrlWidget::onSampleRateBtnClicked()
{
    emit requestSampleRate();
}

void WaveformCtrlWidget::onSampleRateReceived(int rate)
{
    m_pSampleRateDialog = new SampleRateDialog(this);
    m_pSampleRateDialog->setSampleRate(rate);
    if (m_pSampleRateDialog->exec() == QDialog::Accepted)
    {
        emit sampleRateChanged(m_pSampleRateDialog->getSampleRate());
    }
    QTimer::singleShot(0, m_pSampleRateDialog, &SampleRateDialog::deleteLater);
}

void WaveformCtrlWidget::onActionBtnClicked()
{
    if (m_actionClicked)
    {
        emit stopActionRequested();
        this->setBtnStatus(m_actionClicked);
        m_pActionButton->setToolTip("开始数据采集");
        return;
    }
    emit startActionRequested();
    this->setBtnStatus(!m_actionClicked);
    m_pActionButton->setToolTip("结束数据采集");
}

void WaveformCtrlWidget::onStatusChanged(const QString& status)
{
    CMessageBox::showToast(this, status);
    if (!status.contains("数据分发已启动") && !status.contains("数据分发已停止")) return;
    m_actionClicked = status.contains("数据分发已启动");
    m_pActionButton->setProperty("actionClicked", m_actionClicked);
    m_pActionButton->style()->unpolish(m_pActionButton);
    m_pActionButton->style()->polish(m_pActionButton);
    m_pActionButton->update();
}

// 私有方法
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

    this->connect(this, &WaveformCtrlWidget::requestAllChannels, ChannelManager::getInstance(),
                  &ChannelManager::onGetAllChannels);
    this->connect(ChannelManager::getInstance(), &ChannelManager::sendAllChannelsRequested, this,
                  &WaveformCtrlWidget::onChannelsReceived);
    this->connect(this, &WaveformCtrlWidget::addChannelRequested, ChannelManager::getInstance(),
                  &ChannelManager::onAddChannel);
    this->connect(ChannelManager::getInstance(), &ChannelManager::statusChanged, this,
                  &WaveformCtrlWidget::onStatusChanged);
    this->connect(this, &WaveformCtrlWidget::clearAllChannelDataRequested, ChannelManager::getInstance(),
                  &ChannelManager::onClearAllChannelData);
    this->connect(this, &WaveformCtrlWidget::importChannelsDataRequested, ChannelManager::getInstance(),
                  &ChannelManager::onImportChannelsData);
    this->connect(this, &WaveformCtrlWidget::exportChannelsDataRequested, ChannelManager::getInstance(),
                  &ChannelManager::onExportChannelsData);
    this->connect(this, &WaveformCtrlWidget::requestSampleRate, ChannelManager::getInstance(),
                  &ChannelManager::onGetSampleRate);
    this->connect(ChannelManager::getInstance(), &ChannelManager::sendSampleRateRequested, this,
                  &WaveformCtrlWidget::onSampleRateReceived);
    this->connect(this, &WaveformCtrlWidget::sampleRateChanged, ChannelManager::getInstance(),
                  &ChannelManager::onSetSampleRate);
    this->connect(this, &WaveformCtrlWidget::startActionRequested, ChannelManager::getInstance(),
                  &ChannelManager::onStartDataDispatch);
    this->connect(this, &WaveformCtrlWidget::stopActionRequested, ChannelManager::getInstance(),
                  &ChannelManager::onStopDataDispatch);
}

void WaveformCtrlWidget::setBtnStatus(bool actionClicked)
{
    m_pAddChannelButton->setEnabled(actionClicked);
    m_pRemoveChannelButton->setEnabled(actionClicked);
    m_pClearButton->setEnabled(actionClicked);
    m_pExportButton->setEnabled(actionClicked);
    m_pImportButton->setEnabled(actionClicked);
    m_pSampleRateButton->setEnabled(actionClicked);
}

void WaveformCtrlWidget::requestChannels(ChannelOperation operation)
{
    m_pendingOperation = operation;
    emit requestAllChannels();
}

void WaveformCtrlWidget::processChannelsForAdd(const QList<ChannelInfo>& channels)
{
    m_pAddChannelDialog = new AddChannelDialog(this);
    m_pAddChannelDialog->setExistingChannels(channels);

    if (m_pAddChannelDialog->exec() == QDialog::Accepted)
    {
        QString name = m_pAddChannelDialog->getChannelName();
        QString id = m_pAddChannelDialog->getChannelId();
        QString color = m_pAddChannelDialog->getChannelColor();
        emit addChannelRequested(id, name, color);
    }
    m_pAddChannelDialog->deleteLater();
}

void WaveformCtrlWidget::processChannelsForRemove(const QList<ChannelInfo>& channels)
{
    m_pRemoveChannelDialog = new RemoveChannelDialog(this);
    m_pRemoveChannelDialog->setExistingChannels(channels);
    if (m_pRemoveChannelDialog->exec() == QDialog::Accepted);
    m_pRemoveChannelDialog->deleteLater();
}
