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

WaveformWidget::WaveformWidget(QWidget* parent)
    : QWidget(parent)
{
    this->setUI();
}

void WaveformWidget::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    // 立即更新Web视图尺寸
    // m_pWebEngineView->resize(this->size());
    if (m_pageLoaded && !m_resizePending)
    {
        m_resizePending = true;
        // 使用短延迟确保布局完成
        QTimer::singleShot(20, this, [this]()
        {
            if (m_pageLoaded)
            {
                // 触发JavaScript中的尺寸检查
                this->executeJS("if (typeof checkSizeChange === 'function') checkSizeChange();");
            }
            m_resizePending = false;
        });
    }
}

void WaveformWidget::setUI()
{
    this->setAttribute(Qt::WA_StyledBackground);
    this->createComponents();
    this->createLayout();
    this->connectSignals();
}

void WaveformWidget::createComponents()
{
    m_updateTimer = new QTimer(this);
    m_updateTimer->setSingleShot(true);

    m_pWebEngineView = std::make_unique<QWebEngineView>(this);
    // 启用透明背景
    QPalette pal = m_pWebEngineView->palette();
    pal.setColor(QPalette::Window, Qt::transparent);
    m_pWebEngineView->setPalette(pal);
    // 设置背景透明
    m_pWebEngineView->setAttribute(Qt::WA_TranslucentBackground);
    m_pWebEngineView->page()->setBackgroundColor(Qt::transparent);
    // 性能优化设置
    auto settings = m_pWebEngineView->settings();
    settings->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
    settings->setAttribute(QWebEngineSettings::Accelerated2dCanvasEnabled, true);
    settings->setAttribute(QWebEngineSettings::WebGLEnabled, true);
    settings->setAttribute(QWebEngineSettings::ScrollAnimatorEnabled, false);
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
    ChannelManager* manager = ChannelManager::getInstance();
    if (!manager) return;
    this->connect(m_pWebEngineView.get(), &QWebEngineView::loadFinished, this, &WaveformWidget::onPageLoadFinished);
    this->connect(manager, &ChannelManager::channelAdded, this, &WaveformWidget::onAddSeries);
    this->connect(m_updateTimer, &QTimer::timeout, this, &WaveformWidget::onProcessPendingData);
    // 连接数据更新信号
    this->connect(manager, &ChannelManager::channelDataAdded, this, &WaveformWidget::onChannelDataAdded);
}


void WaveformWidget::executeJS(const QString& jsCode)
{
    if (!m_pageLoaded) return;
    m_pWebEngineView->page()->runJavaScript(jsCode, [jsCode](const QVariant& result)
    {
        // qDebug() << "JavaScript执行结果:" << result;
    });
}

void WaveformWidget::setSeriesData(const QString& seriesName, const QVariantList& data)
{
    if (!m_pageLoaded) return;
    QJsonArray jsonArray;
    for (int i = 0; i < data.size(); ++i)
    {
        const auto& point = data[i];
        if (!point.canConvert<QVariantList>()) continue;
        auto pointList = point.toList();
        if (pointList.size() >= 2)
        {
            QJsonArray pointArray;
            pointArray.append(pointList[0].toDouble());
            pointArray.append(pointList[1].toDouble());
            jsonArray.append(pointArray);
        }
    }
    QJsonDocument doc(jsonArray);
    auto jsonString = doc.toJson(QJsonDocument::Compact);
    QString jsCode = QString("setSeriesData('%1', %2);").arg(seriesName, jsonString);
    this->executeJS(jsCode);
}

void WaveformWidget::onPageLoadFinished(bool status)
{
    if (!status) return;
    m_pageLoaded = true;
}

void WaveformWidget::onAddSeries(const QString& name, const QString& color)
{
    if (!m_pageLoaded) return;
    QString jsCode = QString("addSeries('%1', '%2')").arg(name, StyleLoader::getColorHex(color));
    this->executeJS(jsCode);
}

// 实现槽函数
void WaveformWidget::onChannelDataAdded(const QString& channelId, const QVariant& data)
{
    if (!m_pageLoaded) return;

    ChannelManager* manager = ChannelManager::getInstance();
    ChannelInfo channel = manager->getChannel(channelId);

    if (data.canConvert<QVariantList>())
    {
        QVariantList pointList = data.toList();
        if (pointList.size() >= 2)
        {
            // 将数据添加到待处理队列
            m_pendingData[channel.name].append(qMakePair(pointList[0].toDouble(), pointList[1].toDouble()));

            // 如果没有计划更新，则安排一次更新
            if (!m_updateScheduled)
            {
                m_updateScheduled = true;
                m_updateTimer->start(16); // 约60FPS
            }
        }
    }
}

// 新增处理待处理数据的函数
void WaveformWidget::onProcessPendingData()
{
    if (!m_pageLoaded || m_pendingData.isEmpty())
    {
        m_updateScheduled = false;
        return;
    }

    // 使用JSON格式批量传递数据
    QJsonObject seriesDataObject;
    for (auto it = m_pendingData.begin(); it != m_pendingData.end(); ++it)
    {
        const QString& seriesName = it.key();
        const QList<QPair<double, double>>& dataPoints = it.value();

        QJsonArray pointsArray;
        for (const auto& point : dataPoints)
        {
            QJsonArray p;
            p.append(point.first);
            p.append(point.second);
            pointsArray.append(p);
        }
        seriesDataObject[seriesName] = pointsArray;
    }

    QJsonDocument doc(seriesDataObject);
    QString jsonStr = QString::fromUtf8(doc.toJson(QJsonDocument::Compact));

    // 单次JavaScript调用传递所有数据
    QString jsCode = QString("batchAddDataPoints(%1);").arg(jsonStr);
    this->executeJS(jsCode);

    m_pendingData.clear();
    m_updateScheduled = false;
}
