/**
  ******************************************************************************
  * @file           : SerialPortRealTimeSaveWidget.cpp
  * @author         : wangxiangyu
  * @brief          : None
  * @attention      : None
  * @date           : 2025/7/29
  ******************************************************************************
  */

#include "ui/SerialPortRealTimeSaveWidget.h"

SerialPortRealTimeSaveWidget::SerialPortRealTimeSaveWidget(QWidget* parent)
    : QWidget(parent), currentPosition(0), movingRight(true)
{
    this->setUI();
    StyleLoader::loadStyleFromFile(this, ":/resources/qss/serial_port_real_time_save_widget.qss");
}

void SerialPortRealTimeSaveWidget::setUI()
{
    this->setAttribute(Qt::WA_StyledBackground);
    this->createComponents();
    this->createLayout();
    this->connectSignals();
}

bool SerialPortRealTimeSaveWidget::eventFilter(QObject* watched, QEvent* event)
{
    if (watched == m_pSavePathDisplayTextEdit)
    {
        // 拦截输入法事件
        if (event->type() == QEvent::InputMethod)
        {
            return true; // 阻止输入法输入
        }
        // 拦截按键事件
        if (event->type() == QEvent::KeyPress)
        {
            QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
            // 允许功能键和快捷键操作（如Ctrl+C复制）
            if (keyEvent->key() == Qt::Key_Control ||
                keyEvent->key() == Qt::Key_Shift ||
                keyEvent->key() == Qt::Key_Alt ||
                keyEvent->matches(QKeySequence::Copy) ||
                keyEvent->matches(QKeySequence::SelectAll) ||
                keyEvent->matches(QKeySequence::Delete))
            {
                return false; // 放行功能键和快捷键
            }
            return true; // 阻止其他按键输入
        }
    }
    return QWidget::eventFilter(watched, event);
}

void SerialPortRealTimeSaveWidget::createComponents()
{
    m_pPanel = new QWidget(this);
    // 让panel填充整个widget
    m_pPanel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_pPanelLayout = new QVBoxLayout(m_pPanel);
    m_pPanelLayout->setSpacing(2);
    m_pPanelLayout->setContentsMargins(0, 0, 2, 0);
    m_pSavePathDisplayTextEdit = new QPlainTextEdit(m_pPanel);
    m_pSavePathDisplayTextEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_pSavePathDisplayTextEdit->setReadOnly(true);
    m_pSavePathDisplayTextEdit->setUndoRedoEnabled(false);
    // 关键修改：使用任意位置换行而不是在单词边界换行
    m_pSavePathDisplayTextEdit->setLineWrapMode(QPlainTextEdit::WidgetWidth);
    m_pSavePathDisplayTextEdit->setWordWrapMode(QTextOption::WrapAnywhere);
    QFont fixedFont("Consolas", 10);
    if (!QFontDatabase().families().contains("Consolas"))
    {
        fixedFont = QFont("Courier New", 10);
    }
    fixedFont.setStyleHint(QFont::Monospace);
    m_pSavePathDisplayTextEdit->setFont(fixedFont);
    m_pSavePathDisplayTextEdit->installEventFilter(this);
    // 设置固定高度以只显示一行（可根据需要调整）
    QFontMetrics fontMetrics(fixedFont);
    int lineHeight = fontMetrics.height();
    m_pSavePathDisplayTextEdit->setFixedHeight(lineHeight + 20); // 留一些边距
    // 创建进度条容器
    QWidget* progressContainer = new QWidget(m_pPanel);
    progressContainer->setObjectName("progressContainer");
    // 设置进度条和图标
    m_pProgressBar = new QProgressBar();
    m_pProgressBar->setRange(0, 100);
    m_pProgressBar->setValue(100);
    m_pProgressBar->setObjectName("movingIconProgressBar");
    m_pProgressBar->setTextVisible(false);
    // 确保进度条可以扩展
    m_pProgressBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_pIconLabel = new QLabel(m_pProgressBar); // 图标是进度条的子控件
    m_pIconLabel->setFixedSize(iconSize, iconSize);
    m_pIconLabel->setAlignment(Qt::AlignCenter);
    m_pIconLabel->setObjectName("progressIcon");
    // 使用 QIcon 替代 QPixmap，提供更好的兼容性
    QIcon icon(":/resources/icons/ikun.svg"); // 左箭头图标
    QPixmap pixmap = icon.pixmap(iconSize, iconSize);
    m_pIconLabel->setPixmap(pixmap);
    // 将进度条添加到容器，并设置容器的布局
    QHBoxLayout* progressLayout = new QHBoxLayout(progressContainer);
    progressLayout->setContentsMargins(0, 0, 0, 0);
    progressLayout->addWidget(m_pProgressBar);
    // 创建定时器
    m_pTimer = new QTimer(this);
    connect(m_pTimer, &QTimer::timeout, this, &SerialPortRealTimeSaveWidget::moveIcon);
    // 调整组件的高度比例: 文本框占较小比例，进度条占较大比例
    m_pPanelLayout->addWidget(m_pSavePathDisplayTextEdit, 0); // 使用固定大小
    m_pPanelLayout->addWidget(progressContainer, 1); // 占据剩余空间
}

void SerialPortRealTimeSaveWidget::createLayout()
{
    m_pMainLayout = new QVBoxLayout(this);
    m_pMainLayout->setContentsMargins(0, 0, 0, 0);
    m_pMainLayout->setSpacing(0);
    m_pMainLayout->addWidget(m_pPanel);
}

void SerialPortRealTimeSaveWidget::connectSignals()
{
    this->connect(this, &SerialPortRealTimeSaveWidget::sigDisplaySavePath, [this](const QString& path)
    {
        if (path == nullptr)
        {
            m_pTimer->stop();
            return;
        }
        m_pTimer->start(50);
        m_pSavePathDisplayTextEdit->setPlainText(path);
    });
}

void SerialPortRealTimeSaveWidget::moveIcon()
{
    if (!m_pProgressBar || !m_pIconLabel) return;
    int maxPosition = m_pProgressBar->width() - m_pIconLabel->width();
    int yPos = (m_pProgressBar->height() - m_pIconLabel->height()) / 2;
    // 更新位置
    if (movingRight)
    {
        currentPosition += moveStep;
        if (currentPosition >= maxPosition)
        {
            currentPosition = maxPosition;
            movingRight = false;
        }
    }
    else
    {
        currentPosition -= moveStep;
        if (currentPosition <= 0)
        {
            currentPosition = 0;
            movingRight = true;
        }
    }
    // 移动图标（在进度条内部）
    m_pIconLabel->move(currentPosition, yPos);
}
