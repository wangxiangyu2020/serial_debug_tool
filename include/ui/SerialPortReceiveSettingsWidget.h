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
#include "ui/SerialPortDataReceiveWidget.h"
#include "ui/SerialPortConnectConfigWidget.h"

class SerialPortReceiveSettingsWidget : public QWidget
{
    Q_OBJECT

public:
    // 构造函数和析构函数
    explicit SerialPortReceiveSettingsWidget(QWidget* parent = nullptr);
    ~SerialPortReceiveSettingsWidget() = default;

    // 获取方法
    QCheckBox* getSaveToFileCheckBox();

signals:
    void sigSaveToFile(bool status);

private:
    // 私有方法
    void setUI();
    void createComponents();
    void createLayout();
    void connectSignals();

    // UI组件成员
    QLabel* m_pTitleLabel = nullptr;
    QCheckBox* m_pSaveToFileCheckBox = nullptr;
    QCheckBox* m_pDisplayTimestampCheckBox = nullptr;
    QCheckBox* m_pHexDisplayCheckBox = nullptr;
    QPushButton* m_pSaveDataButton = nullptr;
    QPushButton* m_pClearDataButton = nullptr;

    // 布局成员
    QVBoxLayout* m_pMainLayout = nullptr;
    QHBoxLayout* m_pButtonLayout = nullptr;
};

#endif //SERIALPORTRECEIVESETTINGSWIDGET_H
