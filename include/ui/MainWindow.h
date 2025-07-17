/**
  ******************************************************************************
  * @file           : MainWindow.h
  * @author         : wangxiangyu
  * @brief          : None
  * @attention      : None
  * @date           : 2025/7/16
  ******************************************************************************
  */

#include "ui/FramelessBase.h"
#include "ui/TitleBar.h"
#include "ui/CTabWidget.h"

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

class MainWindow : public FramelessBase
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() = default;

private:
    void setUI();

private:
    TitleBar* m_pTitleBar = nullptr;
    CTabWidget* m_pTabContainer = nullptr;
};

#endif //MAINWINDOW_H
