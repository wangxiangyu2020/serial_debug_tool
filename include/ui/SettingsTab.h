/**
  ******************************************************************************
  * @file           : SettingsTab.h
  * @author         : wangxiangyu
  * @brief          : None
  * @attention      : None
  * @date           : 2025/7/17
  ******************************************************************************
  */


#ifndef SETTINGSTAB_H
#define SETTINGSTAB_H

#include <QWidget>
#include "utils/StyleLoader.h"
#include <QTabWidget>
#include <QTextEdit>
#include <QVBoxLayout>

class SettingsTab : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsTab(QWidget* parent = nullptr);
    ~SettingsTab() = default;

private:
    void setUI();
    void createComponents();

private:
    QTabWidget* m_pTabWidget = nullptr;
    QTextEdit* m_pReadmeViewer = nullptr;
};

#endif //SETTINGSTAB_H
