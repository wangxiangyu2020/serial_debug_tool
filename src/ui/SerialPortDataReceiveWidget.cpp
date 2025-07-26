/**
  ******************************************************************************
  * @file           : SerialPortDataReceiveWidget.cpp
  * @author         : wangxiangyu
  * @brief          : None
  * @attention      : None
  * @date           : 2025/7/19
  ******************************************************************************
  */

#include "ui/SerialPortDataReceiveWidget.h"

SerialPortDataReceiveWidget::SerialPortDataReceiveWidget(QWidget* parent)
    : QWidget(parent)
{
    this->setUI();
    StyleLoader::loadStyleFromFile(this, ":/resources/qss/serial_port_data_receive_widget.qss");
}

bool SerialPortDataReceiveWidget::eventFilter(QObject* watched, QEvent* event)
{
    if (watched == m_pReceiveTextEdit)
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
                keyEvent->matches(QKeySequence::SelectAll))
            {
                return false; // 放行功能键和快捷键
            }
            return true; // 阻止其他按键输入
        }
    }
    return QWidget::eventFilter(watched, event);
}

void SerialPortDataReceiveWidget::setUI()
{
    this->setAttribute(Qt::WA_StyledBackground);
    this->createComponents();
    this->createLayout();
    this->connectSignals();
}

void SerialPortDataReceiveWidget::createComponents()
{
    // 接收文本编辑框
    m_pReceiveTextEdit = new QPlainTextEdit(this);
    m_pReceiveTextEdit->setReadOnly(true); // 设置为只读
    m_pReceiveTextEdit->setUndoRedoEnabled(false); // 禁用撤销/重做
    m_pReceiveTextEdit->setLineWrapMode(QPlainTextEdit::WidgetWidth); // 设置自动换行模式为按窗口宽度换行
    // 使用等宽字体
    QFont fixedFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    fixedFont.setPointSize(10);
    m_pReceiveTextEdit->setFont(fixedFont);
    // 安装事件过滤器
    m_pReceiveTextEdit->installEventFilter(this);
}

void SerialPortDataReceiveWidget::createLayout()
{
    // 主垂直布局 - 无间距和边距
    m_pMainLayout = new QVBoxLayout(this);
    m_pMainLayout->setSpacing(0);
    m_pMainLayout->setContentsMargins(0, 0, 0, 0);
    // 添加到布局 - 文本编辑框占据整个容器
    m_pMainLayout->addWidget(m_pReceiveTextEdit);
}

void SerialPortDataReceiveWidget::connectSignals()
{
    this->connect(new SerialPortManager(), &SerialPortManager::sigReceiveData, this,
                  &SerialPortDataReceiveWidget::showReceiveData);
}

void SerialPortDataReceiveWidget::showReceiveData(const QByteArray& data)
{
    // 暂停重绘以提高性能
    m_pReceiveTextEdit->setUpdatesEnabled(false);
    // 获取当前滚动条位置
    QScrollBar* vScroll = m_pReceiveTextEdit->verticalScrollBar();
    bool atBottom = vScroll->value() == vScroll->maximum();
    QString receivedString = QString::fromUtf8(data).trimmed();
    m_pReceiveTextEdit->appendPlainText(receivedString);
    // 恢复自动滚动（如果启用且之前已在底部）
    if (atBottom)
    {
        vScroll->setValue(vScroll->maximum());
    }
    // 恢复重绘
    m_pReceiveTextEdit->setUpdatesEnabled(true);
}
