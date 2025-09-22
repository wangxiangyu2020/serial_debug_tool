/**
  ******************************************************************************
  * @file           : ModbusUtils.h
  * @author         : wangxiangyu
  * @brief          : None
  * @attention      : None
  * @date           : 2025/9/22
  ******************************************************************************
  */


#ifndef MODBUSUTILS_H
#define MODBUSUTILS_H

#include <QVariant>
#include <cstring> // for memcpy
#include <QtGlobal>
#include "utils/ModbusTag.h"

namespace ModbusUtils
{
    quint32 reassemble32BitValue(quint16 reg1, quint16 reg2, ModbusTag::ByteOrder byteOrder);
    QVariant interpretRaw32BitValue(quint32 rawValue, ModbusTag::DataType dataType);
}

#endif //MODBUSUTILS_H
