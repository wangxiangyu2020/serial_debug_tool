/**
  ******************************************************************************
  * @file           : SerialPortRealTimeSaveWidget.h
  * @author         : wangxiangyu
  * @brief          : None
  * @attention      : None
  * @date           : 2025/7/29
  ******************************************************************************
  */


#ifndef SERIALPORTREALTIMESAVEWIDGET_H
#define SERIALPORTREALTIMESAVEWIDGET_H

#include <QWidget>
#include "utils/StyleLoader.h"
#include <QPlainTextEdit>
#include <QVBoxLayout>
#include <QProgressBar>
#include <QTimer>
#include <QLabel>

class SerialPortRealTimeSaveWidget : public QWidget
{
    Q_OBJECT

public:
    // 构造函数和析构函数
    explicit SerialPortRealTimeSaveWidget(QWidget* parent = nullptr);
    ~SerialPortRealTimeSaveWidget() = default;

signals:
    void sigDisplaySavePath(const QString& path);

protected:
    // 事件处理方法
    bool eventFilter(QObject* watched, QEvent* event) override;

private slots:
    void moveIcon();

private:
    // 私有方法
    void setUI();
    void createComponents();
    void createLayout();
    void connectSignals();

    // 静态成员变量
    static constexpr int iconSize = 20;
    static constexpr int moveStep = 5;

    // 布局成员
    QVBoxLayout* m_pMainLayout = nullptr;
    QVBoxLayout* m_pPanelLayout = nullptr;

    // UI组件成员
    QPlainTextEdit* m_pSavePathDisplayTextEdit = nullptr;
    QWidget* m_pPanel = nullptr;
    QProgressBar* m_pProgressBar = nullptr;
    QLabel* m_pIconLabel = nullptr;

    // 定时器对象
    QTimer* m_pTimer = nullptr;

    // 状态变量
    int currentPosition = 0;
    bool movingRight = true;
};

#endif //SERIALPORTREALTIMESAVEWIDGET_H
