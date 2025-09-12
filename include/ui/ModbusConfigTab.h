/**
  ******************************************************************************
  * @file           : ModbusConfigTab.h
  * @author         : wangxiangyu
  * @brief          : None
  * @attention      : None
  * @date           : 2025/9/11
  ******************************************************************************
  */

#ifndef MODBUSCONFIGTAB_H
#define MODBUSCONFIGTAB_H

#include <QWidget>
#include "utils/StyleLoader.h"
#include <QGroupBox>
#include "ui/ModbusDisplayWidget.h"
#include <QVBoxLayout>

class ModbusConfigTab : public QWidget
{
    Q_OBJECT

public:
    explicit ModbusConfigTab(QWidget* parent = nullptr);
    ~ModbusConfigTab() = default;

private:
    void setUI();
    void createComponents();
    void createLayout();
    void connectSignals();

    QVBoxLayout* m_pMainLayout = nullptr;

    ModbusDisplayWidget* m_pModbusDisplayWidget = nullptr;
};

#endif //MODBUSCONFIGTAB_H
