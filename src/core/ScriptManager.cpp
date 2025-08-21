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

bool ScriptManager::loadScript(const QString& scriptText)
{
    QMutexLocker locker(&m_mutex);
    m_lastError.clear();
    m_findFrameFunction = QJSValue();
    m_parseFrameFunction = QJSValue();
    // 检查用户脚本中是否定义了我们需要的函数
    QJSValue result = m_jsEngine.evaluate(scriptText);
    if (result.isError())
    {
        m_lastError = QString("脚本编译错误: %1\n在第 %2 行").arg(result.toString()).arg(result.property("lineNumber").toInt());
        return false;
    }
    m_findFrameFunction = m_jsEngine.globalObject().property("findFrame");
    if (!m_findFrameFunction.isCallable())
    {
        m_lastError = "脚本错误: 未找到名为 'findFrame' 的函数。";
        return false;
    }
    m_parseFrameFunction = m_jsEngine.globalObject().property("parseFrame");
    if (!m_parseFrameFunction.isCallable())
    {
        m_lastError = "脚本错误: 未找到名为 'parseFrame' 的函数。";
        return false;
    }
    m_isScriptLoaded = true;
    return true;
}

bool ScriptManager::isScriptLoaded()
{
    return m_isScriptLoaded;
}

QString ScriptManager::getLastError() const
{
    return m_lastError;
}

int ScriptManager::findFrame(const QByteArray& buffer)
{
    QMutexLocker locker(&m_mutex);
    if (!m_findFrameFunction.isCallable()) return -1;
    QJSValue jsBuffer = m_jsEngine.newArray(buffer.size());
    for (int i = 0; i < buffer.size(); ++i)
    {
        jsBuffer.setProperty(i, static_cast<unsigned char>(buffer.at(i)));
    }
    QJSValueList args;
    args << jsBuffer;
    QJSValue result = m_findFrameFunction.call(args);
    if (result.isError() || !result.isNumber()) return -1;
    return result.toInt();
}

QJSValue ScriptManager::parseFrame(const QByteArray& frame)
{
    QMutexLocker locker(&m_mutex);
    if (!m_parseFrameFunction.isCallable()) return QJSValue::UndefinedValue;
    QJSValue jsFrame = m_jsEngine.newArray(frame.size());
    for (int i = 0; i < frame.size(); ++i)
    {
        jsFrame.setProperty(i, static_cast<unsigned char>(frame.at(i)));
    }
    QJSValueList args;
    args << jsFrame;
    return m_parseFrameFunction.call(args);
}

ScriptManager::ScriptManager(QObject* parent)
    : QObject(parent)
{
}
