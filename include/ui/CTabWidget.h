/**
  ******************************************************************************
  * @file           : TabContainer.h
  * @author         : wangxiangyu
  * @brief          : None
  * @attention      : None
  * @date           : 2025/7/16
  ******************************************************************************
  */


#ifndef TABCONTAINER_H
#define TABCONTAINER_H

#include <QTabWidget>
#include "ui/SettingsTab.h"
#include "ui/WaveformTab.h"
#include "ui/SerialPortConfigTab.h"
#include "utils/StyleLoader.h"
#include <QTabWidget>
#include <QTabBar>
#include <QHBoxLayout>
#include <QSpacerItem>

class CTabWidget : public QTabWidget
{
    Q_OBJECT

public:
    // 构造函数和析构函数
    explicit CTabWidget(QWidget* parent = nullptr);
    ~CTabWidget() = default;

private:
    // 私有方法
    void setUI();

    // UI组件成员
    SerialPortConfigTab* m_pSerialPortConfigTab = nullptr;
    WaveformTab* m_pWaveformTab = nullptr;
    SettingsTab* m_pSettingsTab = nullptr;
};

#endif //TABCONTAINER_H
