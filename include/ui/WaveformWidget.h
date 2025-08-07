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
#include <QFileDialog>

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
    void webEngineViewSettings();
    void executeJS(const QString& jsCode);
    void checkAndScheduleUpdate();
    void checkAndUpdateData();
    void flushPendingJSCommands();
    void clearAllData();

private slots:
    void onPageLoadFinished(bool success);
    void onAddSeries(const QString& name, const QString& color);
    void onChannelDataAdded(const QString& channelId, const QVariant& data);
    void onProcessPendingData();

private:
    static constexpr int MAX_QUEUE_SIZE = 10000;

    struct DataPoint
    {
        QString channelName;
        double timestamp;
        double value;
    };

    QVBoxLayout* m_pMainLayout = nullptr;
    std::unique_ptr<QWebEngineView> m_pWebEngineView;
    bool m_pageLoaded = false;
    bool m_resizePending = false;
    // 图表加载优化相关
    QTimer* m_updateTimer;
    QQueue<DataPoint> m_pendingData;
    mutable QMutex m_dataMutex; // 保护 m_pendingData 的互斥锁
    bool m_updateScheduled = false;
    bool m_isResizing = false;
    QTimer* m_renderTimer = nullptr;
    QTimer* m_updateCheckTimer = nullptr;
    QStringList m_pendingJSCommands; // 缓存被跳过的JS命令
};

#endif //WAVEFORMWIDGET_H
