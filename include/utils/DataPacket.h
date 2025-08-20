/**
  ******************************************************************************
  * @file           : DataPacket.h
  * @author         : wangxiangyu
  * @brief          : None
  * @attention      : None
  * @date           : 2025/8/20
  ******************************************************************************
  */

#ifndef DATAPACKET_H
#define DATAPACKET_H

#include <QString>
#include <QByteArray>
#include <QVariant>

struct DataPacket
{
  QString sourceInfo; // 数据来源 (e.g., "COM3", "192.168.1.10:12345")
  QByteArray data;    // 原始字节数据
};

#endif // DATAPACKET_H
