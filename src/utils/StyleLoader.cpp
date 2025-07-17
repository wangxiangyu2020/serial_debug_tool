/**
  ******************************************************************************
  * @file           : StyleLoader.cpp
  * @author         : wangxiangyu
  * @brief          : None
  * @attention      : None
  * @date           : 2025/7/17
  ******************************************************************************
  */
#include "utils/StyleLoader.h"

StyleLoader::StyleLoader(QObject* parent)
    : QObject(parent)
{
}

void StyleLoader::loadStyleFromFile(QWidget* widget, const QString& filePath)
{
    QFile file(filePath);
    if (file.open(QFile::ReadOnly)) {
        QTextStream stream(&file);
        QString styleSheet = stream.readAll();
        widget->setStyleSheet(styleSheet);
        file.close();
    }
}