/**
  ******************************************************************************
  * @file           : WaveformWidget.cpp
  * @author         : wangxiangyu
  * @brief          : None
  * @attention      : None
  * @date           : 2025/7/31
  ******************************************************************************
  */

#include "ui/WaveformWidget.h"

// 构造函数和析构函数
WaveformWidget::WaveformWidget(QWidget* parent)
    : QWidget(parent)
{
    this->setUI();
}

// 事件处理方法
void WaveformWidget::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);

    if (!m_pageLoaded) return;

    m_isResizing = true;

    // 暂停数据更新以提升resize性能
    if (m_updateTimer->isActive()) m_updateTimer->stop();

    // 延迟恢复数据更新
    QTimer::singleShot(200, this, [this]()
    {
        m_isResizing = false;
        // 执行缓存的JS命令
        this->flushPendingJSCommands();
        // 强制检查并安排更新，确保图表恢复更新
        {
            QMutexLocker locker(&m_dataMutex);
            if (!m_pendingData.isEmpty())
            {
                m_updateScheduled = true;
                m_updateTimer->start(16);
            }
            else
            {
                if (m_updateTimer->isActive()) m_updateTimer->stop();
            }
        }
    });

    // 使用防抖处理resize
    if (m_renderTimer)
    {
        m_renderTimer->start(100); // 100ms防抖
        return;
    }

    m_renderTimer = new QTimer(this);
    m_renderTimer->setSingleShot(true);
    this->connect(m_renderTimer, &QTimer::timeout, this, [this]()
    {
        if (m_pageLoaded) this->executeJS("if (typeof handleResize === 'function') handleResize();");
    });

    m_renderTimer->start(100); // 100ms防抖
}

// private slots
void WaveformWidget::onPageLoadFinished(bool status)
{
    if (!status) return;
    m_pageLoaded = true;
}

void WaveformWidget::onChannelAdded(const QString& name, const QString& color)
{
    if (!m_pageLoaded) return;
    QString jsCode = QString("addSeries('%1', '%2')").arg(name, StyleLoader::getColorHex(color));
    this->executeJS(jsCode);
}

void WaveformWidget::onChannelRemoved(const QString& name)
{
    if (!m_pageLoaded) return;
    QString jsCode = QString("removeSeries('%1')").arg(name);
    this->executeJS(jsCode);
}

void WaveformWidget::onWaveformDataAdded(const QString& channelName, const QVariant& data)
{
    if (!m_pageLoaded) return;
    if (!data.canConvert<QVariantList>()) return;
    QVariantList pointList = data.toList();
    if (pointList.size() < 2) return;
    // 使用互斥锁保护数据访问
    {
        QMutexLocker locker(&m_dataMutex);
        // 将数据添加到待处理队列
        DataPoint point;
        point.channelName = channelName;
        point.timestamp = pointList[0].toDouble();
        point.value = pointList[1].toDouble();
        m_pendingData.enqueue(point);
        if (m_pendingData.size() >= MAX_QUEUE_SIZE) m_pendingData.dequeue();
        // 如果没有计划更新，则安排一次更新
        if (m_updateScheduled) return;
        m_updateScheduled = true;
        // 只有不在 resize 过程中才启动定时器
        if (!m_isResizing) m_updateTimer->start(16); // 约60FPS
        // 如果在 resize 过程中，数据会被保留，等待 resize 结束后处理
    }
}

void WaveformWidget::onProcessPendingData()
{
    // 在resize过程中暂时不更新图表
    if (m_isResizing)
    {
        // 只有在有待处理数据时才重新调度
        QMutexLocker locker(&m_dataMutex);
        if (!m_pendingData.isEmpty() && !m_updateTimer->isActive()) m_updateTimer->start(50);
        return;
    }

    // 批量处理队列中的数据
    QJsonObject seriesDataObject;
    {
        QMutexLocker locker(&m_dataMutex);
        if (!m_pageLoaded || m_pendingData.isEmpty())
        {
            m_updateScheduled = false;
            if (m_updateTimer->isActive()) m_updateTimer->stop();
            return;
        }

        // 按通道名分组处理数据
        QHash<QString, QJsonArray> channelData;

        // 批量处理，每次最多处理1000个数据点
        const int BATCH_SIZE = 1000;
        int processed = 0;

        while (!m_pendingData.isEmpty() && processed < BATCH_SIZE)
        {
            DataPoint point = m_pendingData.dequeue();

            QJsonArray pointArray;
            pointArray.append(point.timestamp);
            pointArray.append(point.value);

            channelData[point.channelName].append(pointArray);
            processed++;
        }

        // 转换为最终的JSON对象
        for (auto it = channelData.begin(); it != channelData.end(); ++it)
        {
            seriesDataObject[it.key()] = it.value();
        }

        // 如果队列还有数据，继续调度处理
        if (!m_pendingData.isEmpty())
        {
            m_updateTimer->start(16);
        }
        else
        {
            m_updateScheduled = false;
        }
    }

    // 发送数据到JavaScript
    if (!seriesDataObject.isEmpty())
    {
        QJsonDocument doc(seriesDataObject);
        QString jsonStr = QString::fromUtf8(doc.toJson(QJsonDocument::Compact));
        QString jsCode = QString("batchAddDataPoints(%1);").arg(jsonStr);
        this->executeJS(jsCode);
    }
}

