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
    if (file.open(QFile::ReadOnly))
    {
        QTextStream stream(&file);
        QString styleSheet = stream.readAll();
        widget->setStyleSheet(styleSheet);
        file.close();
    }
}

QString StyleLoader::loadStyleFromFileToString(const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QFile::ReadOnly))
    {
        qDebug() << "Cannot open stylesheet file:" << filePath;
        return QString();
    }

    QTextStream stream(&file);
    QString styleSheet = stream.readAll();
    file.close();

    return styleSheet;
}

QString StyleLoader::getColorHex(const QString& colorName)
{
    static QMap<QString, QString> colorMap = {
        {"红色", "#FF0000"},
        {"蓝色", "#0000FF"},
        {"绿色", "#00FF00"},
        {"橙色", "#FFA500"},
        {"紫色", "#800080"},
        {"青色", "#00FFFF"},
        {"黄色", "#FFFF00"},
        {"粉色", "#FFC0CB"}
    };

    return colorMap.value(colorName, "#808080");
}
