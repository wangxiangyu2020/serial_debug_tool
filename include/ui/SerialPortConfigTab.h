/**
  ******************************************************************************
  * @file           : SerialPortConfig.h
  * @author         : wangxiangyu
  * @brief          : None
  * @attention      : None
  * @date           : 2025/7/17
  ******************************************************************************
  */


#ifndef SERIALPORTCONFIG_H
#define SERIALPORTCONFIG_H

#include <QWidget>
#include "ui/SerialPortConnectConfigWidget.h"
#include "ui/SerialPortReceiveSettingsWidget.h"
#include "ui/SerialPortSendSettingsWidget.h"
#include "ui/SerialPortDataReceiveWidget.h"
#include "ui/SerialPortDataSendWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "utils/StyleLoader.h"
#include "ui/SerialPortRealTimeSaveWidget.h"
#include <QFileDialog>

class SerialPortConfigTab : public QWidget
{
    Q_OBJECT

public:
    // 构造函数和析构函数
    explicit SerialPortConfigTab(QWidget* parent = nullptr);
    ~SerialPortConfigTab() = default;

signals:
    void displaySavePathRequested(const QString& path = nullptr);

private slots:
    void onReadySaveFile(bool status);

private:
    // 私有方法
    void setUI();
    void createComponents();
    void createLayout();
    void connectSignals();

    // 布局成员
    QHBoxLayout* m_pMainLayout = nullptr;
    QVBoxLayout* m_pSettingsLayout = nullptr;
    QVBoxLayout* m_pContentLayout = nullptr;
    QWidget* m_pSettingsPanel = nullptr;
    QWidget* m_pContentPanel = nullptr;

    // UI组件成员
    SerialPortConnectConfigWidget* m_pSerialPortConfigWidget = nullptr;
    SerialPortReceiveSettingsWidget* m_pSerialPortReceiveSettingsWidget = nullptr;
    SerialPortSendSettingsWidget* m_pSerialPortSendSettingsWidget = nullptr;
    SerialPortDataReceiveWidget* m_pSerialPortDataReceiveWidget = nullptr;
    SerialPortDataSendWidget* m_pSerialPortDataSendWidget = nullptr;
    SerialPortRealTimeSaveWidget* m_pSerialPortRealTimeSaveWidget = nullptr;

    // 文件对象
    QFile* m_pSaveFile = nullptr;
};

#endif //SERIALPORTCONFIG_H
