/**
  ******************************************************************************
  * @file           : ModbusUtils.cpp
  * @author         : wangxiangyu
  * @brief          : None
  * @attention      : None
  * @date           : 2025/9/22
  ******************************************************************************
  */
#include "utils/ModbusUtils.h"

quint32 ModbusUtils::reassemble32BitValue(quint16 reg1, quint16 reg2, ModbusTag::ByteOrder byteOrder)
{
    auto swapBytes = [](uint16_t value) -> quint16
    {
        return (value << 8) | (value >> 8);
    };

    switch (byteOrder)
    {
    // ABCD (高字在前, 字节序正常) -> [reg1: AB][reg2: CD]
    case ModbusTag::ByteOrder::BigEndian:
        return (static_cast<quint32>(reg1) << 16) | reg2;
    // CDAB (低字在前, 字节序正常) -> [reg1: CD][reg2: AB]
    case ModbusTag::ByteOrder::LittleEndianByteSwap:
        return (static_cast<quint32>(reg2) << 16) | reg1;
    // BADC (高字在前, 字节序交换) -> [reg1: BA][reg2: DC]
    case ModbusTag::ByteOrder::BigEndianByteSwap:
        return (static_cast<quint32>(swapBytes(reg1)) << 16) | swapBytes(reg2);
    // DCBA (低字在前, 字节序交换) -> [reg1: DC][reg2: BA]
    case ModbusTag::ByteOrder::LittleEndian:
        return (static_cast<quint32>(swapBytes(reg2)) << 16) | swapBytes(reg1);
    default:
        // 默认情况下，按最常见的BigEndian处理
        return (static_cast<quint32>(reg1) << 16) | reg2;
    }
}

QVariant ModbusUtils::interpretRaw32BitValue(quint32 rawValue, ModbusTag::DataType dataType)
{
    switch (dataType)
    {
    case ModbusTag::DataType::UInt32:
        return QVariant(rawValue);
    case ModbusTag::DataType::Int32:
        {
            qint32 finalValue;
            std::memcpy(&finalValue, &rawValue, sizeof(qint32));
            return QVariant(finalValue);
        }
    case ModbusTag::DataType::Float32:
        {
            float finalValue;
            std::memcpy(&finalValue, &rawValue, sizeof(float));
            return QVariant(finalValue);
        }
    default:
        return QVariant();
    }
}
