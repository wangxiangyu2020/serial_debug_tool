/**
  ******************************************************************************
  * @file           : CDialogBase.cpp
  * @author         : wangxiangyu
  * @brief          : None
  * @attention      : None
  * @date           : 2025/7/31
  ******************************************************************************
  */

#include "ui/CDialogBase.h"

// 构造函数和析构函数
CDialogBase::CDialogBase(QWidget* parent, const QString& title, const QSize& size)
    : QDialog(parent)
{
    // 设置标题
    if (!title.isEmpty()) m_pTitleLabel = new QLabel(title, this);
    this->setFixedSize(size);
    this->setUI();
    // 加载基础对话框样式
    StyleLoader::loadStyleFromFile(this, ":/resources/qss/dialog_base.qss");
}

// 事件处理方法
void CDialogBase::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    // 绘制圆角背景
    QPainterPath path;
    path.addRoundedRect(rect(), 12, 12);
    painter.fillPath(path, QColor(248, 249, 250, 240));
    // 绘制边框
    painter.setPen(QPen(QColor(233, 236, 239), 1));
    painter.drawRoundedRect(rect().adjusted(0, 0, -1, -1), 12, 12);
}

// 重写基类虚函数
void CDialogBase::onConfirmClicked()
{
    this->accept();
}

void CDialogBase::onCancelClicked()
{
    this->reject();
}

// 私有方法
void CDialogBase::setUI()
{
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setupAnimation();

    this->createBaseComponents();
    this->createBaseLayout();
    this->connectBaseSignals();

    m_pFadeAnimation->start();
}

void CDialogBase::createBaseComponents()
{
    m_pConfirmButton = new QPushButton("确定", this);
    m_pCancelButton = new QPushButton("取消", this);
}

void CDialogBase::createBaseLayout()
{
    m_pMainLayout = new QVBoxLayout(this);
    m_pMainLayout->setContentsMargins(20, 20, 20, 20);
    m_pMainLayout->setSpacing(15);
    // 添加标题
    m_pMainLayout->addWidget(m_pTitleLabel);
    // 创建布局容器
    m_pContentLayout = new QVBoxLayout();
    m_pContentLayout->setSpacing(10);
    m_pMainLayout->addLayout(m_pContentLayout);
    // 创建按钮布局
    m_pButtonLayout = new QHBoxLayout();
    m_pButtonLayout->addStretch();
    m_pButtonLayout->addWidget(m_pCancelButton);
    m_pButtonLayout->addWidget(m_pConfirmButton);
    m_pMainLayout->addLayout(m_pButtonLayout);
}

void CDialogBase::connectBaseSignals()
{
    this->connect(m_pConfirmButton, &QPushButton::clicked, this, &CDialogBase::onConfirmClicked);
    this->connect(m_pCancelButton, &QPushButton::clicked, this, &CDialogBase::onCancelClicked);
}

void CDialogBase::setupAnimation()
{
    m_pOpacityEffect = new QGraphicsOpacityEffect(this);
    m_pOpacityEffect->setOpacity(0.0);
    this->setGraphicsEffect(m_pOpacityEffect);
    m_pFadeAnimation = new QPropertyAnimation(m_pOpacityEffect, "opacity", this);
    m_pFadeAnimation->setDuration(300);
    m_pFadeAnimation->setStartValue(0.0);
    m_pFadeAnimation->setEndValue(1.0);
}