void WaveformWidget::onChannelsDataAllCleared()
{
    m_pendingData.clear();
    this->executeJS("clearAllData()");
}

void WaveformWidget::onChannelsDataImported()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("导入数据"), QString(), tr("JSON文件(*.json)"));
    if (fileName.isEmpty()) return;
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return;
    QTextStream in(&file);
    in.setEncoding(QStringConverter::Utf8);
    QString jsonData = in.readAll();
    file.close();
    if (jsonData.isEmpty()) return;
    // 验证JSON格式
    QJsonParseError parseError;
    QJsonDocument::fromJson(jsonData.toUtf8(), &parseError);
    if (parseError.error != QJsonParseError::NoError) return;
    // 先设置全局变量，再调用导入函数
    QString jsCode = QString("window.importJsonData = %1; importData(window.importJsonData);").arg(jsonData);
    this->executeJS(jsCode);
}

void WaveformWidget::onChannelsDataExported()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("导出数据"), QDir::homePath(), tr("JSON文件(*.json)"));
    if (fileName.isEmpty()) return;
    if (!fileName.endsWith(".json", Qt::CaseInsensitive)) fileName += ".json";
    m_pWebEngineView->page()->runJavaScript(QString("exportData()"), [this, fileName](const QVariant& result)
    {
        QString jsonData = result.toString();
        QFile file(fileName);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return;
        QTextStream out(&file);
        out.setEncoding(QStringConverter::Utf8);
        out << jsonData;
        file.close();
    });
}

// 私有方法
void WaveformWidget::setUI()
{
    this->setAttribute(Qt::WA_StyledBackground);
    this->createComponents();
    this->createLayout();
    this->connectSignals();
}

void WaveformWidget::createComponents()
{
    // 初始化更新检查定时器
    m_updateCheckTimer = new QTimer(this);
    m_updateCheckTimer->setInterval(500); // 500ms检查一次
    m_updateTimer = new QTimer(this);
    m_updateTimer->setSingleShot(true);
    m_pWebEngineView = std::make_unique<QWebEngineView>(this);
    this->webEngineViewSettings();
    // 启用透明背景
    QPalette pal = m_pWebEngineView->palette();
    pal.setColor(QPalette::Window, Qt::transparent);
    m_pWebEngineView->setPalette(pal);
    // 设置背景透明
    m_pWebEngineView->setAttribute(Qt::WA_TranslucentBackground);
    m_pWebEngineView->page()->setBackgroundColor(Qt::transparent);
    // 加载html文件
    QFile htmlFile(":/resources/web/wave.html");
    htmlFile.open(QIODevice::ReadOnly);
    m_pWebEngineView->setHtml(htmlFile.readAll());
}

void WaveformWidget::createLayout()
{
    m_pMainLayout = new QVBoxLayout(this);
    m_pMainLayout->addWidget(m_pWebEngineView.get());
    m_pMainLayout->setContentsMargins(0, 0, 0, 5); // 移除所有边距
}

