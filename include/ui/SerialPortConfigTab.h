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
#include "ui/SerialPortReceiveSettingsWidget.h"
#include "ui/SerialPortSendSettingsWidget.h"
#include "ui/SerialPortDataReceiveWidget.h"
#include "ui/SerialPortDataSendWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>

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
    QHBoxLayout* m_pMainLayout = nullptr;
    QVBoxLayout* m_pSettingsLayout = nullptr;
    QVBoxLayout* m_pContentLayout = nullptr;
    SerialPortConnectConfigWidget* m_pSerialPortConfigWidget = nullptr;
    SerialPortReceiveSettingsWidget* m_pSerialPortReceiveSettingsWidget = nullptr;
    SerialPortSendSettingsWidget* m_pSerialPortSendSettingsWidget = nullptr;
    SerialPortDataReceiveWidget* m_pSerialPortDataReceiveWidget = nullptr;
    SerialPortDataSendWidget* m_pSerialPortDataSendWidget = nullptr;
};

#endif //SERIALPORTCONFIG_H
