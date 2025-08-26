/**
  ******************************************************************************
  * @file           : ScriptManager.h
  * @author         : wangxiangyu
  * @brief          : None
  * @attention      : None
  * @date           : 2025/8/21
  ******************************************************************************
  */

#ifndef SCRIPTMANAGER_H
#define SCRIPTMANAGER_H

#include <QObject>
#include <QJSEngine>
#include <QJSValue>
#include <QMutex>

class ScriptManager : public QObject
{
    Q_OBJECT

public:
    static ScriptManager* getInstance();
    QJSValue processBuffer(const QString& scriptName, const QByteArray& buffer, const QJSValue& context = QJSValue());
    QJSEngine* getJsEngine();
    bool isEnableSerialPortScript();
    bool isEnableTcpNetworkClientScript();
    bool isEnableTcpNetworkServerScript();
    bool isTcpNetworkClientConnected();
    bool isTcpNetworkServerListen();

public slots:
    void onScriptSaved(const QString& key, const QString& scriptText);
    void onSerialPortScriptEnabled(bool enabled);
    void onTcpNetworkClientScriptEnabled(bool enabled);
    void onTcpNetworkServerScriptEnabled(bool enabled);
    void onTcpNetworkClientConnected(bool connected);
    void onTcpNetworkServerListen(bool listening);

signals:
    void saveStatusChanged(const QString& key, const QString& status);

private:
    explicit ScriptManager(QObject* parent = nullptr);
    ~ScriptManager() = default;
    ScriptManager(const ScriptManager&) = delete;
    ScriptManager& operator=(const ScriptManager&) = delete;

    static ScriptManager* m_instance;
    static QMutex m_mutex;

    bool m_isSerialPortScriptEnabled = false;
    bool m_isTcpNetworkClientScriptEnabled = false;
    bool m_isTcpNetworkServerScriptEnabled = false;
    bool m_tcpNetworkClientConnected = false;
    bool m_tcpNetworkServerListening = false;
    QJSEngine m_jsEngine;
    // 存储脚本
    QHash<QString, QJSValue> m_scriptFunctionsMap;
};

#endif //SCRIPTMANAGER_H
