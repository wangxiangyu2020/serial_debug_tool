/**
  ******************************************************************************
  * @file           : TitleBar.cpp
  * @author         : wangxiangyu
  * @brief          : None
  * @attention      : None
  * @date           : 2025/7/16
  ******************************************************************************
  */

#include "ui/TitleBar.h"

TitleBar::TitleBar(QWidget* parent)
    : QWidget(parent)
{
    this->setUI();
    StyleLoader::loadStyleFromFile(this, ":/resources/qss/title_bar.qss");
}

void TitleBar::setUI()
{
    this->setAttribute(Qt::WA_StyledBackground);
    this->setFixedHeight(32);
    this->setObjectName("titleBar");

    m_pLogoBtn = new QPushButton(this);
    m_pLogoBtn->setObjectName("logoBtn");
    m_pLogoBtn->setText("iKUN调试助手");
    m_pLogoBtn->setFixedSize(160, 32);

    m_pMinBtn = new QPushButton(this);
    m_pMinBtn->setObjectName("minBtn");
    m_pMinBtn->setText("-");
    m_pMinBtn->setFixedSize(32, 32);

    m_pMaxBtn = new QPushButton(this);
    m_pMaxBtn->setObjectName("maxBtn");
    m_pMaxBtn->setText("□"); // 最大化按钮
    m_pMaxBtn->setFixedSize(32, 32);

    m_pCloseBtn = new QPushButton(this);
    m_pCloseBtn->setObjectName("closeBtn");
    m_pCloseBtn->setText("×"); // 关闭按钮
    m_pCloseBtn->setFixedSize(32, 32);

    // 加载单个QSS文件
    QFile styleFile(":/resources/qss/title_bar.qss");
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
    if (event->button() == Qt::LeftButton)
    {
        m_dragging = true;
        m_dragStartPosition = event->globalPosition().toPoint(); // Qt 5.15+ 使用 globalPosition()
    }
    QWidget::mousePressEvent(event);
}

void TitleBar::mouseMoveEvent(QMouseEvent* event)
{
    if (m_dragging)
    {
        // 检测是否达到拖动阈值（避免微移误触发）
        const int moveThreshold = QApplication::startDragDistance();
        QPoint delta = event->globalPosition().toPoint() - m_dragStartPosition;

        if (delta.manhattanLength() >= moveThreshold)
        {
            m_dragging = false; // 防止重复触发

            QWidget* pWindow = this->window();
            if (pWindow->isWindow())
            {
                // 触发窗口拖动
                ReleaseCapture();
                SendMessage(
                    reinterpret_cast<HWND>(pWindow->winId()),
                    WM_SYSCOMMAND,
                    SC_MOVE | HTCAPTION,
                    0
                );
            }
        }
    }
    QWidget::mouseMoveEvent(event);
}

void TitleBar::mouseReleaseEvent(QMouseEvent* event)
{
    m_dragging = false;
    QWidget::mouseReleaseEvent(event);
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
