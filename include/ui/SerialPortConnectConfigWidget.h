/**
  ******************************************************************************
  * @file           : SerialPortConnectConfigWidget.h
  * @author         : wangxiangyu
  * @brief          : None
  * @attention      : None
  * @date           : 2025/7/17
  ******************************************************************************
  */

#include <QWidget>
#include <QComboBox>
#include <QLabel>
#include <QGridLayout>
#include <QPushButton>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QMessageBox>
#include "utils/StyleLoader.h"
#include "utils/SerialPortSettings.h"
#include <QMetaType>
#include "ui/CMessageBox.h"
#include "core/SerialPortManager.h"
#include "utils/ThreadPoolManager.h"

#ifndef SERIALPORTCONNECTCONFIGWIDGET_H
#define SERIALPORTCONNECTCONFIGWIDGET_H

// 向qt注册自定义类型
Q_DECLARE_METATYPE(QSerialPortInfo)

class SerialPortConnectConfigWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SerialPortConnectConfigWidget(QWidget* parent = nullptr);
    ~SerialPortConnectConfigWidget() = default;

private:
    void setUI();
    void createComponents();
    void componentPropertySettings();
    void createLayout();
    void connectSignals();

private slots:
    void onConnectButtonClicked();
    void detectionAvailablePorts(QList<QSerialPortInfo> ports);

signals:
    void sigPortsDetected(QList<QSerialPortInfo> ports);

private:
    // 下拉框组件
    QComboBox* m_pPortComboBox = nullptr;
    QComboBox* m_pBaudRateComboBox = nullptr;
    QComboBox* m_pDataBitsComboBox = nullptr;
    QComboBox* m_pStopBitsComboBox = nullptr;
    QComboBox* m_pParityComboBox = nullptr;
    QComboBox* m_pFlowControlComboBox; // 流控制下拉框
    // 其他辅助组件
    QLabel* m_pPortLabel = nullptr;
    QLabel* m_pBaudRateLabel = nullptr;
    QLabel* m_pDataBitsLabel = nullptr;
    QLabel* m_pStopBitsLabel = nullptr;
    QLabel* m_pParityLabel = nullptr;
    QLabel* m_pFlowControlLabel; // 流控制标签
    // 按钮
    QPushButton* m_pConnectButton = nullptr; // 连接按钮
    // 布局
    QGridLayout* m_pMainLayout = nullptr;
    // 串口管理器
    SerialPortManager* m_pSerialPortManager = nullptr;
};

#endif //SERIALPORTCONNECTCONFIGWIDGET_H
