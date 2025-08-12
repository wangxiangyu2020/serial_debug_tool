// SplashScreen.h
#ifndef SPLASHSCREEN_H
#define SPLASHSCREEN_H

#include <QWidget>
#include <QLabel>
#include <QMovie>

class SplashScreen : public QWidget
{
    Q_OBJECT

public:
    // 构造函数和析构函数
    explicit SplashScreen(QWidget* parent = nullptr);

signals:
    void finished();

private slots:
    void onAnimationFinished();

private:
    // 私有方法
    void setUI();

    // UI组件成员
    QLabel* m_pMovieLabel = nullptr;
    QMovie* m_pMovie = nullptr;
};

#endif // SPLASHSCREEN_H
