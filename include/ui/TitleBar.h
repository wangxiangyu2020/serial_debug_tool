/**
  ******************************************************************************
  * @file           : TitleBar.h
  * @author         : wangxiangyu
  * @brief          : None
  * @attention      : None
  * @date           : 2025/7/16
  ******************************************************************************
  */

#ifndef TITLEBAR_H
#define TITLEBAR_H

#pragma comment(lib, "user32.lib")
#include <qt_windows.h>
#include <QWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QApplication>
#include "utils/StyleLoader.h"

class TitleBar : public QWidget
{
    Q_OBJECT

public:
    explicit TitleBar(QWidget* parent = nullptr);
    ~TitleBar() = default;

private:
    void setUI();
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;

private slots :
    void onClicked();

signals:
    void sigClosed();

private:
    QPushButton* m_pLogoBtn = nullptr;
    QPushButton* m_pMinBtn = nullptr;
    QPushButton* m_pMaxBtn = nullptr;
    QPushButton* m_pCloseBtn = nullptr;

    bool m_dragging = false;
    QPoint m_dragStartPosition;
};

#endif //TITLEBAR_H
