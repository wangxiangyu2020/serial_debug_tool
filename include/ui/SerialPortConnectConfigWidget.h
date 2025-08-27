/**
  ******************************************************************************
  * @file           : SerialPortConnectConfigWidget.h
  * @author         : wangxiangyu
  * @brief          : None
  * @attention      : None
  * @date           : 2025/7/17
  ******************************************************************************
  */


#ifndef SERIALPORTCONNECTCONFIGWIDGET_H
#define SERIALPORTCONNECTCONFIGWIDGET_H

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
#include <QAbstractItemView>

// 向qt注册自定义类型
Q_DECLARE_METATYPE(QSerialPortInfo)

class SerialPortConnectConfigWidget : public QWidget
{
    Q_OBJECT

public:
    // 构造函数和析构函数
    explicit SerialPortConnectConfigWidget(QWidget* parent = nullptr);
    ~SerialPortConnectConfigWidget() = default;

signals:
    void startConnectionRequested(QMap<QString, QVariant> serialParams);
    void stopConnectionRequested();
    void availablePortsUpdated(const QList<QSerialPortInfo>& availablePorts);

protected:
    // 事件处理方法
    bool eventFilter(QObject* watched, QEvent* event) override;

private slots:
    void onConnectButtonClicked();
    void onStatusChanged(const QString& status, int connectStatus);
    void onUpdatePortComboBox(const QList<QSerialPortInfo>& newPorts);

private:
    // 私有方法
    void setUI();
    void createComponents();
    void componentPropertySettings();
    void createLayout();
    void connectSignals();
    void detectionAvailablePorts();

    // 下拉框组件
    QComboBox* m_pPortComboBox = nullptr;
    QComboBox* m_pBaudRateComboBox = nullptr;
    QComboBox* m_pDataBitsComboBox = nullptr;
    QComboBox* m_pStopBitsComboBox = nullptr;
    QComboBox* m_pParityComboBox = nullptr;
    QComboBox* m_pFlowControlComboBox = nullptr;

    // 标签组件
    QLabel* m_pPortLabel = nullptr;
    QLabel* m_pBaudRateLabel = nullptr;
    QLabel* m_pDataBitsLabel = nullptr;
    QLabel* m_pStopBitsLabel = nullptr;
    QLabel* m_pParityLabel = nullptr;
    QLabel* m_pFlowControlLabel = nullptr;

    // 按钮组件
    QPushButton* m_pConnectButton = nullptr;

    // 布局成员
    QGridLayout* m_pMainLayout = nullptr;

    bool m_isConnected = false;
};

#endif //SERIALPORTCONNECTCONFIGWIDGET_H
