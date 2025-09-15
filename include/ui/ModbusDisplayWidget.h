/**
  ******************************************************************************
  * @file           : ModbusDisplayWidget.h
  * @author         : wangxiangyu
  * @brief          : None
  * @attention      : None
  * @date           : 2025/9/12
  ******************************************************************************
  */

#ifndef MODBUSDISPLAYWIDGET_H
#define MODBUSDISPLAYWIDGET_H

#include <QWidget>
#include "utils/StyleLoader.h"
#include <QGroupBox>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QSpinBox>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QFrame>
#include <QPlainTextEdit>
#include <QTableView>
#include <QHeaderView>
#include <ui/TagManagerDialog.h>


class ModbusDisplayWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ModbusDisplayWidget(QWidget* parent = nullptr);
    ~ModbusDisplayWidget() = default;

private slots:
    void onReadButtonClicked();
    void onWriteButtonClicked();
    void onPollButtonToggled(bool checked);
    void onConfigTagsButtonClicked();
    void onClearLogButtonClicked();

private:
    void setUI();
    void createComponents();
    void createLayout();
    void connectSignals();

    // --- 整体布局 ---
    QVBoxLayout* m_pMainLayout = nullptr;

    // --- 界面分区 ---
    QGroupBox* m_pOperationsGroupBox = nullptr;
    QGridLayout* m_pOperationsLayout = nullptr;
    QGroupBox* m_pControllerGroupBox = nullptr;
    QHBoxLayout* m_pControllerLayout = nullptr;
    QGroupBox* m_pDisplayGroupBox = nullptr;
    QGridLayout* m_pDisplayLayout = nullptr;
    QGroupBox* m_pLogGroupBox = nullptr;
    QVBoxLayout* m_pLogLayout = nullptr;

    // --- "操作区" 内的控件 ---
    QLabel* m_pReadLabel = nullptr;
    QSpinBox* m_pReadSlaveIdSpinBox = nullptr;
    QLabel* m_pReadAddrLabel = nullptr;
    QComboBox* m_pReadAddrComboBox = nullptr;
    QLabel* m_pReadQtyLabel = nullptr;
    QSpinBox* m_pReadQtySpinBox = nullptr;
    QPushButton* m_pReadButton = nullptr;
    QFrame* m_pSeparatorLine = nullptr;
    QLabel* m_pWriteLabel = nullptr;
    QSpinBox* m_pWriteSlaveIdSpinBox = nullptr;
    QLabel* m_pWriteAddrLabel = nullptr;
    QComboBox* m_pWriteAddrComboBox = nullptr;
    QLabel* m_pWriteValueLabel = nullptr;
    QLineEdit* m_pWriteValueLineEdit = nullptr;
    QLabel* m_pWriteFuncCodeLabel = nullptr;
    QComboBox* m_pWriteFuncCodeComboBox = nullptr;
    QLabel* m_pWriteDataTypeLabel = nullptr;
    QComboBox* m_pWriteDataTypeComboBox = nullptr;
    QPushButton* m_pWriteButton = nullptr;

    // --- "控制与配置区" 内的控件 ---
    QPushButton* m_pPollButton = nullptr;
    QLabel* m_pPollIntervalLabel = nullptr;
    QSpinBox* m_pPollIntervalSpinBox = nullptr;
    QPushButton* m_pConfigTagsButton = nullptr;

    // --- "数据显示区" 内的控件 ---
    QTableView* m_pDisplayTableView = nullptr;

    // --- "日志区" 内的控件 ---
    QPlainTextEdit* m_pLogTextEdit = nullptr;
    QPushButton* m_pClearLogButton = nullptr;

    QList<ModbusTag> m_modbusTags;
};

#endif //MODBUSDISPLAYWIDGET_H
