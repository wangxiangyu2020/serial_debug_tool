/**
  ******************************************************************************
  * @file           : SerialPortDataReceiveWidget.h
  * @author         : wangxiangyu
  * @brief          : None
  * @attention      : None
  * @date           : 2025/7/19
  ******************************************************************************
  */

#include <QWidget>
#include <QPlainTextEdit>
#include <QVBoxLayout>
#include <QCheckBox>

#ifndef SERIALPORTDATARECEIVEWIDGET_H
#define SERIALPORTDATARECEIVEWIDGET_H

class SerialPortDataReceiveWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SerialPortDataReceiveWidget(QWidget* parent = nullptr);
    ~SerialPortDataReceiveWidget() = default;

private:
    void setUI();

private:
    QVBoxLayout* m_pMainLayout = nullptr;
    QPlainTextEdit* m_pReceiveTextEdit = nullptr;
    QCheckBox* m_pAutoScrollCheckbox = nullptr;
};

#endif //SERIALPORTDATARECEIVEWIDGET_H
