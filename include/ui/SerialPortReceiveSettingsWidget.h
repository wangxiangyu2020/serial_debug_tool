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
#include <ui/ScriptEditorDialog.h>
#include "core/ScriptManager.h"

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
    void clearDataRequested();
    void saveDataRequested();
    void hexDisplayChanged(bool enabled);
    void timestampDisplayChanged(bool enabled);
    void saveToFileChanged(bool enabled);
    void serialPortScriptSaved(const QString& key, const QString& script);
    void serialPortScriptEnabled(bool enabled);

private slots:
    void onShowScriptEditor();

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
    QCheckBox* m_pScriptReceiveCheckBox = nullptr;
    QPushButton* m_pScriptReceiveButton = nullptr;
    QPushButton* m_pScriptHelpButton = nullptr;
    QPushButton* m_pSaveDataButton = nullptr;
    QPushButton* m_pClearDataButton = nullptr;

    ScriptEditorDialog* m_pScriptEditorDialog = nullptr;

    // 布局成员
    QVBoxLayout* m_pMainLayout = nullptr;
    QHBoxLayout* m_pButtonLayout = nullptr;
};

#endif //SERIALPORTRECEIVESETTINGSWIDGET_H
