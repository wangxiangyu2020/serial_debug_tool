/**
  ******************************************************************************
  * @file           : SettingsTab.h
  * @author         : wangxiangyu
  * @brief          : None
  * @attention      : None
  * @date           : 2025/7/17
  ******************************************************************************
  */


#ifndef SETTINGSTAB_H
#define SETTINGSTAB_H

#include <QWidget>
#include "utils/StyleLoader.h"
#include <QTabWidget>
#include <QWebEngineView>
#include <QRegularExpression>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QTimer>

class SettingsTab : public QWidget
{
    Q_OBJECT

public:
    // 构造函数和析构函数
    explicit SettingsTab(QWidget* parent = nullptr);
    ~SettingsTab() = default;

protected:
    // 事件处理方法
    void resizeEvent(QResizeEvent* event) override;

private:
    // 私有方法
    void setUI();
    void createComponents();
    QString convertMarkdownToHtml(const QString& markdown);

    // UI组件成员
    QTabWidget* m_pTabWidget = nullptr;
    QWebEngineView* m_pReadmeViewer = nullptr;

    // 定时器对象
    QTimer* m_renderTimer = nullptr;
};

#endif //SETTINGSTAB_H
