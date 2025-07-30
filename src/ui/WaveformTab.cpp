/**
  ******************************************************************************
  * @file           : WaveformTab.cpp
  * @author         : wangxiangyu
  * @brief          : None
  * @attention      : None
  * @date           : 2025/7/17
  ******************************************************************************
  */

#include "ui/WaveformTab.h"

WaveformTab::WaveformTab(QWidget* parent)
    : QWidget(parent), m_pageLoaded(false)
{
    this->setUI();
    StyleLoader::loadStyleFromFile(this, ":resources/qss/wave_form_tab.qss");
}

void WaveformTab::setUI()
{
    this->setAttribute(Qt::WA_StyledBackground);
    m_pLayout = new QVBoxLayout(this);
    m_pWebView = new QWebEngineView(this);

    // 性能优化设置
    auto settings = m_pWebView->settings();
    settings->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
    settings->setAttribute(QWebEngineSettings::Accelerated2dCanvasEnabled, true);
    settings->setAttribute(QWebEngineSettings::WebGLEnabled, true);

    // 监听页面加载完成
    connect(m_pWebView, &QWebEngineView::loadFinished, this, &WaveformTab::onPageLoadFinished);

    // 加载ECharts页面
    m_pWebView->load(QUrl("qrc:/resources/web/wave.html"));
    m_pLayout->addWidget(m_pWebView);
}

