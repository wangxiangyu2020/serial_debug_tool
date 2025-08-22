/**
  ******************************************************************************
  * @file           : JavaScriptHighlighter.h
  * @author         : wangxiangyu
  * @brief          : None
  * @attention      : None
  * @date           : 2025/8/22
  ******************************************************************************
  */

#ifndef JAVASCRIPTHIGHLIGHTER_H
#define JAVASCRIPTHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QRegularExpression>
#include <QTextDocument>

class JavaScriptHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    explicit JavaScriptHighlighter(QTextDocument* parent = nullptr);

    // 主题切换方法
    void setTheme(bool isDark);

protected:
    void highlightBlock(const QString& text) override;

private:
    // 初始化高亮规则
    void setupHighlightingRules();
    // 更新颜色方案
    void updateColorScheme();
    struct HighlightingRule
    {
        QRegularExpression pattern;
        QTextCharFormat format;
    };

    QVector<HighlightingRule> highlightingRules;

    // 不同主题的颜色格式
    QTextCharFormat keywordFormat;
    QTextCharFormat singleLineCommentFormat;
    QTextCharFormat multiLineCommentFormat;
    QTextCharFormat quotationFormat;
    QTextCharFormat functionFormat;
    QTextCharFormat numberFormat;
    QTextCharFormat classFormat;

    // 主题状态
    bool m_isDarkTheme = true;

    // 颜色方案
    struct ColorScheme
    {
        QColor keywordColor;
        QColor commentColor;
        QColor stringColor;
        QColor functionColor;
        QColor numberColor;
        QColor classColor;
    };

    ColorScheme m_darkScheme;
    ColorScheme m_lightScheme;
    ColorScheme m_currentScheme;
};

#endif //JAVASCRIPTHIGHLIGHTER_H
