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

class WaveformTab : public QWidget
{
    Q_OBJECT

public:
    explicit WaveformTab(QWidget *parent = nullptr);
    
    // 数据管理接口
    void addSeries(const QString& name, const QString& color);
    void setSeriesData(const QString& seriesName, const QVariantList& data);
    void addDataPoint(const QString& seriesName, double timestamp, double value);
    void addDataPoints(const QString& seriesName, const QVariantList& points);
    void clearAllData();
    void clearSeriesData(const QString& seriesName);
    void removeSeries(const QString& seriesName);
    
    // 图表控制接口
    void setChartTitle(const QString& title);
    void refreshChart();
    
    // 数据导入导出
    QString exportData();
    bool importData(const QString& jsonData);
    
    // 获取统计信息
    void getDataStats();

private slots:
    void onPageLoadFinished(bool success);
    void onStatsReceived(const QVariant& result);

private:
    void setUI();
    void executeJS(const QString& jsCode);
    void executeJSWithCallback(const QString& jsCode, std::function<void(const QVariant&)> callback);
    
    QVBoxLayout* m_pLayout;
    QWebEngineView* m_pWebView;
    bool m_pageLoaded;
};

#endif // WAVEFORMTAB_H
