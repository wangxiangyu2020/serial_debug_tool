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
    explicit SplashScreen(QWidget* parent = nullptr);

private slots:
    void onAnimationFinished();

signals:
    void finished();

private:
    QLabel* m_pMovieLabel = nullptr;
    QMovie* m_pMovie = nullptr;

    void setUI();
};

#endif // SPLASHSCREEN_H
