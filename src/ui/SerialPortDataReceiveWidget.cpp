/**
  ******************************************************************************
  * @file           : SerialPortDataReceiveWidget.cpp
  * @author         : wangxiangyu
  * @brief          : None
  * @attention      : None
  * @date           : 2025/7/19
  ******************************************************************************
  */

#include "ui/SerialPortDataReceiveWidget.h"
#include "utils/StyleLoader.h"

SerialPortDataReceiveWidget::SerialPortDataReceiveWidget(QWidget* parent)
    : QWidget(parent)
{
    this->setUI();
    StyleLoader::loadStyleFromFile(this, ":/resources/qss/serial_port_data_receive_widget.qss");
}

void SerialPortDataReceiveWidget::setUI()
{
    this->setAttribute(Qt::WA_StyledBackground);

    // 主垂直布局 - 无间距和边距
    m_pMainLayout = new QVBoxLayout(this);
    m_pMainLayout->setSpacing(0);
    m_pMainLayout->setContentsMargins(0, 0, 0, 0);

    // 接收文本编辑框
    m_pReceiveTextEdit = new QPlainTextEdit(this);
    m_pReceiveTextEdit->setReadOnly(true); // 设置为只读
    m_pReceiveTextEdit->setUndoRedoEnabled(false); // 禁用撤销/重做
    m_pReceiveTextEdit->setLineWrapMode(QPlainTextEdit::NoWrap); // 禁用自动换行

    // 使用等宽字体
    QFont fixedFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    fixedFont.setPointSize(10);
    m_pReceiveTextEdit->setFont(fixedFont);

    // 添加到布局 - 文本编辑框占据整个容器
    m_pMainLayout->addWidget(m_pReceiveTextEdit);
}
