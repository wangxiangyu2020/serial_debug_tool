/**
******************************************************************************
  * @file           : ModbusTag.h
  * @author         : wangxiangyu
  * @brief          : 定义Modbus点位（标签）的数据结构
  * @attention      : None
  * @date           : 2025/9/13
  ******************************************************************************
  */

#ifndef MODBUSTAG_H
#define MODBUSTAG_H

#include <QString>
#include <QMetaType>

// Modbus点位（或称标签）的配置结构体
// 它包含了从设备读取一个有意义的数据所需的所有信息。
struct ModbusTag {
    // 使用C++11的类内初始化为成员变量提供默认值
    QString name = "New Tag";         // 点位名称, e.g., "电机转速"
    int slaveId = 1;                  // 从站地址 (1-247)
    int functionCode = 3;             // 功能码, e.g., 3 (读保持寄存器)
    int address = 40001;              // 寄存器地址

    // 数据类型枚举
    enum class DataType {
        UInt16,         // 16位无符号整数
        Int16,          // 16位有符号整数
        UInt32,         // 32位无符号整数
        Int32,          // 32位有符号整数
        Float32,        // 32位浮点数
        Double64        // 64位双精度浮点数
    };
    DataType dataType = DataType::UInt16;

    // 字节序（端序）枚举，对32位及以上数据类型有效
    enum class ByteOrder {
        BigEndian,              // ABCD (高字节在前)
        LittleEndian,           // DCBA (低字节在前)
        BigEndianByteSwap,      // BADC (高低字节交换)
        LittleEndianByteSwap    // CDAB (字交换)
    };
    ByteOrder byteOrder = ByteOrder::BigEndian;

    // 数值换算参数
    double gain = 1.0;                // 增益/乘法系数
    double offset = 0.0;              // 偏移/加法量

    // 默认构造函数
    ModbusTag() = default;
};

// 使用Q_DECLARE_METATYPE宏，让这个自定义结构体可以被Qt的信号槽机制和QVariant识别。
// 这对于在不同模块间传递ModbusTag对象至关重要。
Q_DECLARE_METATYPE(ModbusTag)

#endif // MODBUSTAG_H