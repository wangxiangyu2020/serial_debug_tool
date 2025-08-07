/**
  ******************************************************************************
  * @file           : SettingsTab.cpp
  * @author         : wangxiangyu
  * @brief          : None
  * @attention      : None
  * @date           : 2025/7/17
  ******************************************************************************
  */

#include "ui/SettingsTab.h"

SettingsTab::SettingsTab(QWidget* parent)
    : QWidget(parent)
{
    this->setUI();
    StyleLoader::loadStyleFromFile(this, ":/resources/qss/settings_tab.qss");
}

void SettingsTab::setUI()
{
    this->setAttribute(Qt::WA_StyledBackground);
    this->createComponents();
}

void SettingsTab::createComponents()
{
    m_pTabWidget = new QTabWidget(this);
    m_pTabWidget->setTabPosition(QTabWidget::West);
    // 创建README文档查看器
    m_pReadmeViewer = new QTextEdit(this);
    m_pReadmeViewer->setReadOnly(true);
    // 加载README.md内容
    QFile readmeFile(":/README.md");
    if (readmeFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QString content = QString::fromUtf8(readmeFile.readAll());
        m_pReadmeViewer->setPlainText(content);
    }

    m_pTabWidget->addTab(m_pReadmeViewer, "README");
    // 创建布局
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(m_pTabWidget);
    layout->setContentsMargins(2, 2, 2, 2);
}
