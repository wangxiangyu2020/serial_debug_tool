/**
  ******************************************************************************
  * @file           : TitleBar.cpp
  * @author         : wangxiangyu
  * @brief          : None
  * @attention      : None
  * @date           : 2025/7/16
  ******************************************************************************
  */

#pragma comment(lib, "user32.lib")
#include <qt_windows.h>
#include "ui/TitleBar.h"
#include <QHBoxLayout>
#include <QMouseEvent>

#include "utils/StyleLoader.h"

TitleBar::TitleBar(QWidget* parent)
    : QWidget(parent)
{
    this->setUI();
    StyleLoader::loadStyleFromFile(this, ":/resources/qss/title_bar_style.qss");
}

void TitleBar::setUI()
{
    this->setAttribute(Qt::WA_StyledBackground);
    this->setFixedHeight(32);
    this->setObjectName("titleBar");

    m_pLogoBtn = new QPushButton(this);
    m_pLogoBtn->setObjectName("logoBtn");
    m_pLogoBtn->setText("串口调试助手");
    m_pLogoBtn->setFixedSize(160, 32);

    m_pMinBtn = new QPushButton(this);
    m_pMinBtn->setObjectName("minBtn");
    m_pMinBtn->setText("-");
    m_pMinBtn->setFixedSize(32, 20);

    m_pMaxBtn = new QPushButton(this);
    m_pMaxBtn->setObjectName("maxBtn");
    m_pMaxBtn->setText("□"); // 最大化按钮
    m_pMaxBtn->setFixedSize(32, 20);

    m_pCloseBtn = new QPushButton(this);
    m_pCloseBtn->setObjectName("closeBtn");
    m_pCloseBtn->setText("×"); // 关闭按钮
    m_pCloseBtn->setFixedSize(32, 20);

    // 加载单个QSS文件
    QFile styleFile(":/resources/qss/title_bar_style.qss");
    styleFile.open(QFile::ReadOnly);
    QString styleSheet = QLatin1String(styleFile.readAll());
    this->setStyleSheet(styleSheet);

    QHBoxLayout* pHLayout = new QHBoxLayout(this);
    pHLayout->addSpacing(5);
    pHLayout->addWidget(m_pLogoBtn);
    pHLayout->addStretch();
    pHLayout->addWidget(m_pMinBtn);
    pHLayout->addWidget(m_pMaxBtn);
    pHLayout->addWidget(m_pCloseBtn);
    pHLayout->setContentsMargins(0, 0, 0, 2);

    this->connect(m_pMinBtn, &QPushButton::clicked, this, &TitleBar::onClicked);
    this->connect(m_pMaxBtn, &QPushButton::clicked, this, &TitleBar::onClicked);
    this->connect(m_pCloseBtn, &QPushButton::clicked, this, &TitleBar::onClicked);
}

void TitleBar::mousePressEvent(QMouseEvent* event)
{
    if (ReleaseCapture())
    {
        QWidget* pWindow = this->window();
        if (pWindow->isWindow())
        {
            SendMessage(HWND(pWindow->winId()), WM_SYSCOMMAND, SC_MOVE + HTCAPTION, 0);
        }
    }
}

void TitleBar::mouseDoubleClickEvent(QMouseEvent* event)
{
    emit m_pMaxBtn->clicked();
}

void TitleBar::onClicked()
{
    QPushButton* pBtn = qobject_cast<QPushButton*>(sender());
    QWidget* pWindow = this->window();

    if (pBtn == m_pMinBtn)
    {
        pWindow->showMinimized();
    }
    else if (pBtn == m_pMaxBtn)
    {
        if (pWindow->isMaximized())
        {
            pWindow->showNormal();
            m_pMaxBtn->setText("□");
        }
        else
        {
            pWindow->showMaximized();
            m_pMaxBtn->setText("❐");
        }
    }
    else if (pBtn == m_pCloseBtn)
    {
        emit sigClosed();
    }
}
