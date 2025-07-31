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
}
