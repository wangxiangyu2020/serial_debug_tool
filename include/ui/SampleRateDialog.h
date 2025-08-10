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
    explicit SampleRateDialog(QWidget* parent = nullptr);

    int getSampleRate() const;
    void setSampleRate(int rate);

protected:
    void createComponents() override;
    void createContentLayout() override;
    void connectSignals() override;
    void onConfirmClicked() override;

private:
    QSpinBox* m_pSampleRateSpinBox = nullptr; // 采样率输入框

    void setUI();
};

#endif //SAMPLERATEDIALOG_H
