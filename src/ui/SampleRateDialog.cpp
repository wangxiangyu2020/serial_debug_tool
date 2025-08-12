/**
******************************************************************************
  * @file           : SampleRateDialog.cpp
  * @author         : wangxiangyu
  * @brief          : None
  * @attention      : None
  * @date           : 2025/8/10
  ******************************************************************************
  */

#include "ui/SampleRateDialog.h"

#include <QStyle>

// 构造函数和析构函数
SampleRateDialog::SampleRateDialog(QWidget* parent)
    : CDialogBase(parent, "采样间隔设置", QSize(300, 150)) // 修改标题
{
    this->setUI();
    // 使用事件循环后加载样式确保所有组件都已创建
    QMetaObject::invokeMethod(this, [this]()
    {
        // 保存现有的父类样式
        QString parentStyleSheet = this->styleSheet();

        // 加载子类专用样式
        QString childStyleSheet = StyleLoader::loadStyleFromFileToString(":/resources/qss/sample_rate_dialog.qss");

        // 合并样式表（子类样式在后，具有更高优先级）
        this->setStyleSheet(parentStyleSheet + childStyleSheet);

        // 强制刷新样式
        this->style()->unpolish(this);
        this->style()->polish(this);
        this->update();
    }, Qt::QueuedConnection);
}

// 获取方法
int SampleRateDialog::getSampleRate() const
{
    return m_pSampleRateSpinBox->value();
}

// 配置方法
void SampleRateDialog::setSampleRate(int rate)
{
    m_pSampleRateSpinBox->setValue(rate);
}

// 重写基类虚函数
void SampleRateDialog::createComponents()
{
    // 创建数值输入框
    m_pSampleRateSpinBox = new QSpinBox(this);
    m_pSampleRateSpinBox->setObjectName("sampleRateSpinBox");

    // 设置只能输入数字的范围，可根据实际需求调整
    m_pSampleRateSpinBox->setRange(1, 999999); // 设置数值范围
    m_pSampleRateSpinBox->setSuffix(" ms"); // 添加单位后缀

    // 启用鼠标滚轮调节
    m_pSampleRateSpinBox->setFocusPolicy(Qt::StrongFocus);

    if (m_pTitleLabel) m_pTitleLabel->setObjectName("titleLabel");
    // 设置按钮对象名称用于样式
    if (m_pCancelButton) m_pCancelButton->setObjectName("cancelButton");
    if (m_pConfirmButton) m_pConfirmButton->setObjectName("confirmButton");
}

void SampleRateDialog::createContentLayout()
{
    if (!m_pContentLayout || !m_pSampleRateSpinBox) return;

    // 创建水平布局放置标签和输入框
    QHBoxLayout* inputLayout = new QHBoxLayout();
    inputLayout->setObjectName("inputLayout");
    inputLayout->addWidget(m_pSampleRateSpinBox);
    inputLayout->addStretch(); // 添加弹性空间

    m_pContentLayout->addLayout(inputLayout);
}

void SampleRateDialog::connectSignals()
{
}

void SampleRateDialog::onConfirmClicked()
{
    CDialogBase::onConfirmClicked();
}

// 私有方法
void SampleRateDialog::setUI()
{
    this->setAttribute(Qt::WA_StyledBackground, true);
    // 手动调用初始化方法
    this->createComponents();
    this->createContentLayout();
    this->connectSignals();
}
