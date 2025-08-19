/**
  ******************************************************************************
  * @file           : SerialPortConfig.cpp
  * @author         : wangxiangyu
  * @brief          : None
  * @attention      : None
  * @date           : 2025/7/17
  ******************************************************************************
  */

#include "ui/SerialPortConfigTab.h"

// 构造函数和析构函数
SerialPortConfigTab::SerialPortConfigTab(QWidget* parent)
    : QWidget(parent), m_pSaveFile(nullptr)
{
    this->setUI();
    StyleLoader::loadStyleFromFile(this, ":resources/qss/serial_prot_config_tab.qss");
}

void SerialPortConfigTab::onReadySaveFile(bool status)
{
    if (!status)
    {
        // 自动释放内存
        QScopedPointer<QFile> fileGuard(m_pSaveFile);
        m_pSaveFile = nullptr;
        m_pSerialPortRealTimeSaveWidget->hide();
        emit displaySavePathRequested();
        return;
    }
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    "保存数据",
                                                    QDir::homePath(),
                                                    "文件文本(*.txt)");
    if (fileName.isEmpty())
    {
        m_pSerialPortReceiveSettingsWidget->getSaveToFileCheckBox()->setChecked(false);
        return;
    }
    // 使用 Qt 智能指针
    QScopedPointer<QFile> newFile(new QFile(fileName));
    if (!newFile->open(QIODevice::WriteOnly | QIODevice::Text)) return;
    // 替换全局文件对象
    delete m_pSaveFile;
    m_pSaveFile = newFile.take(); // 获取所有权
    emit displaySavePathRequested(fileName);
    QTextStream out(m_pSaveFile);
    QString dataStr = SerialPortDataReceiveWidget::getReceiveTextEdit()->toPlainText();
    // 如果数据不以换行符结尾，则添加换行符
    if (!dataStr.endsWith('\n'))
    {
        dataStr.append('\n');
    }
    out << dataStr;
    m_pSaveFile->flush();
    m_pSerialPortRealTimeSaveWidget->show();
}

// 私有方法
void SerialPortConfigTab::setUI()
{
    this->setAttribute(Qt::WA_StyledBackground);
    this->createComponents();
    this->createLayout();
    this->connectSignals();
}

void SerialPortConfigTab::createComponents()
{
    // ==== 创建左侧设置区域 ====
    m_pSettingsPanel = new QWidget(this); // 左侧容器
    m_pSerialPortConfigWidget = new SerialPortConnectConfigWidget(m_pSettingsPanel);
    m_pSerialPortReceiveSettingsWidget = new SerialPortReceiveSettingsWidget(m_pSettingsPanel);
    m_pSerialPortSendSettingsWidget = new SerialPortSendSettingsWidget(m_pSettingsPanel);
    // ==== 创建右侧内容区域 ====
    m_pContentPanel = new QWidget(this); // 右侧容器
    m_pSerialPortRealTimeSaveWidget = new SerialPortRealTimeSaveWidget(m_pContentPanel);
    m_pSerialPortRealTimeSaveWidget->hide();
    m_pSerialPortDataReceiveWidget = new SerialPortDataReceiveWidget(m_pContentPanel);
    m_pSerialPortDataSendWidget = new SerialPortDataSendWidget(m_pContentPanel);
    // 设置发送容器固定高度（重要！）
    m_pSerialPortDataSendWidget->setMinimumHeight(100); // 最小高度保证可见
}

void SerialPortConfigTab::createLayout()
{
    // 左侧布局
    m_pSettingsLayout = new QVBoxLayout(m_pSettingsPanel); // 注意父对象是settingsPanel
    m_pSettingsLayout->setSpacing(0);
    m_pSettingsLayout->setContentsMargins(2, 0, 0, 0);
    m_pSettingsLayout->addWidget(m_pSerialPortConfigWidget);
    m_pSettingsLayout->addSpacing(1);
    m_pSettingsLayout->addWidget(m_pSerialPortSendSettingsWidget);
    m_pSettingsLayout->addSpacing(1);
    m_pSettingsLayout->addWidget(m_pSerialPortReceiveSettingsWidget);
    // 右侧布局
    m_pContentLayout = new QVBoxLayout(m_pContentPanel); // 父对象为contentPanel
    m_pContentLayout->setSpacing(2);
    m_pContentLayout->setContentsMargins(0, 0, 2, 0);
    m_pContentLayout->addWidget(m_pSerialPortDataReceiveWidget);
    m_pContentLayout->addWidget(m_pSerialPortDataSendWidget);
    // 添加到布局并设置伸缩比例
    m_pContentLayout->addWidget(m_pSerialPortRealTimeSaveWidget, 0);
    m_pContentLayout->addWidget(m_pSerialPortDataReceiveWidget, 9); // 1: 可伸缩区域
    m_pContentLayout->addWidget(m_pSerialPortDataSendWidget, 1); // 0: 固定高度区域
    // ==== 主水平布局 ====
    m_pMainLayout = new QHBoxLayout(this); // 关键：设置this的顶层布局
    m_pMainLayout->setContentsMargins(0, 1, 0, 1); // 移除窗口边距
    m_pMainLayout->setSpacing(2);
    m_pMainLayout->addWidget(m_pSettingsPanel, 1); // 左侧占1份空间
    m_pMainLayout->addWidget(m_pContentPanel, 3); // 右侧占3份空间（比例可调）
}

void SerialPortConfigTab::connectSignals()
{
    this->connect(m_pSerialPortReceiveSettingsWidget, &SerialPortReceiveSettingsWidget::clearDataRequested,
                  m_pSerialPortDataReceiveWidget, &SerialPortDataReceiveWidget::onClearReceiveData);
    this->connect(m_pSerialPortReceiveSettingsWidget, &SerialPortReceiveSettingsWidget::saveDataRequested,
                  m_pSerialPortDataReceiveWidget, &SerialPortDataReceiveWidget::onSaveReceiveData);
    this->connect(m_pSerialPortReceiveSettingsWidget, &SerialPortReceiveSettingsWidget::saveToFileChanged, this,
                  &SerialPortConfigTab::onReadySaveFile);
    this->connect(this, &SerialPortConfigTab::displaySavePathRequested, m_pSerialPortRealTimeSaveWidget,
                  &SerialPortRealTimeSaveWidget::onDisplaySavePath);
    this->connect(SerialPortManager::getInstance(), &SerialPortManager::receiveDataChanged,
                  [this](const QByteArray& data)
                  {
                      Qt::CheckState state = m_pSerialPortReceiveSettingsWidget->getSaveToFileCheckBox()->checkState();
                      if (state == Qt::Checked && m_pSaveFile)
                      {
                          QString dataStr = QString::fromUtf8(data);
                          // 如果数据不以换行符结尾，则添加换行符
                          if (!dataStr.endsWith('\n'))
                          {
                              dataStr.append('\n');
                          }
                          QTextStream out(m_pSaveFile);
                          out << dataStr;
                          m_pSaveFile->flush();
                      }
                  });
}
