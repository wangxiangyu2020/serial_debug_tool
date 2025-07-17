/**
  ******************************************************************************
  * @file           : TabContainer.h
  * @author         : wangxiangyu
  * @brief          : None
  * @attention      : None
  * @date           : 2025/7/16
  ******************************************************************************
  */

#include <QTabWidget>
#include "ui/SettingsTab.h"
#include "ui/WaveformTab.h"
#include "ui/SerialPortConfigTab.h"

#ifndef TABCONTAINER_H
#define TABCONTAINER_H

class CTabWidget : public QTabWidget
{
    Q_OBJECT

public:
    explicit CTabWidget(QWidget* parent = nullptr);
    ~CTabWidget() = default;

private:
    void setUI();

private:
    SerialPortConfigTab* m_pSerialPortConfigTab = nullptr;
    WaveformTab* m_pWaveformTab = nullptr;
    SettingsTab* m_pSettingsTab = nullptr;
};

#endif //TABCONTAINER_H
