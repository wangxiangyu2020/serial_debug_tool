/**
  ******************************************************************************
  * @file           : SerialSettings.cpp
  * @author         : wangxiangyu
  * @brief          : None
  * @attention      : None
  * @date           : 2025/7/19
  ******************************************************************************
  */

#include "utils/SerialPortSettings.h"

SerialPortSettings::SerialPortSettings(QObject* parent)
    : QObject(parent)
{
}

QVector<SerialPortSettings::Option> SerialPortSettings::getBaudRateOptions()
{
    static const QVector<Option> rates = {
        {"1200", QSerialPort::Baud1200},
        {"2400", QSerialPort::Baud2400},
        {"4800", QSerialPort::Baud4800},
        {"9600", QSerialPort::Baud9600},
        {"19200", QSerialPort::Baud19200},
        {"38400", QSerialPort::Baud38400},
        {"57600", QSerialPort::Baud57600},
        {"115200", QSerialPort::Baud115200}
    };
    return rates;
}

QVector<SerialPortSettings::Option> SerialPortSettings::getDataBitsOptions()
{
    static const QVector<Option> dataBits = {
        {"5", QSerialPort::Data5},
        {"6", QSerialPort::Data6},
        {"7", QSerialPort::Data7},
        {"8", QSerialPort::Data8}
    };
    return dataBits;
}

QVector<SerialPortSettings::Option> SerialPortSettings::getParityOptions()
{
    static const QVector<Option> parity = {
        {"None", QSerialPort::NoParity},
        {"Even", QSerialPort::EvenParity},
        {"Odd", QSerialPort::OddParity},
        {"Space", QSerialPort::SpaceParity},
        {"Mark", QSerialPort::MarkParity}
    };
    return parity;
}

QVector<SerialPortSettings::Option> SerialPortSettings::getStopBitsOptions()
{
    static const QVector<Option> stopBits = {
        {"1", QSerialPort::OneStop},
        {"1.5", QSerialPort::OneAndHalfStop},
        {"2", QSerialPort::TwoStop}
    };
    return stopBits;
}

QVector<SerialPortSettings::Option> SerialPortSettings::getFlowControlOptions()
{
    static const QVector<Option> flowControl = {
        {"None", QSerialPort::NoFlowControl},
        {"RTS/CTS", QSerialPort::HardwareControl},
        {"XON/XOFF", QSerialPort::SoftwareControl}
    };
    return flowControl;
}

int SerialPortSettings::getDefaultIndex(const QVector<Option>& options, const QVariant& defaultValue)
{
    for (int i = 0; i < options.size(); ++i)
    {
        if (options[i].value == defaultValue)
        {
            return i;
        }
    }
    return 0;
}

void SerialPortSettings::setSerialPortComboBox(QComboBox* combo, const QVector<SerialPortSettings::Option>& options,
                                               const QVariant& defaultValue)
{
    for (const auto& option : options)
    {
        combo->addItem(option.displayName, option.value);
    }
    int index = SerialPortSettings::getDefaultIndex(options, defaultValue);
    combo->setCurrentIndex(index);
}
