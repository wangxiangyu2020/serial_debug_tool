/**
  ******************************************************************************
  * @file           : WaveformWidget.h
  * @author         : wangxiangyu
  * @brief          : None
  * @attention      : None
  * @date           : 2025/7/31
  ******************************************************************************
  */

#ifndef WAVEFORMWIDGET_H
#define WAVEFORMWIDGET_H

#include <Qwidget>
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
#include "core/ChannelManager.h"

class WaveformWidget : public QWidget
{
    Q_OBJECT

public:
    explicit WaveformWidget(QWidget* parent = nullptr);
    ~WaveformWidget() = default;

protected:
    void resizeEvent(QResizeEvent* event) override;

private:
    void setUI();
    void createComponents();
    void createLayout();
    void connectSignals();
    void executeJS(const QString& jsCode);
    void setSeriesData(const QString& seriesName, const QVariantList& data);
    void updateSeriesData(const QString& seriesName, const QVariantList& newData);

private slots:
    void onPageLoadFinished(bool success);
    void onAddSeries(const QString& name, const QString& color);

private:
    QVBoxLayout* m_pMainLayout = nullptr;
    QWebEngineView* m_pWebEngineView = nullptr;
    bool m_pageLoaded = false;
    bool m_resizePending = false;
};

#endif //WAVEFORMWIDGET_H
