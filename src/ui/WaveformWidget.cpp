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
    m_pWebEngineView = new QWebEngineView(this);
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
    m_pMainLayout->addWidget(m_pWebEngineView);
    m_pMainLayout->setContentsMargins(0, 0, 0, 0); // 移除所有边距
}

void WaveformWidget::connectSignals()
{
    this->connect(m_pWebEngineView, &QWebEngineView::loadFinished, this, &WaveformWidget::onPageLoadFinished);
}

void WaveformWidget::addSeries(const QString& name, const QString& color)
{
    if (!m_pageLoaded) return;
    QString jsCode = QString("addSeries('%1', '%2')").arg(name, color);
    this->executeJS(jsCode);
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

void WaveformWidget::onPageLoadFinished(bool status)
{
    if (!status) return;
    m_pageLoaded = true;
    QTimer::singleShot(1000, this, [this]
    {
        // 添加通道 示例数据
        this->addSeries("通道1", "#FF0000");
        this->addSeries("通道2", "#00FF00");
        this->addSeries("通道3", "#0000FF");

        QVariantList sampleData1, sampleData2, sampleData3;
        for (int i = 0; i < 100; ++i)
        {
            double time = i * 10.0;

            // 创建数据点 - 确保正确的格式
            QVariantList point1;
            point1.append(time);
            point1.append(std::sin(i * 0.1) * 50 + (rand() % 20 - 10));

            QVariantList point2;
            point2.append(time);
            point2.append(std::cos(i * 0.1) * 40 + (rand() % 15 - 7));

            QVariantList point3;
            point3.append(time);
            point3.append(std::sin(i * 0.15) * 30 + (rand() % 25 - 12));

            sampleData1.append(QVariant(point1));
            sampleData2.append(QVariant(point2));
            sampleData3.append(QVariant(point3));
        }
        this->setSeriesData("通道1", sampleData1);
        this->setSeriesData("通道2", sampleData2);
        this->setSeriesData("通道3", sampleData3);
    });
}
