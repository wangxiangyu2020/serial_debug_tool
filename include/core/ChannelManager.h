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
#include <QQueue>
#include <QTimer>

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
    void clearAllChannelData();

    void startDataDispatch();
    void stopDataDispatch();

    int getSampleRate() const;
    void setSampleRate(int rate);

private:
    explicit ChannelManager(QObject* parent = nullptr);
    ~ChannelManager() = default;

    ChannelManager(const ChannelManager&) = delete;
    ChannelManager& operator=(const ChannelManager&) = delete;

private slots:
    void dispatchQueuedData(); //

signals:
    void channelAdded(const QString& name, const QString& color);
    void channelRemoved(const QString& name);
    void channelUpdated(const QString& name, const QString& color);
    void channelsCleared();

    void channelDataAdded(const QString& channelId, const QVariant& data);
    void channelDataProcess(bool status);
    void channelsDataAllCleared();
    void importChannelsData();
    void channelsExportData();

private:
    static ChannelManager* m_instance;
    static QMutex m_mutex;
    QMap<QString, ChannelInfo> m_channels; // 改为存储完整的ChannelInfo
    mutable QMutex m_dataMutex;
    // 通道数据相关
    mutable QMutex m_channelDataMutex;
    QTimer* m_dataDispatchTimer = nullptr; // 数据分发定时器
    QQueue<QPair<QString, QVariant>> m_dataQueue; // 数据队列
    mutable QMutex m_queueMutex; // 队列互斥锁
    bool m_isDispatching = false; // 是否正在分发数据
    int m_sampleRate = 100;
};

#endif // CHANNELMANAGER_H
