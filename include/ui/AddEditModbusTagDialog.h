/**
******************************************************************************
  * @file           : AddEditModbusTagDialog.h
  * @author         : wangxiangyu
  * @brief          : 添加或编辑Modbus点位的对话框
  * @attention      : None
  * @date           : 2025/9/13
  ******************************************************************************
  */

#ifndef ADDEDITMODBUSTAGDIALOG_H
#define ADDEDITMODBUSTAGDIALOG_H

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QPushButton>
#include <QDialogButtonBox>
#include "utils/ModbusTag.h"
#include <QFormLayout>

class AddEditModbusTagDialog : public QDialog
{
    Q_OBJECT

public:
    // 构造函数接收一个要修改的ModbusTag对象的引用
    explicit AddEditModbusTagDialog(ModbusTag& tag, QWidget *parent = nullptr);
    ~AddEditModbusTagDialog() = default;

private slots:
    void onSaveButtonClicked();
    void onDataTypeChanged(int index);

private:
    void setUI();
    void createComponents();
    void createLayout();
    void connectSignals();

    void loadTagData(); // 将tag数据加载到UI
    void saveTagData(); // 将UI数据保存回tag

    ModbusTag& m_tag; // 对外部要修改的tag对象的引用

    // --- UI 控件 ---
    QFormLayout* m_pFormLayout = nullptr;
    QLineEdit* m_pNameLineEdit = nullptr;
    QSpinBox* m_pSlaveIdSpinBox = nullptr;
    QComboBox* m_pFunctionCodeComboBox = nullptr;
    QSpinBox* m_pAddressSpinBox = nullptr;
    QComboBox* m_pDataTypeComboBox = nullptr;
    QComboBox* m_pByteOrderComboBox = nullptr;
    QDoubleSpinBox* m_pGainSpinBox = nullptr;
    QDoubleSpinBox* m_pOffsetSpinBox = nullptr;
    QPushButton* m_pSaveButton = nullptr;
    QPushButton* m_pCancelButton = nullptr;
};

#endif // ADDEDITMODBUSTAGDIALOG_H