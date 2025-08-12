/**
  ******************************************************************************
  * @file           : WaveformTab.cpp
  * @author         : wangxiangyu
  * @brief          : None
  * @attention      : None
  * @date           : 2025/7/17
  ******************************************************************************
  */

#include "ui/WaveformTab.h"

// 构造函数和析构函数
WaveformTab::WaveformTab(QWidget* parent)
    : QWidget(parent)
{
    this->setUI();
    StyleLoader::loadStyleFromFile(this, ":resources/qss/wave_form_tab.qss");
}

// 私有方法
void WaveformTab::setUI()
{
    this->setAttribute(Qt::WA_StyledBackground);
    m_pPanel = new QWidget(this);
    m_pWaveformWidget = new WaveformWidget(m_pPanel);
    m_pWaveformCtrlWidget = new WaveformCtrlWidget(m_pPanel);
    m_pMainLayout = new QVBoxLayout(this);
    m_pMainLayout->addWidget(m_pWaveformCtrlWidget, 0);
    m_pMainLayout->setSpacing(1);
    m_pMainLayout->addWidget(m_pWaveformWidget, 10);
    m_pMainLayout->setContentsMargins(2, 2, 2, 2);
}
