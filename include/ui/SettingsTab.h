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
    explicit SettingsTab(QWidget* parent = nullptr);
    ~SettingsTab() = default;

protected:
    void resizeEvent(QResizeEvent* event) override;

private:
    void setUI();
    void createComponents();

private:
    QTabWidget* m_pTabWidget = nullptr;
    QWebEngineView* m_pReadmeViewer = nullptr;
    QTimer* m_renderTimer = nullptr;

    QString convertMarkdownToHtml(const QString& markdown);
};

#endif //SETTINGSTAB_H
