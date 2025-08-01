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
    m_pWebEngineView->resize(this->size());
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

    m_pWebEngineView->load(QUrl("qrc:/resources/web/wave.html"));
}

void WaveformWidget::createLayout()
{
    m_pMainLayout = new QVBoxLayout(this);
    m_pMainLayout->addWidget(m_pWebEngineView.get());
    m_pMainLayout->setContentsMargins(0, 0, 0, 0); // 移除所有边距
}

void WaveformWidget::connectSignals()
{
    ChannelManager* manager = ChannelManager::getInstance();
    if (!manager) return;
    this->connect(m_pWebEngineView.get(), &QWebEngineView::loadFinished, this, &WaveformWidget::onPageLoadFinished);
    this->connect(manager, &ChannelManager::channelAdded, this, &WaveformWidget::onAddSeries);
}


void WaveformWidget::executeJS(const QString& jsCode)
{
    if (!m_pageLoaded) return;
    m_pWebEngineView->page()->runJavaScript(jsCode, [jsCode](const QVariant& result)
    {
        qDebug() << "JavaScript执行结果:" << result;
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

void WaveformWidget::updateSeriesData(const QString& seriesName, const QVariantList& newData)
{
    if (!m_pageLoaded) return;
    // 先设置数据
    this->setSeriesData(seriesName, newData);
    // 强制刷新图表
    this->executeJS("myChart.resize(); console.log('强制刷新图表');");
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

    // // 只为当前添加的通道生成模拟数据
    // QVariantList simulatedData;
    // for (int i = 0; i < 50; ++i)
    // {
    //     double time = i * 20.0; // 时间间隔20ms
    //     double value = std::sin(i * 0.1) * 50 + (rand() % 20 - 10); // 正弦波 + 随机噪声
    //
    //     QVariantList point;
    //     point.append(time);
    //     point.append(value);
    //     simulatedData.append(QVariant(point));
    // }
    //
    // // 使用延迟更新避免阻塞
    // QTimer::singleShot(100, this, [this, name, simulatedData]()
    // {
    //     this->updateSeriesData(name, simulatedData);
    // });
}
