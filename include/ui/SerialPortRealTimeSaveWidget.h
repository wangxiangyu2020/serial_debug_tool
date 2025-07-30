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
    explicit SerialPortRealTimeSaveWidget(QWidget* parent = nullptr);
    ~SerialPortRealTimeSaveWidget() = default;

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    void setUI();
    void createComponents();
    void createLayout();
    void connectSignals();

private slots:
    void moveIcon();

signals:
    void sigDisplaySavePath(const QString& path);

private:
    QVBoxLayout* m_pMainLayout = nullptr;
    QVBoxLayout* m_pPanelLayout = nullptr;
    QPlainTextEdit* m_pSavePathDisplayTextEdit = nullptr;
    QWidget* m_pPanel = nullptr;
    // 进度条
    QProgressBar* m_pProgressBar;
    QLabel* m_pIconLabel;
    QTimer* m_pTimer;
    int currentPosition;
    bool movingRight;
    const int iconSize = 20;
    const int moveStep = 5;
};

#endif //SERIALPORTREALTIMESAVEWIDGET_H
