/**
  ******************************************************************************
  * @file           : FramelessBase.cpp
  * @author         : wangxiangyu
  * @brief          : None
  * @attention      : None
  * @date           : 2025/7/16
  ******************************************************************************
  */

#include "ui/FramelessBase.h"
#include <QMouseEvent> // 提供鼠标事件处理的类
#include <QApplication> // 提供全局应用程序对象的类
#include <QStyle> // 提供样式相关的功能
#include <QVBoxLayout> // 提供垂直布局管理器

// 构造函数，初始化 FramelessBase 类
FramelessBase::FramelessBase(QWidget* parent, int borderSize)
    : QWidget(parent, Qt::FramelessWindowHint | Qt::WindowMinMaxButtonsHint), // 设置窗口为无边框并启用最小化/最大化按钮
      m_borderSize(borderSize), // 初始化边框大小
      m_resizingEnabled(true), // 默认启用窗口拉伸
      m_borderVisible(false), // 默认边框不可见
      m_borderColor("#3498DB"), // 设置默认边框颜色
      m_borderTop(nullptr), // 初始化顶部边框指针为空
      m_borderRight(nullptr), // 初始化右侧边框指针为空
      m_borderBottom(nullptr), // 初始化底部边框指针为空
      m_borderLeft(nullptr) // 初始化左侧边框指针为空
{
    // 安装应用程序级事件过滤器，用于捕获全局事件
    qApp->installEventFilter(this);

    // 初始化边框控件
    initBorderWidgets();
}

// 设置边框大小
void FramelessBase::setBorderSize(int size)
{
    if (size > 0 && size != m_borderSize)
    {
        // 如果新大小有效且不同于当前大小
        m_borderSize = size; // 更新边框大小
        updateBorderVisibility(m_borderVisible); // 更新边框的可见性
    }
}

// 启用或禁用窗口拉伸功能
void FramelessBase::enableResizing(bool enable)
{
    m_resizingEnabled = enable; // 更新拉伸状态
    if (!enable)
    {
        // 如果禁用拉伸
        unsetCursor(); // 重置光标形状
        updateBorderVisibility(false); // 隐藏边框
    }
}

// 设置边框颜色
void FramelessBase::setBorderColor(const QColor& color)
{
    if (color != m_borderColor)
    {
        // 如果新颜色不同于当前颜色
        m_borderColor = color; // 更新边框颜色
        // 更新边框样式
        QString style = QString("background-color: %1;").arg(m_borderColor.name());
        if (m_borderTop) m_borderTop->setStyleSheet(style); // 更新顶部边框样式
        if (m_borderRight) m_borderRight->setStyleSheet(style); // 更新右侧边框样式
        if (m_borderBottom) m_borderBottom->setStyleSheet(style); // 更新底部边框样式
        if (m_borderLeft) m_borderLeft->setStyleSheet(style); // 更新左侧边框样式
    }
}

// 设置边框是否可见
void FramelessBase::setBorderVisible(bool visible)
{
    if (m_borderVisible != visible)
    {
        // 如果可见性发生变化
        m_borderVisible = visible; // 更新可见性状态
        updateBorderVisibility(visible); // 更新边框的可见性
    }
}

// 初始化边框控件
void FramelessBase::initBorderWidgets()
{
    m_borderTop = new QWidget(this);
    m_borderRight = new QWidget(this);
    m_borderBottom = new QWidget(this);
    m_borderLeft = new QWidget(this);
    // 边框控件不拦截鼠标事件
    m_borderTop->setAttribute(Qt::WA_TransparentForMouseEvents);
    m_borderRight->setAttribute(Qt::WA_TransparentForMouseEvents);
    m_borderBottom->setAttribute(Qt::WA_TransparentForMouseEvents);
    m_borderLeft->setAttribute(Qt::WA_TransparentForMouseEvents);

    setBorderColor(m_borderColor);

    m_borderTop->setFixedHeight(m_borderSize);
    m_borderRight->setFixedWidth(m_borderSize);
    m_borderBottom->setFixedHeight(m_borderSize);
    m_borderLeft->setFixedWidth(m_borderSize);

    updateBorderVisibility(false);
}

// 更新边框的可见性
void FramelessBase::updateBorderVisibility(bool visible)
{
    if (m_borderTop) m_borderTop->setVisible(visible); // 设置顶部边框可见性
    if (m_borderRight) m_borderRight->setVisible(visible); // 设置右侧边框可见性
    if (m_borderBottom) m_borderBottom->setVisible(visible); // 设置底部边框可见性
    if (m_borderLeft) m_borderLeft->setVisible(visible); // 设置左侧边框可见性

    if (visible)
    {
        // 如果边框可见
        // 更新边框位置
        m_borderTop->setGeometry(0, 0, width(), m_borderSize);
        m_borderRight->setGeometry(width() - m_borderSize, 0, m_borderSize, height());
        m_borderBottom->setGeometry(0, height() - m_borderSize, width(), m_borderSize);
        m_borderLeft->setGeometry(0, 0, m_borderSize, height());
    }
}

