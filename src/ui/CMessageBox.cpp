/**
  ******************************************************************************
  * @file           : CMessageBox.cpp
  * @author         : wangxiangyu
  * @brief          : None
  * @attention      : None
  * @date           : 2025/7/19
  ******************************************************************************
  */
#include "ui/CMessageBox.h"

CMessageBox::CMessageBox(QWidget* parent, const QString& message, int displayTime)
    : QDialog(parent)
{
    // 设置窗口属性
    setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog | Qt::Popup);
    setAttribute(Qt::WA_TranslucentBackground);
    setFixedSize(250, 80);

    // 创建透明度效果
    m_pOpacityEffect = new QGraphicsOpacityEffect(this);
    m_pOpacityEffect->setOpacity(1.0);
    setGraphicsEffect(m_pOpacityEffect);

    // 主布局
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 15, 20, 15);
    mainLayout->setSpacing(10);

    // 消息标签
    m_pMessageLabel = new QLabel(message, this);
    m_pMessageLabel->setStyleSheet("font-size: 14px; color: white; font-weight: 500;");
    m_pMessageLabel->setAlignment(Qt::AlignCenter);
    m_pMessageLabel->setWordWrap(true);

    // 添加标签到布局
    mainLayout->addWidget(m_pMessageLabel);

    // 设置计时器
    m_pTimer = new QTimer(this);
    m_pTimer->setSingleShot(true);
    connect(m_pTimer, &QTimer::timeout, this, &CMessageBox::fadeOut);
    m_pTimer->setInterval(displayTime);

    // 设置淡出动画
    m_pFadeAnimation = new QPropertyAnimation(m_pOpacityEffect, "opacity", this);
    m_pFadeAnimation->setDuration(500); // 500ms淡出时间
    m_pFadeAnimation->setStartValue(1.0);
    m_pFadeAnimation->setEndValue(0.0);
    m_pFadeAnimation->setEasingCurve(QEasingCurve::OutCubic);
    connect(m_pFadeAnimation, &QPropertyAnimation::finished, this, &QDialog::accept);
}

void CMessageBox::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 绘制背景
    QPainterPath path;
    path.addRoundedRect(rect(), 10, 10);
    painter.fillPath(path, QColor(74, 144, 226, 220)); // 半透明蓝色背景

    // 绘制边框
    painter.setPen(QPen(QColor(255, 255, 255, 100), 1));
    painter.drawRoundedRect(rect().adjusted(0, 0, -1, -1), 10, 10);
}

void CMessageBox::showToast()
{
    show();

    // 获取应用程序的主窗口
    QWidget* mainWindow = nullptr;

    // 方式1: 通过应用程序的activeWindow获取
    mainWindow = QApplication::activeWindow();

    // 方式2: 如果activeWindow不可用，则遍历所有顶层窗口
    if (!mainWindow)
    {
        const auto topLevelWidgets = QApplication::topLevelWidgets();
        for (QWidget* widget : topLevelWidgets)
        {
            if (widget->isWindow() && widget->objectName() == "MainWindow")
            {
                mainWindow = widget;
                break;
            }
        }
    }

    // 方式3: 如果仍然找不到，使用第一个顶层窗口
    if (!mainWindow && !QApplication::topLevelWidgets().isEmpty())
    {
        mainWindow = QApplication::topLevelWidgets().first();
    }

    // 居中显示在主窗口上
    if (mainWindow)
    {
        QRect mainRect = mainWindow->frameGeometry();
        int x = mainRect.left() + (mainRect.width() - width()) / 2;
        int y = mainRect.top() + (mainRect.height() - height()) / 2;
        move(x, y);
    }
    // 如果没有主窗口，居中显示在屏幕上
    else
    {
        QRect screenGeometry = QApplication::primaryScreen()->geometry();
        int x = (screenGeometry.width() - width()) / 2;
        int y = (screenGeometry.height() - height()) / 2;
        move(x, y);
    }

    // 启动定时器
    m_pTimer->start();
}

void CMessageBox::fadeOut()
{
    // 开始淡出动画
    m_pFadeAnimation->start();
}
