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
    bool loadScript(const QString& scriptText);
    bool isScriptLoaded();
    QString getLastError() const;
    int findFrame(const QByteArray& buffer);
    QJSValue parseFrame(const QByteArray& frame);

private:
    explicit ScriptManager(QObject* parent = nullptr);
    ~ScriptManager() = default;
    ScriptManager(const ScriptManager&) = delete;
    ScriptManager& operator=(const ScriptManager&) = delete;

    static ScriptManager* m_instance;
    static QMutex m_mutex;

    bool m_isScriptLoaded = false;
    QString m_lastError;
    QJSEngine m_jsEngine;
    QJSValue m_findFrameFunction;
    QJSValue m_parseFrameFunction;
};

#endif //SCRIPTMANAGER_H
