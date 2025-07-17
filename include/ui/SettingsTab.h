/**
  ******************************************************************************
  * @file           : SettingsTab.h
  * @author         : wangxiangyu
  * @brief          : None
  * @attention      : None
  * @date           : 2025/7/17
  ******************************************************************************
  */

#include <QWidget>

#ifndef SETTINGSTAB_H
#define SETTINGSTAB_H

class SettingsTab : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsTab(QWidget* parent = nullptr);
    ~SettingsTab() = default;

private:
    void setUI();
};

#endif //SETTINGSTAB_H
