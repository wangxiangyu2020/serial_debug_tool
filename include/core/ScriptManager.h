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
    int findFrame(const QString& scriptName, const QByteArray& buffer);
    QJSValue parseFrame(const QString& scriptName, const QByteArray& frame);
    bool isEnableSerialPortScript();

public slots:
    void onScriptSaved(const QString& key, const QString& scriptText);
    void onSerialPortScriptEnabled(bool enabled);

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
    QJSEngine m_jsEngine;
    // 存储脚本
    QHash<QString, QList<QJSValue>> m_scriptFunctionsMap;
};

#endif //SCRIPTMANAGER_H
