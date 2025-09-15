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


// 构造函数和析构函数
SettingsTab::SettingsTab(QWidget* parent)
    : QWidget(parent)
{
    this->setUI();
    StyleLoader::loadStyleFromFile(this, ":/resources/qss/settings_tab.qss");
}

// 事件处理方法
void SettingsTab::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);

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
        // 先尝试触发 resize 事件
        m_pReadmeViewer->page()->runJavaScript("window.dispatchEvent(new Event('resize'));");

        // 延迟检查内容是否正确显示，如果没有则重新加载
        QTimer::singleShot(50, [this]()
        {
            m_pReadmeViewer->page()->runJavaScript("document.body.innerHTML", [this](const QVariant& result)
            {
                if (result.toString().isEmpty())
                {
                    // 如果内容为空，则重新加载
                    QFile readmeFile(":/README.md");
                    if (readmeFile.open(QIODevice::ReadOnly | QIODevice::Text))
                    {
                        QString markdownContent = QString::fromUtf8(readmeFile.readAll());
                        QString htmlContent = convertMarkdownToHtml(markdownContent);
                        m_pReadmeViewer->setHtml(htmlContent);
                    }
                }
            });
        });
    });

    m_renderTimer->start(100); // 100ms防抖
}

// 私有方法
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

    // 启用透明背景
    QPalette pal = m_pReadmeViewer->palette();
    pal.setColor(QPalette::Window, Qt::transparent);
    m_pReadmeViewer->setPalette(pal);
    // 设置背景透明
    m_pReadmeViewer->setAttribute(Qt::WA_TranslucentBackground);
    m_pReadmeViewer->page()->setBackgroundColor(Qt::transparent);

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
    html.replace(QRegularExpression("`([^`\\n]+)`"), "<code>\\1</code>");

    // 3. 处理标题（从高级到低级）
    html.replace(QRegularExpression("^#### (.+)$", QRegularExpression::MultilineOption), "<h4>\\1</h4>");
    html.replace(QRegularExpression("^### (.+)$", QRegularExpression::MultilineOption), "<h3>\\1</h3>");
    html.replace(QRegularExpression("^## (.+)$", QRegularExpression::MultilineOption), "<h2>\\1</h2>");
    html.replace(QRegularExpression("^# (.+)$", QRegularExpression::MultilineOption), "<h1>\\1</h1>");

    // 4. 处理链接
    html.replace(QRegularExpression("\\[([^\\]]+)\\]\\(([^\\)]+)\\)"), "<a href=\"\\2\">\\1</a>");

    // 5. 处理粗体和斜体
    html.replace(QRegularExpression("\\*\\*([^*\\n]+)\\*\\*"), "<strong>\\1</strong>");
    html.replace(QRegularExpression("\\*([^*\\n]+)\\*"), "<em>\\1</em>");

    // 6. 处理删除线
    html.replace(QRegularExpression("~~([^~\\n]+)~~"), "<del>\\1</del>");

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
        "body { font-family: 'Microsoft YaHei UI', sans-serif; padding: 20px; line-height: 1.8; color: #333; }"
        "h1 { color: #2c3e50; border-bottom: 2px solid #3498db; padding-bottom: 8px; margin-bottom: 16px; font-weight: 600; }"
        "h2 { color: #2c3e50; border-bottom: 1px solid #bdc3c7; padding-bottom: 6px; margin-bottom: 12px; font-weight: 600; }"
        "h3 { color: #2c3e50; margin-bottom: 10px; font-weight: 600; }"
        "h4 { color: #2c3e50; margin-bottom: 8px; font-weight: 600; }"
        "code { background: #f8f9fa; padding: 3px 6px; border-radius: 4px; font-family: 'Consolas', 'Monaco', monospace; font-size: 90%; color: #e74c3c; }"
        "pre { background: #f8f9fa; padding: 16px; border-radius: 6px; overflow-x: auto; border-left: 4px solid #3498db; margin: 16px 0; }"
        "pre code { background: none; padding: 0; color: #2c3e50; font-size: 14px; }"
        "ul { margin: 12px 0; padding-left: 24px; }"
        "li { margin: 6px 0; line-height: 1.6; }"
        "blockquote { border-left: 4px solid #3498db; margin: 16px 0; padding: 12px 20px; background: #ecf0f1; border-radius: 0 4px 4px 0; }"
        "a { color: #3498db; text-decoration: none; font-weight: 500; }"
        "a:hover { text-decoration: underline; color: #2980b9; }"
        "hr { border: none; height: 2px; background: linear-gradient(to right, #3498db, #ecf0f1); margin: 24px 0; }"
        "del { color: #7f8c8d; }"
        "strong { color: #2c3e50; font-weight: 600; }"
        "em { color: #34495e; font-style: italic; }"
        "</style></head><body>") + result + "</body></html>";
}