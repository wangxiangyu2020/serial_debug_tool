/**
  ******************************************************************************
  * @file           : WaveformCtrlWidget.h
  * @author         : wangxiangyu
  * @brief          : None
  * @attention      : None
  * @date           : 2025/7/31
  ******************************************************************************
  */

#ifndef WAVEFORMCTRLWIDGET_H
#define WAVEFORMCTRLWIDGET_H

#include <QWidget>
#include "utils/StyleLoader.h"
#include <QHBoxLayout>
#include <QPushButton>

class WaveformCtrlWidget : public QWidget
{
    Q_OBJECT

public:
    explicit WaveformCtrlWidget(QWidget* parent = nullptr);
    ~WaveformCtrlWidget() = default;

private:
    void setUI();
    void createComponents();
    void createLayout();
    void connectSignals();

private:
    QHBoxLayout* m_pMainLayout = nullptr;
    QPushButton* m_pAddChannelButton = nullptr;
    QPushButton* m_pRemoveChannelButton = nullptr;
    QPushButton* m_pClearButton = nullptr;
    QPushButton* m_pImportButton = nullptr;
    QPushButton* m_pExportButton = nullptr;
    QPushButton* m_pActionButton = nullptr;
};

#endif //WAVEFORMCTRLWIDGET_H
