/**
  ******************************************************************************
  * @file           : SettingsTab.cpp
  * @author         : wangxiangyu
  * @brief          : None
  * @attention      : None
  * @date           : 2025/7/17
  ******************************************************************************
  */

#include "ui/SettingsTab.h"

SettingsTab::SettingsTab(QWidget* parent)
    : QWidget(parent)
{
    this->setUI();
    StyleLoader::loadStyleFromFile(this, ":/resources/qss/settings_tab.qss");
}

void SettingsTab::setUI()
{
    this->setAttribute(Qt::WA_StyledBackground);
    this->createComponents();
}

void SettingsTab::createComponents()
{
    m_pTabWidget = new QTabWidget(this);
    m_pTabWidget->setTabPosition(QTabWidget::West);

    // 创建README文档查看器 - 使用WebEngineView
    m_pReadmeViewer = new QWebEngineView(this);

    // 加载README.md内容并转换为HTML
    QFile readmeFile(":/README.md");
    if (readmeFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QString markdownContent = QString::fromUtf8(readmeFile.readAll());
        QString htmlContent = convertMarkdownToHtml(markdownContent);
        m_pReadmeViewer->setHtml(htmlContent);
    }

    m_pTabWidget->addTab(m_pReadmeViewer, "README");

    // 创建布局
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(m_pTabWidget);
    layout->setContentsMargins(2, 2, 2, 2);
}

QString SettingsTab::convertMarkdownToHtml(const QString& markdown)
{
    // 简单的 Markdown 到 HTML 转换
    QString html = markdown;

    // 转换标题
    html.replace(QRegularExpression("^### (.+)$", QRegularExpression::MultilineOption), "<h3>\\1</h3>");
    html.replace(QRegularExpression("^## (.+)$", QRegularExpression::MultilineOption), "<h2>\\1</h2>");
    html.replace(QRegularExpression("^# (.+)$", QRegularExpression::MultilineOption), "<h1>\\1</h1>");

    // 转换代码块
    html.replace(QRegularExpression("```([\\s\\S]*?)```"), "<pre><code>\\1</code></pre>");
    html.replace(QRegularExpression("`([^`]+)`"), "<code>\\1</code>");

    // 转换粗体和斜体
    html.replace(QRegularExpression("\\*\\*([^*]+)\\*\\*"), "<strong>\\1</strong>");
    html.replace(QRegularExpression("\\*([^*]+)\\*"), "<em>\\1</em>");

    // 转换换行
    html.replace("\n", "<br>");

    return QString("<html><head><style>"
        "body { font-family: 'Microsoft YaHei UI', sans-serif; padding: 20px; }"
        "h1 { color: #2c3e50; }"
        "h2 { color: #34495e; }"
        "h3 { color: #7f8c8d; }"
        "code { background: #f8f9fa; padding: 2px 4px; border-radius: 3px; }"
        "pre { background: #f8f9fa; padding: 10px; border-radius: 5px; overflow-x: auto; }"
        "</style></head><body>") + html + "</body></html>";
}
