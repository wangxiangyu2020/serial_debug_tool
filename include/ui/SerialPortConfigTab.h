/**
  ******************************************************************************
  * @file           : SerialPortConfig.h
  * @author         : wangxiangyu
  * @brief          : None
  * @attention      : None
  * @date           : 2025/7/17
  ******************************************************************************
  */

#include <QWidget>
#include "ui/SerialPortConnectConfigWidget.h"

#ifndef SERIALPORTCONFIG_H
#define SERIALPORTCONFIG_H

class SerialPortConfigTab : public QWidget
{
    Q_OBJECT

public:
    explicit SerialPortConfigTab(QWidget* parent = nullptr);
    ~SerialPortConfigTab() = default;

private:
    void setUI();

private:
    SerialPortConnectConfigWidget* m_pSerialPortConfigWidget = nullptr;
};

#endif //SERIALPORTCONFIG_H
