/**
  ******************************************************************************
  * @file           : WaveformTab.h
  * @author         : wangxiangyu
  * @brief          : None
  * @attention      : None
  * @date           : 2025/7/17
  ******************************************************************************
  */


#ifndef WAVEFORMTAB_H
#define WAVEFORMTAB_H

#include <QWidget>
#include "utils/StyleLoader.h"
#include <QWebEngineView>
#include <QWebEngineSettings>
#include <QVBoxLayout>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QTimer>
#include <QUrl>
#include <cmath>
#include "ui/WaveformWidget.h"
#include "ui/WaveformCtrlWidget.h"

class WaveformTab : public QWidget
{
    Q_OBJECT

public:
    explicit WaveformTab(QWidget* parent = nullptr);
    ~WaveformTab() = default;

private:
    void setUI();

    QVBoxLayout* m_pMainLayout;
    QWidget* m_pPanel = nullptr;
    WaveformWidget* m_pWaveformWidget = nullptr;
    WaveformCtrlWidget* m_pWaveformCtrlWidget = nullptr;
};

#endif // WAVEFORMTAB_H
