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

QJSValue ScriptManager::processBuffer(const QString& scriptName, const QByteArray& buffer)
{
    QMutexLocker locker(&m_mutex);

    if (!m_scriptFunctionsMap.contains(scriptName))
        return QJSValue::UndefinedValue;
    // 直接从 map 中获取 processBuffer 函数
    QJSValue processBufferFunction = m_scriptFunctionsMap.value(scriptName);
    if (!processBufferFunction.isCallable())
        return QJSValue::UndefinedValue;
    // 一次性创建 JS 数组，传递给脚本
    QJSValue jsBuffer = m_jsEngine.newArray(buffer.size());
    for (int i = 0; i < buffer.size(); ++i)
    {
        jsBuffer.setProperty(i, static_cast<unsigned char>(buffer.at(i)));
    }
    QJSValueList args;
    args << jsBuffer;
    QJSValue result = processBufferFunction.call(args);
    // 立即清理，释放内存
    jsBuffer = QJSValue();

    if (result.isError())
    {
        qWarning() << "Script execution error in processBuffer:" << result.toString();
        return QJSValue::UndefinedValue;
    }

    return result;
}

bool ScriptManager::isEnableSerialPortScript()
{
    return m_isSerialPortScriptEnabled;
}

void ScriptManager::onScriptSaved(const QString& key, const QString& scriptText)
{
    // 参数验证
    if (key.isEmpty()) return;
    if (scriptText.size() > 50000) // 限制脚本大小为50KB
    {
        emit saveStatusChanged(key, "脚本过大，请优化脚本代码。");
        return;
    }
    // 线程同步保护
    QMutexLocker locker(&m_mutex);
    // 清理旧的脚本函数（如果存在）
    if (m_scriptFunctionsMap.contains(key)) m_scriptFunctionsMap.remove(key);
    // 强制垃圾回收，清理JavaScript引擎内存
    m_jsEngine.collectGarbage();
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
    QJSValue processBufferFunction = m_jsEngine.globalObject().property("processBuffer");
    if (!processBufferFunction.isCallable())
    {
        emit saveStatusChanged(key, "脚本错误: 未找到名为 'processBuffer' 的函数。");
        return;
    }
    // 存储脚本函数
    m_scriptFunctionsMap[key] = processBufferFunction;
    // 清理编译结果
    result = QJSValue();
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

ScriptManager::ScriptManager(QObject* parent)
    : QObject(parent)
{
}
