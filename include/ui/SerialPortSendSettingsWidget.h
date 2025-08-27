/**
  ******************************************************************************
  * @file           : SerialPortSendSettingsWidget.h
  * @author         : wangxiangyu
  * @brief          : None
  * @attention      : None
  * @date           : 2025/7/18
  ******************************************************************************
  */


#ifndef SERIALPORTSENDSETTINGSWIDGET_H
#define SERIALPORTSENDSETTINGSWIDGET_H

#include <QWidget>
#include <QCheckBox>
#include <QLineEdit>
#include <QDoubleValidator>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "utils/StyleLoader.h"
#include "ui/SerialPortConnectConfigWidget.h"
#include "ui/SerialPortDataSendWidget.h"

class SerialPortSendSettingsWidget : public QWidget
{
    Q_OBJECT

public:
    // 构造函数和析构函数
    explicit SerialPortSendSettingsWidget(QWidget* parent = nullptr);
    ~SerialPortSendSettingsWidget() = default;

    // 静态方法
    static QCheckBox* getTimedSendCheckBox();

signals:
    void hexSendChanged(bool status);
    void showSendStringChanged(bool status);
    void startTimedSendRequested(double interval, const QString& data);
    void stopTimedSendRequested();

private slots:
    void onTimedSendCheckBoxClicked(bool status);

private:
    // 私有方法
    void setUI();
    void createComponents();
    void createLayout();
    void connectSignals();

    // UI组件成员
    QLabel* m_pTitleLabel = nullptr;
    QCheckBox* m_pHexSendCheckBox = nullptr;
    QCheckBox* m_pTimedSendCheckBox = nullptr;
    QCheckBox* m_pShowSendStringCheckBox = nullptr;

    // 定时发送相关成员
    QLineEdit* m_pIntervalEdit = nullptr;
    QLabel* m_pIntervalLabel = nullptr;

    // 布局成员
    QVBoxLayout* m_pMainLayout = nullptr;
    QHBoxLayout* m_pTimedSendLayout = nullptr;
};

#endif //SERIALPORTSENDSETTINGSWIDGET_H
