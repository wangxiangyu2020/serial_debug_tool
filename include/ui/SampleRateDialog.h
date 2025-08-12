/**
******************************************************************************
  * @file           : AddChannelDialog.cpp
  * @author         : wangxiangyu
  * @brief          : None
  * @attention      : None
  * @date           : 2025/8/10
  ******************************************************************************
 */

#ifndef SAMPLERATEDIALOG_H
#define SAMPLERATEDIALOG_H

#include "ui/CDialogBase.h"
#include <QSpinBox>

class SampleRateDialog : public CDialogBase
{
    Q_OBJECT

public:
    // 构造函数和析构函数
    explicit SampleRateDialog(QWidget* parent = nullptr);

    // 获取方法
    int getSampleRate() const;

    // 配置方法
    void setSampleRate(int rate);

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
    QSpinBox* m_pSampleRateSpinBox = nullptr;
};

#endif //SAMPLERATEDIALOG_H
