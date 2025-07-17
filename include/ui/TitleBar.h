/**
  ******************************************************************************
  * @file           : TitleBar.h
  * @author         : wangxiangyu
  * @brief          : None
  * @attention      : None
  * @date           : 2025/7/16
  ******************************************************************************
  */

#include <QWidget>
#include <QPushButton>

#ifndef TITLEBAR_H
#define TITLEBAR_H

class TitleBar : public QWidget
{
    Q_OBJECT

public:
    explicit TitleBar(QWidget* parent = nullptr);
    ~TitleBar() = default;

private:
    void setUI();
    void mousePressEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;

private slots :
    void onClicked();

signals:
    void sigClosed();

private:
    QPushButton* m_pLogoBtn = nullptr;
    QPushButton* m_pMinBtn = nullptr;
    QPushButton* m_pMaxBtn = nullptr;
    QPushButton* m_pCloseBtn = nullptr;
};

#endif //TITLEBAR_H
