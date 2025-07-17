/**
  ******************************************************************************
  * @file           : MainWindow.cpp
  * @author         : wangxiangyu
  * @brief          : None
  * @attention      : None
  * @date           : 2025/7/16
  ******************************************************************************
  */
#include "ui/MainWindow.h"
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget* parent)
    : FramelessBase(parent)
{
    this->setUI();
}


void MainWindow::setUI()
{
    m_pTitleBar = new TitleBar(this);
    m_pTabContainer = new CTabWidget(this);

    QVBoxLayout* pMainLayout = new QVBoxLayout(this);
    pMainLayout->addWidget(m_pTitleBar);
    pMainLayout->setSpacing(5);
    pMainLayout->addWidget(m_pTabContainer);

    pMainLayout->setContentsMargins(0, 0, 0, 0);

    this->connect(m_pTitleBar, &TitleBar::sigClosed, [=]
    {
        this->close();
    });
}
