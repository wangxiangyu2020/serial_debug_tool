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

struct ChannelInfo
{
    QString name;
    QString id;
    QString color;

    ChannelInfo() = default;

    ChannelInfo(const QString& n, const QString& i, const QString& c) : name(n), id(i), color(c)
    {
    }
};

class ChannelManager : public QObject
{
    Q_OBJECT

public:
    static ChannelManager* getInstance();

    // 通道操作
    bool addChannel(const QString& name, const QString& id, const QString& color);
    bool removeChannel(const QString& name);
    bool updateChannel(const QString& name, const QString& id, const QString& newColor);
    void clearChannels();

    // 查询操作
    QList<ChannelInfo> getAllChannels() const;
    ChannelInfo getChannel(const QString& name) const;
    bool hasChannel(const QString& name) const;
    int getChannelCount() const;

signals:
    void channelAdded(const QString& name, const QString& color);
    void channelRemoved(const QString& name);
    void channelUpdated(const QString& name, const QString& color);
    void channelsCleared();

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
};

#endif // CHANNELMANAGER_H
