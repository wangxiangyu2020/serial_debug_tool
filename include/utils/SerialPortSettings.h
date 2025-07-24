/**
  ******************************************************************************
  * @file           : SerialSettings.h
  * @author         : wangxiangyu
  * @brief          : None
  * @attention      : None
  * @date           : 2025/7/19
  ******************************************************************************
  */


#ifndef SERIALSETTINGS_H
#define SERIALSETTINGS_H

#include <QObject>
#include <QVariant>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QComboBox>

class SerialPortSettings : public QObject
{
    Q_OBJECT

public:
    explicit SerialPortSettings(QObject* parent = nullptr);
    ~SerialPortSettings() = default;

public:
    struct Option
    {
        QString displayName;
        QVariant value;
    };

    static QVector<Option> getBaudRateOptions();
    static QVector<Option> getDataBitsOptions();
    static QVector<Option> getParityOptions();
    static QVector<Option> getStopBitsOptions();
    static QVector<Option> getFlowControlOptions();

    static int getDefaultIndex(const QVector<Option>& options, const QVariant& defaultValue);
    static void setSerialPortComboBox(QComboBox* combo, const QVector<SerialPortSettings::Option>& options,
                            const QVariant& defaultValue);
};

#endif //SERIALSETTINGS_H
