/**
  ******************************************************************************
  * @file           : MainWindow.h
  * @author         : wangxiangyu
  * @brief          : None
  * @attention      : None
  * @date           : 2025/7/16
  ******************************************************************************
  */


#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui/FramelessBase.h"
#include "ui/TitleBar.h"
#include "ui/CTabWidget.h"
#include <QVBoxLayout>
#include "utils/StyleLoader.h"

class MainWindow : public FramelessBase
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() = default;

private:
    void setUI();

    TitleBar* m_pTitleBar = nullptr;
    CTabWidget* m_pTabContainer = nullptr;
};

#endif //MAINWINDOW_H
