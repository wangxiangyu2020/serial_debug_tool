// SplashScreen.cpp
#include "ui/SplashScreen.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QMovie>
#include <QTimer>

SplashScreen::SplashScreen(QWidget* parent) : QWidget(parent)
{
    this->setUI();
}

void SplashScreen::onAnimationFinished()
{
    m_pMovie->stop();
    emit finished();
    this->close();
}

void SplashScreen::setUI()
{
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    this->setAttribute(Qt::WA_TranslucentBackground);
    // 创建用于显示动画的标签
    m_pMovieLabel = new QLabel(this);
    m_pMovieLabel->setAlignment(Qt::AlignCenter);
    m_pMovieLabel->setMinimumSize(500, 300); // 设置最小尺寸
    // 加载GIF动画
    m_pMovie = new QMovie(":/resources/image/ikun.gif"); // 替换为你的GIF路径
    m_pMovieLabel->setMovie(m_pMovie);
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(m_pMovieLabel);
    setLayout(layout);
    // 启动动画
    m_pMovie->start();
    // 设置启动画面大小
    this->resize(500, 300);
    this->setFixedSize(500, 300);
    // 3秒后结束启动画面
    QTimer::singleShot(8000, this, &SplashScreen::onAnimationFinished);
}
