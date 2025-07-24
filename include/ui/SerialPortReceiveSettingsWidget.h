/**
  ******************************************************************************
  * @file           : SerialPortReceiveSettingsWidget.h
  * @author         : wangxiangyu
  * @brief          : None
  * @attention      : None
  * @date           : 2025/7/18
  ******************************************************************************
  */


#ifndef SERIALPORTRECEIVESETTINGSWIDGET_H
#define SERIALPORTRECEIVESETTINGSWIDGET_H

#include <QWidget>
#include <QCheckBox>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "utils/StyleLoader.h"

class SerialPortReceiveSettingsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SerialPortReceiveSettingsWidget(QWidget* parent = nullptr);
    ~SerialPortReceiveSettingsWidget() = default;

private:
    void setUI();

private:
    // 组件
    QLabel* m_pTitleLabel = nullptr;
    QCheckBox* m_pSaveToFileCheckBox = nullptr;
    QCheckBox* m_pHexDisplayCheckBox = nullptr;
    QPushButton* m_pSaveDataButton = nullptr;
    QPushButton* m_pClearDataButton = nullptr;
    // 布局
    QVBoxLayout* m_pMainLayout = nullptr;
    QHBoxLayout* m_pButtonLayout = nullptr;
};

#endif //SERIALPORTRECEIVESETTINGSWIDGET_H
