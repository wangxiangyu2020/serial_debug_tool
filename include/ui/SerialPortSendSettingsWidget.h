/**
  ******************************************************************************
  * @file           : SerialPortSendSettingsWidget.h
  * @author         : wangxiangyu
  * @brief          : None
  * @attention      : None
  * @date           : 2025/7/18
  ******************************************************************************
  */

#include <QWidget>
#include <QCheckBox>
#include <QLineEdit>
#include <QDoubleValidator>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>

#ifndef SERIALPORTSENDSETTINGSWIDGET_H
#define SERIALPORTSENDSETTINGSWIDGET_H

class SerialPortSendSettingsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SerialPortSendSettingsWidget(QWidget* parent = nullptr);
    ~SerialPortSendSettingsWidget() = default;

private:
    void setUI();

private:
    // 组件
    QLabel* m_pTitleLabel;
    QCheckBox* m_pHexSendCheckBox;
    QCheckBox* m_pTimedSendCheckBox;
    QCheckBox* m_pShowSendStringCheckBox;

    // 定时发送相关
    QLineEdit* m_pIntervalEdit;
    QLabel* m_pIntervalLabel;

    // 布局
    QVBoxLayout* m_pMainLayout;
    QHBoxLayout* m_pTimedSendLayout;
};

#endif //SERIALPORTSENDSETTINGSWIDGET_H
