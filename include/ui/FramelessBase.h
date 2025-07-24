/**
  ******************************************************************************
  * @file           : FramelessBase.h
  * @author         : wangxiangyu
  * @brief          : None
  * @attention      : None
  * @date           : 2025/7/16
  ******************************************************************************
  */


#ifndef FRAMELESSBASE_H
#define FRAMELESSBASE_H

#include <QWidget>
#include <QWindow>
#include <QFlags>
#include <QColor>
#include <QMouseEvent> // 提供鼠标事件处理的类
#include <QApplication> // 提供全局应用程序对象的类
#include <QStyle> // 提供样式相关的功能
#include <QVBoxLayout> // 提供垂直布局管理器

class FramelessBase : public QWidget
{
    Q_OBJECT

public:
    explicit FramelessBase(QWidget* parent = nullptr, int borderSize = 5);
    virtual ~FramelessBase() = default;

    // 设置边框大小（热区宽度）
    void setBorderSize(int size);
    // 启用/禁用拉伸功能
    void enableResizing(bool enable);
    // 设置边框颜色（可选）
    void setBorderColor(const QColor& color);
    // 设置是否显示边框（视觉反馈）
    void setBorderVisible(bool visible);

protected:
    // 事件处理
    bool eventFilter(QObject* obj, QEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

    // 子类可重写的方法
    virtual bool isResizeEnabled() const;
    virtual bool isPointInResizeArea(const QPoint& point) const;
    virtual void updateCursorShape(const QPoint& point);
    virtual void handleResize(const QPoint& point);
    virtual void updateBorderVisibility(bool visible);

private:
    void initBorderWidgets();
    Qt::Edges getResizeEdges(const QPoint& point) const;

    int m_borderSize;
    bool m_resizingEnabled;
    bool m_borderVisible;
    QColor m_borderColor;

    // 边框控件（可选）
    QWidget* m_borderTop;
    QWidget* m_borderRight;
    QWidget* m_borderBottom;
    QWidget* m_borderLeft;
};

#endif //FRAMELESSBASE_H
