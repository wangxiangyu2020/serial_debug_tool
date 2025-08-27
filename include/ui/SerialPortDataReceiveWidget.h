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
#include "ui/SerialPortConnectConfigWidget.h"
#include <QFileDialog>
#include <QTextBlock>


class SerialPortDataReceiveWidget : public QWidget
{
    Q_OBJECT

public:
    // 构造函数和析构函数
    explicit SerialPortDataReceiveWidget(QWidget* parent = nullptr);
    ~SerialPortDataReceiveWidget() = default;

    // 静态方法
    static SerialPortDataReceiveWidget* getSerialPortDataReceiveWidget();
    static QPlainTextEdit* getReceiveTextEdit();

public slots:
    void onClearReceiveData();
    void onSaveReceiveData();

protected:
    // 事件处理方法
    bool eventFilter(QObject* watched, QEvent* event) override;

private slots:
    void onDisplayReceiveData(const QString& data);
    void onDisplaySentDataWithHighlight(const QString& data);

private:
    // 私有方法
    void setUI();
    void createComponents();
    void createLayout();
    void connectSignals();

    // 布局成员
    QVBoxLayout* m_pMainLayout = nullptr;

    // UI组件成员
    QPlainTextEdit* m_pReceiveTextEdit = nullptr;
};

#endif //SERIALPORTDATARECEIVEWIDGET_H
