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
    // 静态工厂方法/单例方法
    static ChannelManager* getInstance();

    // 拷贝控制
    ChannelManager(const ChannelManager&) = delete;
    ChannelManager& operator=(const ChannelManager&) = delete;

    // 查询操作方法
    QList<ChannelInfo> getAllChannels() const;
    ChannelInfo getChannel(const QString& id) const;

    // 通道数据操作方法
    void addChannelData(const QString& channelId, const QVariant& data);

    int getSampleRate() const;

public slots:
    void onGetAllChannels();
    void onAddChannel(const QString& id, const QString& name, const QString& color);
    void onRemoveChannel(const QString& id);
    void onClearAllChannelData();
    void onImportChannelsData();
    void onExportChannelsData();
    void onGetSampleRate();
    void onSetSampleRate(int rate);
    void onStartDataDispatch();
    void onStopDataDispatch();

signals:
    void channelAddedRequested(const QString& name, const QString& color);
    void channelRemovedRequested(const QString& name);
    void channelDataAddedRequested(const QString& channelName, const QVariant& data);
    void channelDataProcessRequested(bool status);
    void channelsDataAllClearedRequested();
    void channelsDataImportedRequested();
    void channelsDataExportedRequested();

    void sendAllChannelsRequested(QList<ChannelInfo> channels);
    void statusChanged(const QString& status);
    void sendSampleRateRequested(int rate);

private slots:
    void onDispatchQueuedData();

private:
    // 构造函数和析构函数
    explicit ChannelManager(QObject* parent = nullptr);
    ~ChannelManager() = default;

    void connectSignals();

    // 静态成员变量
    static ChannelManager* m_instance;
    static QMutex m_mutex;

    // 核心数据成员
    QMap<QString, ChannelInfo> m_channels;
    QQueue<QPair<QString, QVariant>> m_dataQueue;

    // 定时器对象
    QTimer* m_dataDispatchTimer = nullptr;

    // 同步对象
    mutable QMutex m_dataMutex;
    mutable QMutex m_channelDataMutex;
    mutable QMutex m_queueMutex;

    // 配置变量
    bool m_isDispatching = false;
    int m_sampleRate = 100;
};

#endif // CHANNELMANAGER_H
