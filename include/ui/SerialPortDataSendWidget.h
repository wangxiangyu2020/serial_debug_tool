/**
  ******************************************************************************
  * @file           : SerialPortDataSendWidget.h
  * @author         : wangxiangyu
  * @brief          : None
  * @attention      : None
  * @date           : 2025/7/19
  ******************************************************************************
  */


#ifndef SERIALPORTDATASENDWIDGET_H
#define SERIALPORTDATASENDWIDGET_H

#include <QWidget>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include "utils/StyleLoader.h"
#include <QTimer>
#include "core/SerialPortManager.h"
#include "ui/CMessageBox.h"

class SerialPortDataSendWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SerialPortDataSendWidget(QWidget* parent = nullptr);
    ~SerialPortDataSendWidget() = default;

protected:
    void resizeEvent(QResizeEvent* event) override;

private:
    void setUI();
    void createComponents();
    void createLayout();
    void connectSignals();

private slots:
    void onSendButtonClicked();

private:
    // 组件
    QPlainTextEdit* m_pSendTextEdit = nullptr;
    QPushButton* m_pSendButton = nullptr;
    // 布局
    QHBoxLayout* m_pMainLayout = nullptr;
};

#endif //SERIALPORTDATASENDWIDGET_H
