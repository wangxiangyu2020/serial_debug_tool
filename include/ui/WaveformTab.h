/**
  ******************************************************************************
  * @file           : WaveformTab.h
  * @author         : wangxiangyu
  * @brief          : None
  * @attention      : None
  * @date           : 2025/7/17
  ******************************************************************************
  */

#include <QWidget>

#ifndef WAVEFORMTAB_H
#define WAVEFORMTAB_H

class WaveformTab : public QWidget
{
    Q_OBJECT

public:
    explicit WaveformTab(QWidget* parent = nullptr);
    ~WaveformTab() = default;

private:
    void setUI();
};

#endif //WAVEFORMTAB_H
