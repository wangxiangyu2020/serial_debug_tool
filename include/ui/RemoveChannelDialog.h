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
    // 构造函数和析构函数
    explicit RemoveChannelDialog(QWidget* parent = nullptr);

    // 配置方法
    void setExistingChannels(const QList<ChannelInfo>& channels);

protected:
    // 重写基类虚函数
    void createComponents() override;
    void createContentLayout() override;
    void connectSignals() override;
    void onConfirmClicked() override;

private:
    // 私有方法
    void setUI();

    // UI组件成员
    QListWidget* m_pChannelListWidget = nullptr;
};

#endif //REMOVECHANNELDIALOG_H
