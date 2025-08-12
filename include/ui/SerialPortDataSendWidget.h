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
#include "ui/SerialPortConnectConfigWidget.h"

class SerialPortDataSendWidget : public QWidget
{
    Q_OBJECT

public:
    // 构造函数和析构函数
    explicit SerialPortDataSendWidget(QWidget* parent = nullptr);
    ~SerialPortDataSendWidget() = default;

    // 静态方法
    static QPlainTextEdit* getSendTextEdit();

protected:
    // 事件处理方法
    void resizeEvent(QResizeEvent* event) override;

private slots:
    void onSendButtonClicked();

private:
    // 私有方法
    void setUI();
    void createComponents();
    void createLayout();
    void connectSignals();

    // UI组件成员
    QPlainTextEdit* m_pSendTextEdit = nullptr;
    QPushButton* m_pSendButton = nullptr;

    // 布局成员
    QHBoxLayout* m_pMainLayout = nullptr;
};

#endif //SERIALPORTDATASENDWIDGET_H