void WaveformTab::onPageLoadFinished(bool success)
{
    if (success) {
        m_pageLoaded = true;
        qDebug() << "波形图表页面加载成功";
        
        // 延迟执行，确保JavaScript完全初始化
        QTimer::singleShot(1000, this, [this]() {
            qDebug() << "开始添加示例数据...";
            
            // 添加示例数据
            addSeries("通道1", "#5470c6");
            addSeries("通道2", "#91cc75");
            addSeries("通道3", "#fac858");
            
            // 生成示例数据
            QVariantList sampleData1, sampleData2, sampleData3;
            for (int i = 0; i < 100; i++) {
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
            
            qDebug() << "生成数据完成，通道1数据点数:" << sampleData1.size();
            if (!sampleData1.isEmpty()) {
                QVariantList firstPoint = sampleData1.first().toList();
                qDebug() << "第一个数据点:" << firstPoint;
                qDebug() << "第一个数据点大小:" << firstPoint.size();
                if (firstPoint.size() >= 2) {
                    qDebug() << "时间:" << firstPoint[0].toDouble() << "值:" << firstPoint[1].toDouble();
                }
            }
            
            setSeriesData("通道1", sampleData1);
            setSeriesData("通道2", sampleData2);
            setSeriesData("通道3", sampleData3);
            
            qDebug() << "示例数据设置完成";
        });
        
    } else {
        qDebug() << "波形图表页面加载失败";
    }
}

void WaveformTab::addSeries(const QString& name, const QString& color)
{
    if (!m_pageLoaded) return;

    QString jsCode = QString("addSeries('%1', '%2');").arg(name, color);
    executeJS(jsCode);
}

void WaveformTab::setSeriesData(const QString& seriesName, const QVariantList& data)
{
    if (!m_pageLoaded) return;
    
    qDebug() << "setSeriesData 开始处理:" << seriesName << "数据点数:" << data.size();
    
    QJsonArray jsonArray;
    for (int i = 0; i < data.size(); ++i) {
        const auto& point = data[i];
        qDebug() << "处理数据点" << i << ":" << point << "类型:" << point.typeName();
        
        if (point.canConvert<QVariantList>()) {
            QVariantList pointList = point.toList();
            qDebug() << "转换为列表:" << pointList << "大小:" << pointList.size();
            
            if (pointList.size() >= 2) {
                QJsonArray pointArray;
                pointArray.append(pointList[0].toDouble());
                pointArray.append(pointList[1].toDouble());
                jsonArray.append(pointArray);
                
                if (i < 3) { // 只打印前3个点用于调试
                    qDebug() << "添加点:" << pointList[0].toDouble() << "," << pointList[1].toDouble();
                }
            } else {
                qDebug() << "数据点大小不足:" << pointList.size();
            }
        } else {
            qDebug() << "无法转换为QVariantList:" << point;
        }
    }
    
    QJsonDocument doc(jsonArray);
    QString jsonString = doc.toJson(QJsonDocument::Compact);
    
    qDebug() << "生成的JSON长度:" << jsonString.length();
    qDebug() << "JSON前100字符:" << jsonString.left(100);
    
    QString jsCode = QString("setSeriesData('%1', %2);").arg(seriesName, jsonString);
    executeJS(jsCode);
}

void WaveformTab::addDataPoint(const QString& seriesName, double timestamp, double value)
{
    if (!m_pageLoaded) return;

    QString jsCode = QString("addDataPoint('%1', %2, %3);")
                     .arg(seriesName)
                     .arg(timestamp)
                     .arg(value);
    executeJS(jsCode);
}

void WaveformTab::addDataPoints(const QString& seriesName, const QVariantList& points)
{
    if (!m_pageLoaded) return;

    QJsonArray jsonArray;
    for (const auto& point : points)
    {
        if (point.canConvert<QVariantList>())
        {
            QVariantList pointList = point.toList();
            if (pointList.size() >= 2)
            {
                QJsonArray pointArray;
                pointArray.append(pointList[0].toDouble());
                pointArray.append(pointList[1].toDouble());
                jsonArray.append(pointArray);
            }
        }
    }

    QJsonDocument doc(jsonArray);
    QString jsonString = doc.toJson(QJsonDocument::Compact);

    QString jsCode = QString("addDataPoints('%1', %2);").arg(seriesName, jsonString);
    executeJS(jsCode);
}

void WaveformTab::clearAllData()
{
    if (!m_pageLoaded) return;
    executeJS("clearAllData();");
}

void WaveformTab::clearSeriesData(const QString& seriesName)
{
    if (!m_pageLoaded) return;
    QString jsCode = QString("clearSeriesData('%1');").arg(seriesName);
    executeJS(jsCode);
}

void WaveformTab::removeSeries(const QString& seriesName)
{
    if (!m_pageLoaded) return;
    QString jsCode = QString("removeSeries('%1');").arg(seriesName);
    executeJS(jsCode);
}

void WaveformTab::setChartTitle(const QString& title)
{
    if (!m_pageLoaded) return;
    QString jsCode = QString("setChartTitle('%1');").arg(title);
    executeJS(jsCode);
}

void WaveformTab::refreshChart()
{
    if (!m_pageLoaded) return;
    executeJS("updateChart();");
}

QString WaveformTab::exportData()
{
    if (!m_pageLoaded) return QString();

    // 这里需要同步获取数据，实际项目中可能需要异步处理
    executeJS("exportData();");
    return QString(); // 简化实现
}

bool WaveformTab::importData(const QString& jsonData)
{
    if (!m_pageLoaded) return false;

    QString jsCode = QString("importData('%1');").arg(jsonData);
    executeJS(jsCode);
    return true;
}

void WaveformTab::getDataStats()
{
    if (!m_pageLoaded) return;

    executeJSWithCallback("getDataStats();", [this](const QVariant& result)
    {
        onStatsReceived(result);
    });
}

void WaveformTab::onStatsReceived(const QVariant& result)
{
    qDebug() << "数据统计信息:" << result;
}

void WaveformTab::executeJS(const QString& jsCode)
{
    if (m_pageLoaded) {
        qDebug() << "执行JavaScript:" << jsCode.left(100);
        m_pWebView->page()->runJavaScript(jsCode, [jsCode](const QVariant& result) {
            if (!result.isNull()) {
                qDebug() << "JavaScript执行结果:" << result;
            }
        });
    } else {
        qDebug() << "页面未加载完成，跳过JavaScript执行";
    }
}

void WaveformTab::executeJSWithCallback(const QString& jsCode, std::function<void(const QVariant&)> callback)
{
    if (m_pageLoaded)
    {
        m_pWebView->page()->runJavaScript(jsCode, callback);
    }
}
