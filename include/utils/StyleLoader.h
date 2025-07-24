/**
  ******************************************************************************
  * @file           : StyleLoader.h
  * @author         : wangxiangyu
  * @brief          : None
  * @attention      : None
  * @date           : 2025/7/17
  ******************************************************************************
  */


#ifndef STYLELOADER_H
#define STYLELOADER_H

#include <QObject>
#include <QFile>
#include <QTextStream>
#include <QWidget>

class StyleLoader : public QObject
{
  Q_OBJECT

public:
  explicit StyleLoader(QObject* parent = nullptr);
  ~StyleLoader() =  default;
  // 从文件路径加载样式
  static void loadStyleFromFile(QWidget* widget, const QString& filePath);
};

#endif //STYLELOADER_H
