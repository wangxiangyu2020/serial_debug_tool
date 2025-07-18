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

#ifndef SERIALPORTCONNECTCONFIGWIDGET_H
#define SERIALPORTCONNECTCONFIGWIDGET_H

class SerialPortConnectConfigWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SerialPortConnectConfigWidget(QWidget* parent = nullptr);
    ~SerialPortConnectConfigWidget() = default;

private:
    void setUI();

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
    QPushButton* m_pConnectButton; // 连接按钮

    QGridLayout* m_pMainLayout = nullptr;
};

#endif //SERIALPORTCONNECTCONFIGWIDGET_H
