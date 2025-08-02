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
#include <memory>

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
    void addDataPoint(const QString& seriesName, double timestamp, double value);

private slots:
    void onPageLoadFinished(bool success);
    void onAddSeries(const QString& name, const QString& color);
    void onChannelDataAdded(const QString& channelId, const QVariant& data);
    void onProcessPendingData();

private:
    QVBoxLayout* m_pMainLayout = nullptr;
    std::unique_ptr<QWebEngineView> m_pWebEngineView;
    bool m_pageLoaded = false;
    bool m_resizePending = false;

    QTimer* m_updateTimer;
    QHash<QString, QList<QPair<double, double>>> m_pendingData;
    bool m_updateScheduled = false;
};

#endif //WAVEFORMWIDGET_H
