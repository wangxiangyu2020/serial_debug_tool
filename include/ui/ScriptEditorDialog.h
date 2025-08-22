/**
  ******************************************************************************
  * @file           : ScriptEditorDialog.h
  * @author         : wangxiangyu
  * @brief          : None
  * @attention      : None
  * @date           : 2025/8/22
  ******************************************************************************
  */

#ifndef SCRIPTEDITORDIALOG_H
#define SCRIPTEDITORDIALOG_H

#include <QDialog>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QPushButton>
#include <QHBoxLayout>
#include <QLabel>
#include <QStyle>
#include <utils/StyleLoader.h>
#include <utils/JavaScriptHighlighter.h>

class ScriptEditorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ScriptEditorDialog(QWidget* parent = nullptr);
    ~ScriptEditorDialog() = default;

    void setDefaultScript();
    // 获取和设置脚本内容
    QString getScriptContent() const;

private slots:
    void onToggleTheme();

private:
    void setUI();
    void createComponents();
    void createLayout();
    void connectSignals();

    // UI组件
    QLabel* m_pDescriptionLabel = nullptr;
    QTextEdit* m_pScriptEditor = nullptr;

    QPushButton* m_pThemeToggleButton = nullptr;
    bool m_isDarkTheme = true; // 默认使用深色主题
    QPushButton* m_pResetButton = nullptr;
    QPushButton* m_pCancelButton = nullptr;
    QPushButton* m_pSaveButton = nullptr;

    QVBoxLayout* m_pMainLayout = nullptr;
    QHBoxLayout* m_pButtonLayout = nullptr;

    JavaScriptHighlighter* m_pHighlighter = nullptr;
};

#endif //SCRIPTEDITORDIALOG_H
