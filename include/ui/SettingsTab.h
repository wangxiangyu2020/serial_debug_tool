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
