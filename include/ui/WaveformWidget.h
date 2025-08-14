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
#include "core/ChannelManager.h"
#include <memory>
#include <QFileDialog>

class WaveformWidget : public QWidget
{
    Q_OBJECT

public:
    // 构造函数和析构函数
    explicit WaveformWidget(QWidget* parent = nullptr);
    ~WaveformWidget() = default;

protected:
    // 事件处理方法
    void resizeEvent(QResizeEvent* event) override;

private slots:
    void onPageLoadFinished(bool success);
    void onAddSeries(const QString& name, const QString& color);
    void onChannelDataAdded(const QString& channelId, const QVariant& data);
    void onProcessPendingData();

private:
    // 私有方法
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

    // 静态成员变量
    static constexpr int MAX_QUEUE_SIZE = 10000;

    // 数据结构定义
    struct DataPoint
    {
        QString channelName;
        double timestamp;
        double value;
    };

    // 布局成员
    QVBoxLayout* m_pMainLayout = nullptr;

    // 核心对象成员
    std::unique_ptr<QWebEngineView> m_pWebEngineView;

    // 定时器对象
    QTimer* m_updateTimer = nullptr;
    QTimer* m_renderTimer = nullptr;
    QTimer* m_updateCheckTimer = nullptr;

    // 同步对象
    mutable QMutex m_dataMutex; // 保护 m_pendingData 的互斥锁

    // 状态变量
    bool m_pageLoaded = false;
    bool m_resizePending = false;
    bool m_updateScheduled = false;
    bool m_isResizing = false;

    // 数据队列
    QQueue<DataPoint> m_pendingData;
    QStringList m_pendingJSCommands; // 缓存被跳过的JS命令
};

#endif //WAVEFORMWIDGET_H
