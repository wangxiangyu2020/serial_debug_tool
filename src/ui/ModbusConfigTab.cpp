/**
  ******************************************************************************
  * @file           : ModbusConfigTab.cpp
  * @author         : wangxiangyu
  * @brief          : None
  * @attention      : None
  * @date           : 2025/9/11
  ******************************************************************************
  */

#include "ui/ModbusConfigTab.h"

ModbusConfigTab::ModbusConfigTab(QWidget* parent)
    : QWidget(parent)
{
    this->setUI();
    StyleLoader::loadStyleFromFile(this, ":/resources/qss/modbus_config_tab.qss");
}

void ModbusConfigTab::setUI()
{
    this->setAttribute(Qt::WA_StyledBackground);
    this->createComponents();
    this->createLayout();
    this->connectSignals();
}

void ModbusConfigTab::createComponents()
{
    m_pModbusDisplayWidget = new ModbusDisplayWidget(this);
}

void ModbusConfigTab::createLayout()
{
    m_pMainLayout = new QVBoxLayout(this);
    m_pMainLayout->addWidget(m_pModbusDisplayWidget);
    m_pMainLayout->setContentsMargins(5, 5, 5, 5);
}

void ModbusConfigTab::connectSignals()
{
}
