/**
  ******************************************************************************
  * @file           : ScriptManager.cpp
  * @author         : wangxiangyu
  * @brief          : None
  * @attention      : None
  * @date           : 2025/8/21
  ******************************************************************************
  */

#include "core/ScriptManager.h"

ScriptManager* ScriptManager::m_instance = nullptr;
QMutex ScriptManager::m_mutex;

ScriptManager* ScriptManager::getInstance()
{
    if (m_instance == nullptr)
    {
        QMutexLocker locker(&m_mutex);
        if (m_instance == nullptr) m_instance = new ScriptManager;
    }
    return m_instance;
}

int ScriptManager::findFrame(const QString& scriptName, const QByteArray& buffer)
{
    QMutexLocker locker(&m_mutex);

    // 直接从映射表中获取函数
    if (!m_scriptFunctionsMap.contains(scriptName)) return -1;

    const QList<QJSValue>& functions = m_scriptFunctionsMap[scriptName];
    if (functions.size() < 2) return -1;

    QJSValue findFrameFunction = functions[0]; // findFrame函数
    if (!findFrameFunction.isCallable()) return -1;

    QJSValue jsBuffer = m_jsEngine.newArray(buffer.size());
    for (int i = 0; i < buffer.size(); ++i)
    {
        jsBuffer.setProperty(i, static_cast<unsigned char>(buffer.at(i)));
    }
    QJSValueList args;
    args << jsBuffer;
    QJSValue result = findFrameFunction.call(args);
    if (result.isError() || !result.isNumber()) return -1;
    return result.toInt();
}

QJSValue ScriptManager::parseFrame(const QString& scriptName, const QByteArray& frame)
{
    QMutexLocker locker(&m_mutex);

    // 直接从映射表中获取函数
    if (!m_scriptFunctionsMap.contains(scriptName))
        return QJSValue::UndefinedValue;

    const QList<QJSValue>& functions = m_scriptFunctionsMap[scriptName];
    if (functions.size() < 2)
        return QJSValue::UndefinedValue;

    QJSValue parseFrameFunction = functions[1]; // parseFrame函数
    if (!parseFrameFunction.isCallable())
        return QJSValue::UndefinedValue;

    QJSValue jsFrame = m_jsEngine.newArray(frame.size());
    for (int i = 0; i < frame.size(); ++i)
    {
        jsFrame.setProperty(i, static_cast<unsigned char>(frame.at(i)));
    }
    QJSValueList args;
    args << jsFrame;
    return parseFrameFunction.call(args);
}

bool ScriptManager::isEnableSerialPortScript()
{
    return m_isSerialPortScriptEnabled;
}

void ScriptManager::onScriptSaved(const QString& key, const QString& scriptText)
{
    // 参数验证
    if (key.isEmpty()) return;
    // 线程同步保护
    QMutexLocker locker(&m_mutex);
    // 存储脚本之前先验证脚本
    QJSValue result = m_jsEngine.evaluate(scriptText);
    if (result.isError())
    {
        emit saveStatusChanged(
            key,
            QString("脚本编译错误: %1\n在第 %2 行")
            .arg(result.toString())
            .arg(result.property("lineNumber").toInt()));
        return;
    }
    QJSValue findFrameFunction = m_jsEngine.globalObject().property("findFrame");
    if (!findFrameFunction.isCallable())
    {
        emit saveStatusChanged(key, "脚本错误: 未找到名为 'findFrame' 的函数。");
        return;
    }
    QJSValue parseFrameFunction = m_jsEngine.globalObject().property("parseFrame");
    if (!parseFrameFunction.isCallable())
    {
        emit saveStatusChanged(key, "脚本错误: 未找到名为 'parseFrame' 的函数。");
        return;
    }
    // 存储脚本函数
    QList<QJSValue> functions;
    functions.append(findFrameFunction); // 索引0: findFrame函数
    functions.append(parseFrameFunction); // 索引1: parseFrame函数
    m_scriptFunctionsMap[key] = functions;
    emit saveStatusChanged(key, "脚本加载成功。");
}

void ScriptManager::onSerialPortScriptEnabled(bool enabled)
{
    m_isSerialPortScriptEnabled = enabled;
}

ScriptManager::ScriptManager(QObject* parent)
    : QObject(parent)
{
}
