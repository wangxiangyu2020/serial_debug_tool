/**
  ******************************************************************************
  * @file           : AddChannelDialog.h
  * @author         : wangxiangyu
  * @brief          : None
  * @attention      : None
  * @date           : 2025/7/31
  ******************************************************************************
  */

#ifndef ADDCHANNELDIALOG_H
#define ADDCHANNELDIALOG_H

#include <QLineEdit>
#include <QComboBox>
#include <QLabel>
#include <QListWidget>
#include <QFrame>
#include <QHBoxLayout>
#include <QMap>
#include "CDialogBase.h"
#include "ui/CMessageBox.h"
#include "core/ChannelManager.h"

class AddChannelDialog : public CDialogBase
{
    Q_OBJECT

public:
    // 构造函数和析构函数
    explicit AddChannelDialog(QWidget* parent = nullptr);

    // 获取方法
    QString getChannelName() const;
    QString getChannelId() const;
    QString getChannelColor() const;

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
    QLabel* m_pNameLabel = nullptr;
    QLabel* m_pIdLabel = nullptr;
    QLabel* m_pColorLabel = nullptr;
    QLineEdit* m_pNameEdit = nullptr;
    QLineEdit* m_pIdEdit = nullptr;
    QComboBox* m_pColorCombo = nullptr;
    QLabel* m_pExistingLabel = nullptr;
    QListWidget* m_pChannelListWidget = nullptr;
};

#endif //ADDCHANNELDIALOG_H
