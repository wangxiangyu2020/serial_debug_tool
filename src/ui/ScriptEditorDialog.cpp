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
            " * 查找一个5字节的完整数据帧。\n"
            " */\n"
            "function findFrame(buffer) {\n"
            "    // 查找 0xEB ... 0xED 格式的5字节固定长度帧\n"
            "    for (var i = 4; i < buffer.length; ++i) {\n"
            "        if (buffer[i] === 0xED && buffer[i - 4] === 0xEB) {\n"
            "            return i;\n"
            "        }\n"
            "    }\n"
            "    return -1; // 没有找到完整帧\n"
            "}\n"
            "\n"
            "/**\n"
            " * 解析一个完整的数据帧。\n"
            " * @param {Array<number>} frame - 一个完整的数据帧缓冲区。\n"
            " * @param {object} context - 上下文对象。\n"
            " * @returns {object|null} - 返回一个包含解析数据的对象。\n"
            " */\n"
            "function parseFrame(frame, context) {\n"
            "    if (frame.length !== 5 || frame[0] !== 0xEB || frame[4] !== 0xED) {\n"
            "        return null; // 格式不对，丢弃\n"
            "    }\n"
            "\n"
            "    // 第1步：识别通道ID\n"
            "    var channelId = frame[1];\n"
            "\n"
            "    // 第2步：提取数据\n"
            "    var value = (frame[2] << 8) | frame[3];\n"
            "    if (value & 0x8000) { value -= 0x10000; } // 符号转换\n"
            "\n"
            "     // --- 在 displayText 前面加上来源信息 --- (可选)\n"
            "     // var displayTextWithSource = \"from \" + context.source + \": \" + value;\n"
            "    \n"
            "    // 第3步：根据通道ID返回不同的对象\n"
            "    // channelId 需要与您在软件中添加通道时的\"通道标识符\"一致\n"
            "    switch (channelId) {\n"
            "        case 1:\n"
            "            return {\n"
            "                displayText: \"channel1: \" + value, // 用于显示文本\n"
            "                chartData: { channelId: \"ch1\", point: value } // 用于绘制图表\n"
            "            };\n"
            "        case 2:\n"
            "            return {\n"
            "                displayText: \"channel2: \" + value,\n"
            "                chartData: { channelId: \"ch2\", point: value }\n"
            "            };\n"
            "    }\n"
            "}\n"
            "\n"
            "/**\n"
            " * (核心函数) 批处理整个数据缓冲区。\n"
            " * @param {Array<number>} buffer - 从C++传入的整个原始数据缓冲区。\n"
            " * @param {object} context - 从C++传入的上下文对象，包含了额外信息。\n"
            " * 例如: { source: \"192.168.1.100:12345\" } { source: \"COM1\" }\n"
            " * @returns {object} - { bytesConsumed: number, frames: Array<object> }\n"
            " */\n"
            "function processBuffer(buffer, context) {\n"
            "    var results = [];\n"
            "    var bytesConsumed = 0;\n"
            "    while (bytesConsumed < buffer.length) {\n"
            "        var remainingBuffer = buffer.slice(bytesConsumed);\n"
            "        var frameEndPos = findFrame(remainingBuffer);\n"
            "        if (frameEndPos < 0) { break; }\n"
            "        var frameSize = frameEndPos + 1;\n"
            "        var completeFrame = remainingBuffer.slice(0, frameSize);\n"
            "        var parsedObject = parseFrame(completeFrame, context); \n"
            "        if (parsedObject) { results.push(parsedObject); }\n"
            "        bytesConsumed += frameSize;\n"
            "    }\n"
            "    return { bytesConsumed: bytesConsumed, frames: results };\n"
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
        "2. parseFrame(frame, context) - 解析帧数据并返回结果\n"
        "3. processBuffer(buffer, context) - 批处理整个数据缓冲区，找出所有完整的数据帧并解析它们",
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
