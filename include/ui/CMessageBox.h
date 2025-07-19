/**
  ******************************************************************************
  * @file           : CMessageBox.h
  * @author         : wangxiangyu
  * @brief          : None
  * @attention      : None
  * @date           : 2025/7/19
  ******************************************************************************
  */

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

#ifndef CMESSAGEBOX_H
#define CMESSAGEBOX_H

class CMessageBox : public QDialog
{
    Q_OBJECT

public:
    explicit CMessageBox(QWidget* parent = nullptr,
                         const QString& message = "",
                         int displayTime = 2000);
    ~CMessageBox() = default;

    void showToast();

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    QLabel* m_pMessageLabel;
    QTimer* m_pTimer;
    QPropertyAnimation* m_pFadeAnimation;
    QGraphicsOpacityEffect* m_pOpacityEffect;

private slots:
    void fadeOut();
};

#endif //CMESSAGEBOX_H