// 检查是否启用了拉伸功能
bool FramelessBase::isResizeEnabled() const
{
    return m_resizingEnabled && !isMaximized(); // 仅在未最大化时启用拉伸
}

// 检查点是否在拉伸区域内
bool FramelessBase::isPointInResizeArea(const QPoint& point) const
{
    // 放宽底部和右侧的判断，防止 off-by-one 问题
    return (point.y() >= height() - m_borderSize && point.y() < height()) || // 底部边框
        (point.x() >= width() - m_borderSize && point.x() < width()) || // 右侧边框
        (point.y() >= 0 && point.y() < m_borderSize) || // 顶部边框
        (point.x() >= 0 && point.x() < m_borderSize); // 左侧边框
}

// 更新光标形状
void FramelessBase::updateCursorShape(const QPoint& point)
{
    if (!isResizeEnabled())
    {
        // 如果未启用拉伸
        unsetCursor(); // 重置光标
        return;
    }

    // 判断光标所在的边框位置
    bool atTop = (point.y() >= 0 && point.y() < m_borderSize);
    bool atRight = (point.x() >= width() - m_borderSize && point.x() < width());
    bool atBottom = (point.y() >= height() - m_borderSize && point.y() < height());
    bool atLeft = (point.x() >= 0 && point.x() < m_borderSize);

    // 根据位置设置光标形状
    if (atTop && atLeft)
    {
        setCursor(Qt::SizeFDiagCursor); // 左上角
    }
    else if (atTop && atRight)
    {
        setCursor(Qt::SizeBDiagCursor); // 右上角
    }
    else if (atBottom && atLeft)
    {
        setCursor(Qt::SizeBDiagCursor); // 左下角
    }
    else if (atBottom && atRight)
    {
        setCursor(Qt::SizeFDiagCursor); // 右下角
    }
    else if (atTop || atBottom)
    {
        setCursor(Qt::SizeVerCursor); // 上下边框
    }
    else if (atLeft || atRight)
    {
        setCursor(Qt::SizeHorCursor); // 左右边框
    }
    else
    {
        unsetCursor(); // 重置光标
    }
}

// 获取拉伸的边缘
Qt::Edges FramelessBase::getResizeEdges(const QPoint& point) const
{
    Qt::Edges edges = Qt::Edges(); // 初始化为空

    if (point.y() >= 0 && point.y() < m_borderSize) edges |= Qt::TopEdge;
    if (point.x() >= width() - m_borderSize && point.x() < width()) edges |= Qt::RightEdge;
    if (point.y() >= height() - m_borderSize && point.y() < height()) edges |= Qt::BottomEdge;
    if (point.x() >= 0 && point.x() < m_borderSize) edges |= Qt::LeftEdge;

    return edges;
}

// 处理窗口拉伸
void FramelessBase::handleResize(const QPoint& point)
{
    if (!windowHandle()) return; // 如果没有窗口句柄，直接返回

    Qt::Edges edges = getResizeEdges(point); // 获取拉伸边缘
    if (edges != Qt::Edges())
    {
        // 如果边缘有效
        windowHandle()->startSystemResize(edges); // 开始系统级拉伸
    }
}

// 处理窗口大小调整事件
void FramelessBase::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event); // 调用基类的 resizeEvent
    updateBorderVisibility(m_borderVisible); // 更新边框可见性
}

// 事件过滤器
bool FramelessBase::eventFilter(QObject* obj, QEvent* event)
{
    if (!isResizeEnabled())
    {
        // 如果未启用拉伸
        return QWidget::eventFilter(obj, event); // 调用基类的事件过滤器
    }

    switch (event->type())
    {
    // 根据事件类型处理
    case QEvent::MouseMove:
        {
            // 鼠标移动事件
            QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
            // 用全局坐标转换为主窗口坐标
            QPoint globalPos = mouseEvent->globalPosition().toPoint();
            QPoint localPos = this->mapFromGlobal(globalPos);

            // 更新光标形状
            updateCursorShape(localPos);

            // 更新边框可见性
            bool inResizeArea = isPointInResizeArea(localPos);
            if (inResizeArea != m_borderVisible)
            {
                m_borderVisible = inResizeArea;
                updateBorderVisibility(inResizeArea);
            }
            break;
        }

    case QEvent::MouseButtonPress:
        {
            // 鼠标按下事件
            QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
            if (mouseEvent->button() == Qt::LeftButton)
            {
                // 如果是左键
                QPoint globalPos = mouseEvent->globalPosition().toPoint();
                QPoint localPos = this->mapFromGlobal(globalPos);
                if (isPointInResizeArea(localPos))
                {
                    // 如果在拉伸区域
                    handleResize(localPos); // 处理拉伸
                    return true; // 事件已处理
                }
            }
            break;
        }

    case QEvent::WindowStateChange:
        {
            // 窗口状态改变事件
            // 最大化时禁用拉伸
            if (isMaximized())
            {
                unsetCursor(); // 重置光标
                updateBorderVisibility(false); // 隐藏边框
            }
            break;
        }

    default:
        break;
    }

    return QWidget::eventFilter(obj, event); // 调用基类的事件过滤器
}
