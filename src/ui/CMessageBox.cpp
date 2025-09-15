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
#include "utils/StyleLoader.h"
#include <QHBoxLayout>

/**
 * @brief Confirm对话框的私有构造函数
 */
CMessageBox::CMessageBox(QWidget* parent, const QString& title, const QString& message)
    : QDialog(parent), m_isConfirmDialog(true)
{
    // 1. 设置窗口属性 (保持原有尺寸)
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setModal(true);
    this->setFixedSize(320, 160);
    this->setProperty("dialogType", "confirm");

    // 2. 创建文本控件 (保持原有居中布局)
    m_pTitleLabel = new QLabel(title, this);
    m_pTitleLabel->setObjectName("titleLabel");
    m_pTitleLabel->setAlignment(Qt::AlignCenter);
    m_pTitleLabel->setWordWrap(true);

    m_pMessageLabel = new QLabel(message, this);
    m_pMessageLabel->setObjectName("messageLabel");
    m_pMessageLabel->setAlignment(Qt::AlignCenter);
    m_pMessageLabel->setWordWrap(true);

    // 3. 创建按钮
    m_pOkButton = new QPushButton("确定", this);
    m_pOkButton->setObjectName("okButton");
    m_pOkButton->setCursor(Qt::PointingHandCursor);
    // 确定按钮 - 蓝色 (调整为更小的尺寸)
    m_pOkButton->setStyleSheet(
        "QPushButton {"
        "    background-color: #007bff; color: white; border: none;"
        "    border-radius: 4px; padding: 6px 16px; font-size: 12px;"
        "    font-weight: 500; font-family: 'Microsoft YaHei';"
        "    min-width: 60px; max-height: 28px;"
        "}"
        "QPushButton:hover { background-color: #0056b3; }"
        "QPushButton:pressed { background-color: #004085; }"
    );

    m_pCancelButton = new QPushButton("取消", this);
    m_pCancelButton->setObjectName("cancelButton");
    m_pCancelButton->setCursor(Qt::PointingHandCursor);
    // 取消按钮 - 灰色 (调整为更小的尺寸)
    m_pCancelButton->setStyleSheet(
        "QPushButton {"
        "    background-color: #6c757d;"
        "    color: white;"
        "    border: 1px solid #dee2e6;"
        "    border-radius: 4px;"
        "    padding: 6px 16px;"
        "    font-size: 12px;"
        "    font-weight: 500;"
        "    font-family: 'Microsoft YaHei';"
        "    min-width: 60px; max-height: 28px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #5c636a;"
        "    border-color: #adb5bd;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #545b62;"
        "    border-color: #6c757d;"
        "}"
    );

    // 4. 创建布局 (保持原有垂直布局)
    setupConfirmLayout();

    // 5. 加载样式
    StyleLoader::loadStyleFromFile(this, ":/resources/qss/message_box.qss");

    // 6. 连接信号
    connect(m_pOkButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(m_pCancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

void CMessageBox::setupConfirmLayout()
{
    // 主布局 (保持原有的垂直布局)
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(24, 20, 24, 20);
    mainLayout->setSpacing(0);

    // 标题区域
    mainLayout->addWidget(m_pTitleLabel);
    mainLayout->addSpacing(12);
    
    // 消息区域
    mainLayout->addWidget(m_pMessageLabel, 1);
    mainLayout->addSpacing(24);

    // 按钮区域 (保持原有布局)
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(12);
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_pCancelButton);
    buttonLayout->addWidget(m_pOkButton);

    mainLayout->addLayout(buttonLayout);
}

/**
 * @brief 显示确认对话框的静态方法
 */
bool CMessageBox::confirm(QWidget* parent, const QString& title, const QString& message)
{
    // 使用新的私有构造函数创建对话框
    CMessageBox confirmDialog(parent, title, message);
    return confirmDialog.exec() == QDialog::Accepted;
}

// Toast构造函数 (保持原有简洁设计)
CMessageBox::CMessageBox(QWidget* parent, const QString& message, int displayTime)
    : QDialog(parent), m_isConfirmDialog(false)
{
    // 设置窗口属性 (保持原有尺寸)
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog | Qt::Popup);
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setFixedSize(250, 80);
    this->setProperty("dialogType", "toast");

    // 创建透明度效果
    m_pOpacityEffect = new QGraphicsOpacityEffect(this);
    m_pOpacityEffect->setOpacity(1.0);
    this->setGraphicsEffect(m_pOpacityEffect);

    // 消息标签 (保持原有居中布局)
    m_pMessageLabel = new QLabel(message, this);
    m_pMessageLabel->setObjectName("messageLabel");
    m_pMessageLabel->setAlignment(Qt::AlignCenter);
    m_pMessageLabel->setWordWrap(true);
    // 直接设置白色字体
    m_pMessageLabel->setStyleSheet("font-size: 14px; color: white; font-weight: 500; font-family: 'Microsoft YaHei';");

    // 布局 (保持原有简洁布局)
    this->setupToastLayout();

    // 加载样式
    StyleLoader::loadStyleFromFile(this, ":/resources/qss/message_box.qss");

    // 设置计时器和动画
    this->setupToastAnimation(displayTime);
}

void CMessageBox::setupToastLayout()
{
    // 保持原有的简洁垂直布局
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 15, 20, 15);
    mainLayout->setSpacing(10);
    
    mainLayout->addWidget(m_pMessageLabel);
}

