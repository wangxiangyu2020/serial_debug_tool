/**
  ******************************************************************************
  * @file           : CDialogBase.h
  * @author         : wangxiangyu
  * @brief          : None
  * @attention      : None
  * @date           : 2025/7/31
  ******************************************************************************
  */

#ifndef CDIALOGBASE_H
#define CDIALOGBASE_H

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QPainter>
#include <QPainterPath>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QPaintEvent>
#include "utils/StyleLoader.h"

class CDialogBase : public QDialog
{
    Q_OBJECT

public:
    explicit CDialogBase(QWidget* parent = nullptr,
                         const QString& title = "",
                         const QSize& size = QSize(320, 200));
    virtual ~CDialogBase() = default;

protected:
    void paintEvent(QPaintEvent* event) override;
    // 子类可重写的虚函数
    virtual void createComponents() = 0;
    virtual void createContentLayout() = 0;
    virtual void connectSignals() = 0;
    virtual void onConfirmClicked();
    virtual void onCancelClicked();
    // 提供给子类使用的布局和组件
    QVBoxLayout* m_pMainLayout = nullptr;
    QVBoxLayout* m_pContentLayout = nullptr;
    QHBoxLayout* m_pButtonLayout = nullptr;
    QLabel* m_pTitleLabel = nullptr;
    QPushButton* m_pConfirmButton = nullptr;
    QPushButton* m_pCancelButton = nullptr;

private:
    void setUI();
    void createBaseComponents();
    void createBaseLayout();
    void connectBaseSignals();
    void setupAnimation();

private:
    QGraphicsOpacityEffect* m_pOpacityEffect = nullptr;
    QPropertyAnimation* m_pFadeAnimation = nullptr;
};

#endif //CDIALOGBASE_H
