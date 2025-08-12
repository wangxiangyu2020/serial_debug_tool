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

// 构造函数和析构函数
MainWindow::MainWindow(QWidget* parent)
    : FramelessBase(parent)
{
    this->setUI();
    StyleLoader::loadStyleFromFile(this, ":/resources/qss/main_window.qss");
}

// 私有方法
void MainWindow::setUI()
{
    this->setAttribute(Qt::WA_TranslucentBackground);
    m_pTitleBar = new TitleBar(this);
    m_pTabContainer = new CTabWidget(this);

    QVBoxLayout* pMainLayout = new QVBoxLayout(this);
    pMainLayout->addWidget(m_pTitleBar);
    pMainLayout->setSpacing(0);
    pMainLayout->addWidget(m_pTabContainer);

    pMainLayout->setContentsMargins(0, 0, 0, 0);

    this->connect(m_pTitleBar, &TitleBar::sigClosed, [=]
    {
        this->close();
    });
}
