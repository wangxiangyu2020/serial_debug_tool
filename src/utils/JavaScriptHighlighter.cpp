/**
  ******************************************************************************
  * @file           : JavaScriptHighlighter.cpp
  * @author         : wangxiangyu
  * @brief          : None
  * @attention      : None
  * @date           : 2025/8/22
  ******************************************************************************
  */
#include "utils/JavaScriptHighlighter.h"

JavaScriptHighlighter::JavaScriptHighlighter(QTextDocument* parent)
    : QSyntaxHighlighter(parent)
{
    // 初始化深色主题颜色方案
    m_darkScheme = {
        QColor(86, 156, 214), // keywordColor - 蓝色
        QColor(106, 153, 85), // commentColor - 绿色
        QColor(214, 157, 133), // stringColor - 橙色
        QColor(220, 220, 170), // functionColor - 黄色
        QColor(181, 206, 168), // numberColor - 浅绿色
        QColor(78, 201, 176) // classColor - 青色
    };

    // 初始化浅色主题颜色方案
    m_lightScheme = {
        QColor(0, 0, 255), // keywordColor - 蓝色
        QColor(0, 128, 0), // commentColor - 绿色
        QColor(163, 21, 21), // stringColor - 深红色
        QColor(128, 0, 128), // functionColor - 紫色
        QColor(0, 128, 128), // numberColor - 青色
        QColor(43, 145, 175) // classColor - 蓝绿色
    };

    // 默认使用深色主题
    m_isDarkTheme = true;
    m_currentScheme = m_darkScheme;

    this->setupHighlightingRules();
}

void JavaScriptHighlighter::setTheme(bool isDark)
{
    if (m_isDarkTheme != isDark)
    {
        m_isDarkTheme = isDark;
        m_currentScheme = isDark ? m_darkScheme : m_lightScheme;
        this->updateColorScheme();
        this->setupHighlightingRules(); // 重新设置高亮规则
        // 重新高亮所有文本
        this->rehighlight();
    }
}

void JavaScriptHighlighter::setupHighlightingRules()
{
    // 设置初始颜色方案
    this->updateColorScheme();

    // 清空现有规则
    highlightingRules.clear();

    // 关键字
    QStringList keywordPatterns;
    keywordPatterns << "\\bfunction\\b" << "\\bvar\\b" << "\\blet\\b" << "\\bconst\\b"
        << "\\bif\\b" << "\\belse\\b" << "\\bfor\\b" << "\\bwhile\\b"
        << "\\bdo\\b" << "\\breturn\\b" << "\\btrue\\b" << "\\bfalse\\b"
        << "\\bnull\\b" << "\\bundefined\\b" << "\\bnew\\b" << "\\bthis\\b"
        << "\\btypeof\\b" << "\\binstanceof\\b" << "\\bbreak\\b" << "\\bcontinue\\b"
        << "\\btry\\b" << "\\bcatch\\b" << "\\bfinally\\b" << "\\bthrow\\b"
        << "\\bswitch\\b" << "\\bcase\\b" << "\\bdefault\\b";

    for (const QString& pattern : keywordPatterns)
    {
        HighlightingRule rule;
        rule.pattern = QRegularExpression(pattern);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }

    // 函数调用
    HighlightingRule functionRule;
    functionRule.pattern = QRegularExpression("\\b[A-Za-z0-9_]+(?=\\()");
    functionRule.format = functionFormat;
    highlightingRules.append(functionRule);

    // 单行注释
    HighlightingRule commentRule;
    commentRule.pattern = QRegularExpression("//[^\n]*");
    commentRule.format = singleLineCommentFormat;
    highlightingRules.append(commentRule);

    // 字符串
    HighlightingRule quotationRule1;
    quotationRule1.pattern = QRegularExpression("\".*?\"");
    quotationRule1.format = quotationFormat;
    highlightingRules.append(quotationRule1);

    HighlightingRule quotationRule2;
    quotationRule2.pattern = QRegularExpression("'.*?'");
    quotationRule2.format = quotationFormat;
    highlightingRules.append(quotationRule2);

    // 数字
    HighlightingRule numberRule;
    numberRule.pattern = QRegularExpression("\\b[0-9]+\\.?[0-9]*\\b");
    numberRule.format = numberFormat;
    highlightingRules.append(numberRule);
}

void JavaScriptHighlighter::updateColorScheme()
{
    // 更新关键字格式
    keywordFormat.setForeground(m_currentScheme.keywordColor);
    keywordFormat.setFontWeight(QFont::Bold);

    // 更新单行注释格式
    singleLineCommentFormat.setForeground(m_currentScheme.commentColor);
    singleLineCommentFormat.setFontItalic(true);

    // 更新多行注释格式
    multiLineCommentFormat.setForeground(m_currentScheme.commentColor);
    multiLineCommentFormat.setFontItalic(true);

    // 更新字符串格式
    quotationFormat.setForeground(m_currentScheme.stringColor);

    // 更新函数格式
    functionFormat.setForeground(m_currentScheme.functionColor);

    // 更新数字格式
    numberFormat.setForeground(m_currentScheme.numberColor);

    // 更新类格式
    classFormat.setForeground(m_currentScheme.classColor);
    classFormat.setFontWeight(QFont::Bold);
}

void JavaScriptHighlighter::highlightBlock(const QString& text)
{
    // 应用高亮规则
    for (const HighlightingRule& rule : qAsConst(highlightingRules))
    {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext())
        {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }

    // 处理多行注释
    setCurrentBlockState(0);

    int startIndex = 0;
    if (previousBlockState() != 1)
        startIndex = text.indexOf("/*");

    while (startIndex >= 0)
    {
        int endIndex = text.indexOf("*/", startIndex);
        int commentLength = 0;
        if (endIndex == -1)
        {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        }
        else
        {
            commentLength = endIndex - startIndex + 2;
        }
        setFormat(startIndex, commentLength, multiLineCommentFormat);
        startIndex = text.indexOf("/*", startIndex + commentLength);
    }
}
