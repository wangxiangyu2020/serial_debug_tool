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

struct ChannelInfo
{
    QString name;
    QString color;

    ChannelInfo() = default;

    ChannelInfo(const QString& n, const QString& c) : name(n), color(c)
    {
    }
};

class AddChannelDialog : public CDialogBase
{
    Q_OBJECT

public:
    explicit AddChannelDialog(QWidget* parent = nullptr);

    QString getChannelName() const;
    QString getChannelColor() const;
    void setExistingChannels(const QList<ChannelInfo>& channels);

protected:
    void createComponents() override;
    void createContentLayout() override;
    void connectSignals() override;
    void onConfirmClicked() override;

private:
    void setUI();
    QString getColorHex(const QString& colorName) const;

private:
    QLabel* m_pNameLabel = nullptr;
    QLabel* m_pColorLabel = nullptr;
    QLineEdit* m_pNameEdit = nullptr;
    QComboBox* m_pColorCombo = nullptr;
    // 已添加通道列表
    QLabel* m_pExistingLabel = nullptr;
    QListWidget* m_pChannelListWidget = nullptr;
};

#endif //ADDCHANNELDIALOG_H
