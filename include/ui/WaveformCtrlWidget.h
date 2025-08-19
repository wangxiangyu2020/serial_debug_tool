/**
  ******************************************************************************
  * @file           : WaveformCtrlWidget.h
  * @author         : wangxiangyu
  * @brief          : None
  * @attention      : None
  * @date           : 2025/7/31
  ******************************************************************************
  */

#ifndef WAVEFORMCTRLWIDGET_H
#define WAVEFORMCTRLWIDGET_H

#include <QWidget>
#include "utils/StyleLoader.h"
#include <QHBoxLayout>
#include <QPushButton>
#include "ui/AddChannelDialog.h"
#include "ui/RemoveChannelDialog.h"
#include "ui/SampleRateDialog.h"
#include "core/ChannelManager.h"
#include <QRandomGenerator>

class AddChannelDialog;

class WaveformCtrlWidget : public QWidget
{
    Q_OBJECT

public:
    // 构造函数和析构函数
    explicit WaveformCtrlWidget(QWidget* parent = nullptr);
    ~WaveformCtrlWidget() = default;

signals:
    void requestAllChannels();
    void addChannelRequested(const QString& id, const QString& name, const QString& color);
    void clearAllChannelDataRequested();
    void importChannelsDataRequested();
    void exportChannelsDataRequested();
    void requestSampleRate();
    void sampleRateChanged(int rate);
    void startActionRequested();
    void stopActionRequested();

private slots:
    void onAddChannelBtnClicked();
    void onRemoveChannelBtnClicked();
    void onChannelsReceived(const QList<ChannelInfo>& channels);
    void onClearBtnClicked();
    void onImportBtnClicked();
    void onExportBtnClicked();
    void onSampleRateBtnClicked();
    void onSampleRateReceived(int rate);
    void onActionBtnClicked();
    void onStatusChanged(const QString& status);

private:
    enum class ChannelOperation
    {
        AddChannel,
        RemoveChannel
    };

    // 私有方法
    void setUI();
    void createComponents();
    void createLayout();
    void connectSignals();
    void setBtnStatus(bool actionClicked);

    void requestChannels(ChannelOperation operation);
    void processChannelsForAdd(const QList<ChannelInfo>& channels);
    void processChannelsForRemove(const QList<ChannelInfo>& channels);

    // 布局成员
    QHBoxLayout* m_pMainLayout = nullptr;

    // 按钮成员
    QPushButton* m_pAddChannelButton = nullptr;
    QPushButton* m_pRemoveChannelButton = nullptr;
    QPushButton* m_pClearButton = nullptr;
    QPushButton* m_pImportButton = nullptr;
    QPushButton* m_pExportButton = nullptr;
    QPushButton* m_pActionButton = nullptr;
    QPushButton* m_pSampleRateButton = nullptr;

    // 对话框成员
    ChannelOperation m_pendingOperation = ChannelOperation::AddChannel;
    AddChannelDialog* m_pAddChannelDialog = nullptr;
    RemoveChannelDialog* m_pRemoveChannelDialog = nullptr;
    SampleRateDialog* m_pSampleRateDialog = nullptr;

    bool m_actionClicked = false;
};

#endif //WAVEFORMCTRLWIDGET_H
