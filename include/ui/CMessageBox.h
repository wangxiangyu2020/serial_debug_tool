/**
  ******************************************************************************
  * @file           : CMessageBox.h
  * @author         : wangxiangyu
  * @brief          : None
  * @attention      : None
  * @date           : 2025/7/19
  ******************************************************************************
  */


#ifndef CMESSAGEBOX_H
#define CMESSAGEBOX_H

#include <QDialog>
#include <QLabel>
#include <QTimer>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QVBoxLayout>
#include <QPainter>
#include <QPainterPath>
#include <QApplication>
#include <QScreen>

class CMessageBox : public QDialog
{
    Q_OBJECT

public:
    // 构造函数和析构函数
    explicit CMessageBox(QWidget* parent = nullptr,
                         const QString& message = "",
                         int displayTime = 2000);
    ~CMessageBox() = default;

    // 主要业务方法
    void showToast();

    // 静态方法
    static void showToast(const QString& message);
    static void showToast(QWidget* parent, const QString& message);

protected:
    // 事件处理方法
    void paintEvent(QPaintEvent* event) override;

private slots:
    void fadeOut();

private:
    // UI组件成员
    QLabel* m_pMessageLabel = nullptr;
    QTimer* m_pTimer = nullptr;
    QPropertyAnimation* m_pFadeAnimation = nullptr;
    QGraphicsOpacityEffect* m_pOpacityEffect = nullptr;
};

#endif //CMESSAGEBOX_H
