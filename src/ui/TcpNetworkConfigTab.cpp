/**
  ******************************************************************************
  * @file           : TcpNetworkConfigTab.cpp
  * @author         : wangxiangyu
  * @brief          : None
  * @attention      : None
  * @date           : 2025/8/13
  ******************************************************************************
  */
#include "ui/TcpNetworkConfigTab.h"


TcpNetworkConfigTab::TcpNetworkConfigTab(QWidget* parent)
    : QWidget(parent)
{
    this->setUI();
    StyleLoader::loadStyleFromFile(this, ":/resources/qss/tcp_network_config_tab.qss");
}

bool TcpNetworkConfigTab::eventFilter(QObject* watched, QEvent* event)
{
    if (event->type() == QEvent::Wheel)
    {
        if (auto* cb = qobject_cast<QComboBox*>(watched))
        {
            if (!cb->view()->isVisible())
            {
                return true;
            }
        }
    }
    return QWidget::eventFilter(watched, event);
}

void TcpNetworkConfigTab::onClientStateUpdated(bool displayTimestamp, bool hexDisplay, bool hexSend)
{
    m_clientState.displayTimestamp = displayTimestamp;
    m_clientState.hexDisplay = hexDisplay;
    m_clientState.hexSend = hexSend;
}

void TcpNetworkConfigTab::onServerStateUpdated(bool displayTimestamp, bool hexDisplay, bool hexSend)
{
    m_serverState.displayTimestamp = displayTimestamp;
    m_serverState.hexDisplay = hexDisplay;
    m_serverState.hexSend = hexSend;
}

void TcpNetworkConfigTab::setUI()
{
    this->setAttribute(Qt::WA_StyledBackground);
    this->createComponents();
    this->createLayout();
    this->connectSignals();
}

void TcpNetworkConfigTab::createComponents()
{
    m_pModeLabel = new QLabel(this);
    m_pModeLabel->setText("模式选择: ");
    m_pModeComboBox = new QComboBox(this);
    m_pModeComboBox->addItem("TCP 客户端");
    m_pModeComboBox->addItem("TCP 服务端");

    m_pClientWidget = new TcpNetworkClientWidget(this);

    m_pServerWidget = new TcpNetworkServerWidget(this);
    m_pServerWidget->hide();

    // 安装事件过滤器
    m_pModeComboBox->installEventFilter(this);
}

void TcpNetworkConfigTab::createLayout()
{
    m_pModeLayout = new QHBoxLayout();
    m_pModeLayout->addWidget(m_pModeLabel);
    m_pModeLayout->addWidget(m_pModeComboBox);
    m_pModeLayout->addStretch();

    m_pMainLayout = new QVBoxLayout(this);
    m_pMainLayout->addLayout(m_pModeLayout);
    m_pMainLayout->addWidget(m_pClientWidget);
    m_pMainLayout->addWidget(m_pServerWidget);
    m_pMainLayout->setContentsMargins(10, 15, 10, 10);
}

void TcpNetworkConfigTab::connectSignals()
{
    this->connect(m_pModeComboBox, &QComboBox::currentIndexChanged, [this](int index)
    {
        if (index == 0)
        {
            m_pClientWidget->show();
            m_pServerWidget->hide();
            m_pClientWidget->requestScriptReceiveCheckBox()->setChecked(false);
            emit m_pClientWidget->tcpNetworkClientScriptEnabled(false);
            emit applyClientState(m_clientState);
        }
        else if (index == 1)
        {
            m_pClientWidget->hide();
            m_pServerWidget->show();
            m_pServerWidget->requestScriptReceiveCheckBox()->setChecked(false);
            emit m_pServerWidget->tcpNetworkServerScriptEnabled(false);
            emit applyServerState(m_serverState);
        }
    });
    // 连接客户端状态更新
    this->connect(this, &TcpNetworkConfigTab::applyClientState, m_pClientWidget, &TcpNetworkClientWidget::onApplyState);
    this->connect(m_pClientWidget, &TcpNetworkClientWidget::stateChanged, this,
                  &TcpNetworkConfigTab::onClientStateUpdated);
    // 连接服务端状态更新
    this->connect(this, &TcpNetworkConfigTab::applyServerState, m_pServerWidget, &TcpNetworkServerWidget::onApplyState);
    this->connect(m_pServerWidget, &TcpNetworkServerWidget::stateChanged, this,
                  &TcpNetworkConfigTab::onServerStateUpdated);
}