void WaveformWidget::connectSignals()
{
    this->connect(m_updateCheckTimer, &QTimer::timeout, this, &WaveformWidget::checkAndUpdateData);
    this->connect(m_pWebEngineView.get(), &QWebEngineView::loadFinished, this, &WaveformWidget::onPageLoadFinished);
    this->connect(m_updateTimer, &QTimer::timeout, this, &WaveformWidget::onProcessPendingData);
    m_updateCheckTimer->start();

    this->connect(ChannelManager::getInstance(), &ChannelManager::channelAddedRequested, this,
                  &WaveformWidget::onChannelAdded,
                  Qt::QueuedConnection);
    this->connect(ChannelManager::getInstance(), &ChannelManager::channelRemovedRequested, this,
                  &WaveformWidget::onChannelRemoved,
                  Qt::QueuedConnection);
    // 连接数据更新信号
    this->connect(PacketProcessor::getInstance(), &PacketProcessor::waveformDataReady, this,
                  &WaveformWidget::onWaveformDataAdded, Qt::QueuedConnection);
    this->connect(ChannelManager::getInstance(), &ChannelManager::channelsDataAllClearedRequested, this,
                  &WaveformWidget::onChannelsDataAllCleared,
                  Qt::QueuedConnection);
    this->connect(ChannelManager::getInstance(), &ChannelManager::channelsDataImportedRequested, this,
                  &WaveformWidget::onChannelsDataImported, Qt::QueuedConnection);
    this->connect(ChannelManager::getInstance(), &ChannelManager::channelsDataExportedRequested, this,
                  &WaveformWidget::onChannelsDataExported, Qt::QueuedConnection);
}

void WaveformWidget::webEngineViewSettings()
{
    // 性能优化设置
    auto settings = m_pWebEngineView->settings();
    // 启用JavaScript支持，确保网页能够正常执行脚本
    settings->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
    // 启用2D画布加速渲染，提升图形绘制性能
    settings->setAttribute(QWebEngineSettings::Accelerated2dCanvasEnabled, true);
    // 关闭WebGL支持，不允许网页使用3D图形渲染功能
    settings->setAttribute(QWebEngineSettings::WebGLEnabled, false);
    // 禁用滚动动画效果，提升滚动操作的响应速度
    settings->setAttribute(QWebEngineSettings::ScrollAnimatorEnabled, false);
    // 禁止本地内容访问远程URL，增强安全性和性能
    settings->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, false);
    // 禁用空间导航功能，减少不必要的键盘导航处理
    settings->setAttribute(QWebEngineSettings::SpatialNavigationEnabled, false);
    // 禁用超链接审计功能，避免发送额外的网络请求
    settings->setAttribute(QWebEngineSettings::HyperlinkAuditingEnabled, false);
    // 禁用全屏支持功能，减少资源占用
    settings->setAttribute(QWebEngineSettings::FullScreenSupportEnabled, false);
    // 禁用插件支持，提升安全性和性能
    settings->setAttribute(QWebEngineSettings::PluginsEnabled, false);
    // 保持图像自动加载启用状态，确保图表等图片内容正常显示
    settings->setAttribute(QWebEngineSettings::AutoLoadImages, true);
}

void WaveformWidget::executeJS(const QString& jsCode)
{
    if (!m_pageLoaded) return;

    // 在resize过程中限制JS执行频率
    static qint64 lastExecTime = 0;
    qint64 currentTime = QDateTime::currentMSecsSinceEpoch();

    if (m_isResizing && (currentTime - lastExecTime) < 50)
    {
        // 在resize过程中缓存高频JS调用，而不是直接丢弃
        m_pendingJSCommands.append(jsCode);
        return;
    }

    lastExecTime = currentTime;

    m_pWebEngineView->page()->runJavaScript(jsCode, [jsCode](const QVariant& result)
    {
        // qDebug() << "JavaScript执行结果:" << result;
    });
}

void WaveformWidget::checkAndUpdateData()
{
    QMutexLocker locker(&m_dataMutex);
    if (!m_isResizing && !m_updateScheduled && !m_pendingData.isEmpty() && m_pageLoaded)
    {
        m_updateScheduled = true;
        m_updateTimer->start(16);
    }
    else if (m_pendingData.isEmpty() && m_updateTimer->isActive())
    {
        m_updateTimer->stop();
    }
}

void WaveformWidget::flushPendingJSCommands()
{
    if (!m_pageLoaded || m_pendingJSCommands.isEmpty()) return;

    // 执行所有缓存的JS命令
    for (const QString& jsCode : m_pendingJSCommands)
    {
        m_pWebEngineView->page()->runJavaScript(jsCode, [](const QVariant& result)
        {
            // qDebug() << "缓存的JavaScript执行结果:" << result;
        });
    }

    m_pendingJSCommands.clear();
}
