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

QJSValue ScriptManager::processBuffer(const QString& scriptName, const QByteArray& buffer, const QJSValue& context)
{
    QMutexLocker locker(&m_mutex);

    if (!m_scriptFunctionsMap.contains(scriptName))
        return QJSValue::UndefinedValue;

    QJSValue processBufferFunction = m_scriptFunctionsMap.value(scriptName);
    if (!processBufferFunction.isCallable())
        return QJSValue::UndefinedValue;

    QJSValue jsBuffer = m_jsEngine.newArray(buffer.size());
    for (int i = 0; i < buffer.size(); ++i)
    {
        jsBuffer.setProperty(i, static_cast<unsigned char>(buffer.at(i)));
    }

    QJSValueList args;
    args << jsBuffer;
    // --- 新增：将 context 对象作为第二个参数传给JS函数 ---
    if (context.isObject()) args << context;
    QJSValue result = processBufferFunction.call(args);
    jsBuffer = QJSValue();

    if (result.isError())
    {
        qWarning() << "Script execution error in processBuffer:" << result.toString();
        return QJSValue::UndefinedValue;
    }

    return result;
}

QJSEngine* ScriptManager::getJsEngine()
{
    return &m_jsEngine;
}

bool ScriptManager::isEnableSerialPortScript()
{
    return m_isSerialPortScriptEnabled;
}

bool ScriptManager::isEnableTcpNetworkClientScript()
{
    return m_isTcpNetworkClientScriptEnabled;
}

bool ScriptManager::isEnableTcpNetworkServerScript()
{
    return m_isTcpNetworkServerScriptEnabled;
}

bool ScriptManager::isTcpNetworkClientConnected()
{
    return m_tcpNetworkClientConnected;
}

bool ScriptManager::isTcpNetworkServerListen()
{
    return m_tcpNetworkServerListening;
}

void ScriptManager::onScriptSaved(const QString& key, const QString& scriptText)
{
    // 参数验证
    if (key.isEmpty()) return;
    if (scriptText.size() > 50000)
    {
        emit saveStatusChanged(key, "脚本过大，请优化脚本代码。");
        return;
    }

    // 1. 使用临时的JS引擎进行验证
    {
        QJSEngine validationEngine;
        QJSValue validationResult = validationEngine.evaluate(scriptText);

        if (validationResult.isError())
        {
            emit saveStatusChanged(
                key,
                QString("脚本编译错误: %1\n在第 %2 行")
                .arg(validationResult.toString())
                .arg(validationResult.property("lineNumber").toInt()));
            return;
        }
        if (!validationEngine.globalObject().property("findFrame").isCallable())
        {
            emit saveStatusChanged(key, "脚本错误: 未找到名为 'findFrame' 的函数。");
            return;
        }
        if (!validationEngine.globalObject().property("parseFrame").isCallable())
        {
            emit saveStatusChanged(key, "脚本错误: 未找到名为 'parseFrame' 的函数。");
            return;
        }
        if (!validationEngine.globalObject().property("processBuffer").isCallable())
        {
            emit saveStatusChanged(key, "脚本错误: 未找到名为 'processBuffer' 的函数。");
            return;
        }
    }

    // 2. 验证通过后，再对我们持久化的主引擎 m_jsEngine 进行操作
    QMutexLocker locker(&m_mutex);
    // 在主引擎上评估脚本。这会用新脚本中的函数覆盖掉旧的同名函数。
    m_jsEngine.evaluate(scriptText);
    // 3. 从【主引擎】获取最新的 processBuffer 函数句柄并存储
    QJSValue processBufferFunction = m_jsEngine.globalObject().property("processBuffer");
    m_scriptFunctionsMap[key] = processBufferFunction;

    emit saveStatusChanged(key, "脚本加载成功。");
}

void ScriptManager::onSerialPortScriptEnabled(bool enabled)
{
    m_isSerialPortScriptEnabled = enabled;
    // 如果禁用脚本，进行一次垃圾回收
    if (!enabled)
    {
        QMutexLocker locker(&m_mutex);
        m_jsEngine.collectGarbage();
    }
}

void ScriptManager::onTcpNetworkClientScriptEnabled(bool enabled)
{
    m_isTcpNetworkClientScriptEnabled = enabled;
    // 如果禁用脚本，进行一次垃圾回收
    if (!enabled)
    {
        QMutexLocker locker(&m_mutex);
        m_jsEngine.collectGarbage();
    }
}

void ScriptManager::onTcpNetworkServerScriptEnabled(bool enabled)
{
    m_isTcpNetworkServerScriptEnabled = enabled;
    if (!enabled)
    {
        QMutexLocker locker(&m_mutex);
        m_jsEngine.collectGarbage();
    }
}

void ScriptManager::onTcpNetworkClientConnected(bool connected)
{
    m_tcpNetworkClientConnected = connected;
}

void ScriptManager::onTcpNetworkServerListen(bool listening)
{
    m_tcpNetworkServerListening = listening;
}

ScriptManager::ScriptManager(QObject* parent)
    : QObject(parent)
{
}
