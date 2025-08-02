/**
  ******************************************************************************
  * @file           : ChannelManager.h
  * @author         : wangxiangyu
  * @brief          : 通道信息管理工具类（单例模式）
  * @attention      : None
  * @date           : 2025/7/31
  ******************************************************************************
  */

#ifndef CHANNELMANAGER_H
#define CHANNELMANAGER_H

#include <QObject>
#include <QMap>
#include <QString>
#include <QMutex>
#include <QVariant>

struct ChannelInfo
{
    QString id;
    QString name;
    QString color;

    ChannelInfo() = default;

    ChannelInfo(const QString& i, const QString& n, const QString& c) : id(i), name(n), color(c)
    {
    }
};

class ChannelManager : public QObject
{
    Q_OBJECT

public:
    static ChannelManager* getInstance();

    // 通道操作
    bool addChannel(const QString& id, const QString& name, const QString& color);
    bool removeChannel(const QString& id);
    bool updateChannel(const QString& id, const QString& name, const QString& newColor);
    void clearChannels();
    // 查询操作
    QList<ChannelInfo> getAllChannels() const;
    ChannelInfo getChannel(const QString& id) const;
    bool hasChannel(const QString& id) const;
    int getChannelCount() const;
    // 通道数据crud操作
    void addChannelData(const QString& channelId, const QVariant& data);

signals:
    void channelAdded(const QString& name, const QString& color);
    void channelRemoved(const QString& name);
    void channelUpdated(const QString& name, const QString& color);
    void channelsCleared();

    void channelDataAdded(const QString& channelId, const QVariant& data);

private:
    explicit ChannelManager(QObject* parent = nullptr);
    ~ChannelManager() = default;

    ChannelManager(const ChannelManager&) = delete;
    ChannelManager& operator=(const ChannelManager&) = delete;

private:
    static ChannelManager* m_instance;
    static QMutex m_mutex;
    QMap<QString, ChannelInfo> m_channels; // 改为存储完整的ChannelInfo
    mutable QMutex m_dataMutex;
    // 通道数据相关
    mutable QMutex m_channelDataMutex;
};

#endif // CHANNELMANAGER_H