void CMessageBox::setupToastAnimation(int displayTime)
{
    // 设置计时器
    m_pTimer = new QTimer(this);
    m_pTimer->setSingleShot(true);
    this->connect(m_pTimer, &QTimer::timeout, this, &CMessageBox::fadeOut);
    m_pTimer->setInterval(displayTime);

    // 设置淡出动画
    m_pFadeAnimation = new QPropertyAnimation(m_pOpacityEffect, "opacity", this);
    m_pFadeAnimation->setDuration(500);
    m_pFadeAnimation->setStartValue(1.0);
    m_pFadeAnimation->setEndValue(0.0);
    m_pFadeAnimation->setEasingCurve(QEasingCurve::OutCubic);
    this->connect(m_pFadeAnimation, &QPropertyAnimation::finished, this, &QDialog::accept);
}

// 主要业务方法
void CMessageBox::showToast()
{
    this->show();

    // 优先使用父窗口计算位置
    QWidget* parent = parentWidget();
    QWidget* mainWindow = parent ? parent->window() : nullptr;

    // 如果父窗口无效，尝试其他方法查找主窗口
    if (!mainWindow)
    {
        // 方式1: 通过应用程序的activeWindow获取
        mainWindow = QApplication::activeWindow();

        // 方式2: 如果activeWindow不可用，则遍历所有顶层窗口
        if (!mainWindow)
        {
            const auto topLevelWidgets = QApplication::topLevelWidgets();
            for (QWidget* widget : topLevelWidgets)
            {
                if (widget->isWindow() && widget->isActiveWindow())
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
    }

    // 居中显示
    if (mainWindow)
    {
        // 使用更精确的窗口区域计算
        QRect mainRect = mainWindow->frameGeometry();
        int x = mainRect.x() + (mainRect.width() - width()) / 2;
        int y = mainRect.y() + (mainRect.height() - height()) / 2;
        move(x, y);
    }
    else
    {
        // 屏幕居中（支持多显示器）
        QScreen* screen = QGuiApplication::screenAt(QCursor::pos());
        if (!screen) screen = QApplication::primaryScreen();
        QRect screenGeometry = screen->geometry();
        int x = (screenGeometry.width() - width()) / 2;
        int y = (screenGeometry.height() - height()) / 2;
        move(x, y);
    }

    m_pTimer->start();
}

// 静态方法
void CMessageBox::showToast(const QString& message)
{
    CMessageBox* toast = new CMessageBox(nullptr, message, 1500); // 显示1.5秒
    toast->showToast();
    toast->setAttribute(Qt::WA_DeleteOnClose); // 自动删除
}

void CMessageBox::showToast(QWidget* parent, const QString& message)
{
    CMessageBox* toast = new CMessageBox(parent, message, 1500); // 显示1.5秒
    toast->showToast();
    toast->setAttribute(Qt::WA_DeleteOnClose); // 自动删除
}

// 事件处理方法
void CMessageBox::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    if (m_isConfirmDialog) {
        // 确认弹框 - 现代化设计
        drawConfirmDialog(painter);
    } else {
        // Toast弹框 - 现代化设计
        drawToastDialog(painter);
    }
}

void CMessageBox::drawConfirmDialog(QPainter& painter)
{
    QRect dialogRect = rect();
    
    // 启用抗锯齿，确保圆角平滑
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    
    // 绘制阴影 - 使用圆角路径
    painter.setPen(Qt::NoPen);
    for (int i = 1; i <= 4; ++i) {
        QRect shadowRect = dialogRect.adjusted(-i, -i, i, i);
        QPainterPath shadowPath;
        shadowPath.addRoundedRect(shadowRect, 10, 10);
        painter.setBrush(QColor(0, 0, 0, 4));
        painter.drawPath(shadowPath);
    }
    
    // 绘制主背景 - 只使用圆角路径
    QPainterPath mainPath;
    mainPath.addRoundedRect(dialogRect, 10, 10);
    painter.setBrush(QColor(255, 255, 255, 250));
    painter.drawPath(mainPath);
    
    // 绘制边框 - 只使用圆角路径
    QPainterPath borderPath;
    borderPath.addRoundedRect(dialogRect.adjusted(0, 0, -1, -1), 10, 10);
    painter.setBrush(Qt::NoBrush);
    painter.setPen(QPen(QColor(230, 230, 230, 200), 1));
    painter.drawPath(borderPath);
}

void CMessageBox::drawToastDialog(QPainter& painter)
{
    QRect dialogRect = rect();
    
    // 绘制阴影 (保持原有风格)
    painter.setPen(Qt::NoPen);
    for (int i = 0; i < 5; ++i) {
        int alpha = 6 - i;
        painter.setBrush(QColor(0, 0, 0, alpha));
        QPainterPath shadowPath;
        shadowPath.addRoundedRect(dialogRect.adjusted(-i, -i, i, i), 10 + i, 10 + i);
        painter.drawPath(shadowPath);
    }
    
    // 绘制主背景 - 更淡的蓝色背景
    QPainterPath mainPath;
    mainPath.addRoundedRect(dialogRect, 10, 10);
    painter.fillPath(mainPath, QColor(74, 144, 226, 150)); // 进一步降低透明度到150
    
    // 绘制边框
    painter.setPen(QPen(QColor(255, 255, 255, 60), 1));
    painter.drawRoundedRect(dialogRect.adjusted(0, 0, -1, -1), 10, 10);
}

// private slots
void CMessageBox::fadeOut()
{
    // 开始淡出动画
    m_pFadeAnimation->start();
}
