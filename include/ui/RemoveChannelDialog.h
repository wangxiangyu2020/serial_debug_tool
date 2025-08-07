/**
  ******************************************************************************
  * @file           : RemoveChannelDialog.h
  * @author         : wangxiangyu
  * @brief          : None
  * @attention      : None
  * @date           : 2025/8/7
  ******************************************************************************
  */

#ifndef REMOVECHANNELDIALOG_H
#define REMOVECHANNELDIALOG_H

#include "ui/CDialogBase.h"
#include "core/SerialPortManager.h"
#include <QListWidget>

class RemoveChannelDialog : public CDialogBase
{
    Q_OBJECT

public:
    explicit RemoveChannelDialog(QWidget* parent = nullptr);

    void setExistingChannels(const QList<ChannelInfo>& channels);

protected:
    void createComponents() override;
    void createContentLayout() override;
    void connectSignals() override;
    void onConfirmClicked() override;

private:
    void setUI();

private:
    QListWidget* m_pChannelListWidget = nullptr;
};

#endif //REMOVECHANNELDIALOG_H
