/**
  ******************************************************************************
  * @file           : ScriptEditorDialog.cpp
  * @author         : wangxiangyu
  * @brief          : None
  * @attention      : None
  * @date           : 2025/8/22
  ******************************************************************************
  */

#include "ui/ScriptEditorDialog.h"

ScriptEditorDialog::ScriptEditorDialog(QWidget* parent)
    : QDialog(parent)
{
    this->setUI();
    StyleLoader::loadStyleFromFile(this, ":/resources/qss/script_editor_dialog.qss");
}

void ScriptEditorDialog::setDefaultScript()
{
    if (m_pScriptEditor)
    {
        m_pScriptEditor->setPlainText(
            "/**\n"
            " * 在字节数组中查找最后一个完整帧的结束位置。\n"
            " * 协议以逗号 (ASCII 44) 结尾。\n"
            " * @param {number[]} buffer - C++ 传入的字节数组。\n"
            " * @returns {number} - 最后一个逗号的索引，如果找不到则返回 -1。\n"
            " */\n"
            "function findFrame(buffer) {\n"
            "    return buffer.lastIndexOf(44); // 44 是 ',' 的ASCII码\n"
            "}\n"
            "\n"
            "/**\n"
            " * 解析一个完整的帧。\n"
            " * @param {number[]} frame - 一个完整的帧，例如 \"CH1=123.45,\" 对应的字节数组。\n"
            " * @returns {object|null} - 返回一个包含显示和图表数据的对象，或null。\n"
            " */\n"
            "function parseFrame(frame) {\n"
            "    // 将字节数组转为字符串\n"
            "    let text = String.fromCharCode.apply(null, frame);\n"
            "    \n"
            "    let displayText = text.trim();\n"
            "\n"
            "    // 尝试解析录波数据，如果不需要录波数据，请可以不写此部分\n"
            "    let parts = text.trim().replace(',', '').split('=');\n"
            "    if (parts.length === 2) {\n"
            "        let channelId = parts[0]; // 脚本可以直接用ID作为Name\n"
            "        let value = parseFloat(parts[1]);\n"
            "        \n"
            "        if (!isNaN(value)) {\n"
            "            // 返回一个结构化的对象\n"
            "            return {\n"
            "                displayText: displayText,\n"
            "                chartData: {\n"
            "                    channelId: channelId, \n"
            "                    point: value\n"
            "                }\n"
            "            };\n"
            "        }\n"
            "    }\n"
            "    \n"
            "    // 如果不是录波数据，只返回显示文本\n"
            "    return { displayText: displayText };\n"
            "}");
    }
}

QString ScriptEditorDialog::getScriptContent() const
{
    return m_pScriptEditor ? m_pScriptEditor->toPlainText() : QString();
}

void ScriptEditorDialog::onToggleTheme()
{
    m_isDarkTheme = !m_isDarkTheme;

    if (m_isDarkTheme)
    {
        m_pScriptEditor->setProperty("class", "dark-theme");
        m_pThemeToggleButton->setText("浅色主题");
        m_pHighlighter->setTheme(true);
    }
    else
    {
        m_pScriptEditor->setProperty("class", "light-theme");
        m_pThemeToggleButton->setText("深色主题");
        m_pHighlighter->setTheme(false);
    }

    // 重新应用样式
    m_pScriptEditor->style()->unpolish(m_pScriptEditor);
    m_pScriptEditor->style()->polish(m_pScriptEditor);
    m_pScriptEditor->update();
}

void ScriptEditorDialog::setUI()
{
    this->createComponents();
    this->createLayout();
    this->connectSignals();
}

void ScriptEditorDialog::createComponents()
{
    // 设置窗口属性
    this->setWindowTitle("脚本编辑器");
    this->setMinimumSize(600, 400);
    // 添加窗口控制按钮
    this->setWindowFlags(Qt::Window |
        Qt::WindowCloseButtonHint |
        Qt::WindowMinimizeButtonHint |
        Qt::WindowMaximizeButtonHint);
    // 说明文本
    m_pDescriptionLabel = new QLabel(
        "编写JavaScript脚本来处理接收到的数据。脚本需要实现以下函数：\n"
        "1. findFrame(buffer) - 查找完整帧的结束位置\n"
        "2. parseFrame(frame) - 解析帧数据并返回结果",
        this);
    m_pDescriptionLabel->setObjectName("m_pDescriptionLabel");
    m_pDescriptionLabel->setWordWrap(true);
    // 代码编辑器
    m_pScriptEditor = new QTextEdit(this);
    this->setDefaultScript();
    m_pScriptEditor->setObjectName("m_pScriptEditor");
    m_pScriptEditor->setProperty("class", "dark-theme");
    m_pHighlighter = new JavaScriptHighlighter(m_pScriptEditor->document());
    // 主题切换按钮
    m_pThemeToggleButton = new QPushButton("浅色主题", this);
    m_pThemeToggleButton->setObjectName("m_pThemeToggleButton");
    m_pResetButton = new QPushButton("重置默认", this);
    m_pResetButton->setObjectName("m_pResetButton");

    m_pCancelButton = new QPushButton("取消", this);
    m_pCancelButton->setObjectName("m_pCancelButton");

    m_pSaveButton = new QPushButton("保存", this);
    m_pSaveButton->setObjectName("m_pSaveButton");
}

void ScriptEditorDialog::createLayout()
{
    // 创建布局
    m_pMainLayout = new QVBoxLayout(this);
    m_pMainLayout->setSpacing(5);
    m_pMainLayout->setContentsMargins(15, 15, 15, 15);
    // 按钮布局
    m_pButtonLayout = new QHBoxLayout();
    m_pButtonLayout->setSpacing(8);

    m_pButtonLayout->addWidget(m_pThemeToggleButton);
    m_pButtonLayout->addWidget(m_pResetButton); // 添加重置按钮
    m_pButtonLayout->addStretch();
    m_pButtonLayout->addWidget(m_pCancelButton);
    m_pButtonLayout->addWidget(m_pSaveButton);

    // 添加到主布局
    m_pMainLayout->addWidget(m_pDescriptionLabel);
    m_pMainLayout->addWidget(m_pScriptEditor);
    m_pMainLayout->addLayout(m_pButtonLayout);

    // 设置对话框属性
    this->setModal(true);
}

void ScriptEditorDialog::connectSignals()
{
    // 连接按钮信号
    this->connect(m_pCancelButton, &QPushButton::clicked, [this]()
    {
        this->close();
    });

    this->connect(m_pSaveButton, &QPushButton::clicked, [this]()
    {
        // 发出保存信号或直接关闭
        this->accept();
    });
    this->connect(m_pResetButton, &QPushButton::clicked, [this]()
    {
        this->setDefaultScript();
    });
    this->connect(m_pThemeToggleButton, &QPushButton::clicked, this, &ScriptEditorDialog::onToggleTheme);
}
