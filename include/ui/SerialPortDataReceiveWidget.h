/**
  ******************************************************************************
  * @file           : SerialPortDataReceiveWidget.h
  * @author         : wangxiangyu
  * @brief          : None
  * @attention      : None
  * @date           : 2025/7/19
  ******************************************************************************
  */


#ifndef SERIALPORTDATARECEIVEWIDGET_H
#define SERIALPORTDATARECEIVEWIDGET_H

#include <QWidget>
#include <QPlainTextEdit>
#include <QVBoxLayout>
#include <QCheckBox>
#include "utils/StyleLoader.h"
#include <QScrollBar>
#include "utils/ThreadPoolManager.h"
#include "core/SerialPortManager.h"

class SerialPortDataReceiveWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SerialPortDataReceiveWidget(QWidget* parent = nullptr);
    ~SerialPortDataReceiveWidget() = default;

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    void setUI();
    void createComponents();
    void createLayout();
    void connectSignals();

private slots:
    void showReceiveData(const QByteArray& data);

private:
    QVBoxLayout* m_pMainLayout = nullptr;
    QPlainTextEdit* m_pReceiveTextEdit = nullptr;
    QCheckBox* m_pAutoScrollCheckbox = nullptr;
};

#endif //SERIALPORTDATARECEIVEWIDGET_H
