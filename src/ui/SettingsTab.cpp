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
    QString html = markdown;

    // 1. 首先处理代码块（避免其他规则影响代码内容）
    html.replace(QRegularExpression("```(\\w+)\\n([\\s\\S]*?)```"),
                 "<pre><code class=\"language-\\1\">\\2</code></pre>");
    html.replace(QRegularExpression("```([\\s\\S]*?)```"), "<pre><code>\\1</code></pre>");

    // 2. 处理行内代码
    html.replace(QRegularExpression("`([^`\n]+)`"), "<code>\\1</code>");

    // 3. 处理标题（从高级到低级）
    html.replace(QRegularExpression("^#### (.+)$", QRegularExpression::MultilineOption), "<h4>\\1</h4>");
    html.replace(QRegularExpression("^### (.+)$", QRegularExpression::MultilineOption), "<h3>\\1</h3>");
    html.replace(QRegularExpression("^## (.+)$", QRegularExpression::MultilineOption), "<h2>\\1</h2>");
    html.replace(QRegularExpression("^# (.+)$", QRegularExpression::MultilineOption), "<h1>\\1</h1>");

    // 4. 处理链接
    html.replace(QRegularExpression("\\[([^\\]]+)\\]\\(([^\\)]+)\\)"), "<a href=\"\\2\">\\1</a>");

    // 5. 处理粗体和斜体
    html.replace(QRegularExpression("\\*\\*([^*\n]+)\\*\\*"), "<strong>\\1</strong>");
    html.replace(QRegularExpression("\\*([^*\n]+)\\*"), "<em>\\1</em>");

    // 6. 处理删除线
    html.replace(QRegularExpression("~~([^~\n]+)~~"), "<del>\\1</del>");

    // 7. 处理水平线
    html.replace(QRegularExpression("^---+$", QRegularExpression::MultilineOption), "<hr>");
    html.replace(QRegularExpression("^\\*\\*\\*+$", QRegularExpression::MultilineOption), "<hr>");

    // 8. 处理引用
    html.replace(QRegularExpression("^> (.+)$", QRegularExpression::MultilineOption), "<blockquote>\\1</blockquote>");

    // 9. 处理列表项（简化处理）
    html.replace(QRegularExpression("^- (.+)$", QRegularExpression::MultilineOption), "<li>\\1</li>");
    html.replace(QRegularExpression("^\\* (.+)$", QRegularExpression::MultilineOption), "<li>\\1</li>");
    html.replace(QRegularExpression("^\\+ (.+)$", QRegularExpression::MultilineOption), "<li>\\1</li>");
    html.replace(QRegularExpression("^\\d+\\. (.+)$", QRegularExpression::MultilineOption), "<li>\\1</li>");

    // 10. 简单包装列表项为ul标签
    QStringList lines = html.split('\n');
    QString result;
    bool inList = false;

    for (const QString& line : lines)
    {
        if (line.contains("<li>"))
        {
            if (!inList)
            {
                result += "<ul>\n";
                inList = true;
            }
            result += line + "\n";
        }
        else
        {
            if (inList)
            {
                result += "</ul>\n";
                inList = false;
            }
            result += line + "\n";
        }
    }
    if (inList)
    {
        result += "</ul>\n";
    }

    // 11. 最后处理换行
    result.replace("\n", "<br>");

    return QString("<html><head><style>"
        "body { font-family: 'Microsoft YaHei UI', sans-serif; padding: 20px; line-height: 1.6; }"
        "h1 { color: #000000; border-bottom: 2px solid #3498db; padding-bottom: 5px; margin-bottom: 10px; font-weight: bold; }"
        "h2 { color: #000000; border-bottom: 1px solid #bdc3c7; padding-bottom: 3px; margin-bottom: 8px; }"
        "h3 { color: #000000; margin-bottom: 6px; }"
        "h4 { color: #000000; margin-bottom: 6px; }"
        "code { background: #f8f9fa; padding: 2px 4px; border-radius: 3px; font-family: 'Consolas', monospace; }"
        "pre { background: #f8f9fa; padding: 15px; border-radius: 5px; overflow-x: auto; border-left: 4px solid #3498db; }"
        "pre code { background: none; padding: 0; }"
        "ul { margin: 10px 0; padding-left: 20px; }"
        "li { margin: 5px 0; }"
        "blockquote { border-left: 4px solid #3498db; margin: 10px 0; padding: 10px 20px; background: #ecf0f1; }"
        "a { color: #3498db; text-decoration: none; }"
        "a:hover { text-decoration: underline; }"
        "hr { border: none; height: 1px; background: #bdc3c7; margin: 20px 0; }"
        "del { color: #7f8c8d; }"
        "</style></head><body>") + result + "</body></html>";
}
