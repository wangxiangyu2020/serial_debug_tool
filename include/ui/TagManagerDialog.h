/**
  ******************************************************************************
  * @file           : TagManagerDialog.h
  * @author         : wangxiangyu
  * @brief          : None
  * @attention      : None
  * @date           : 2025/9/13
  ******************************************************************************
  */

#ifndef TAGMANAGERDIALOG_H
#define TAGMANAGERDIALOG_H

#include <QDialog>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QMouseEvent>
#include "utils/StyleLoader.h"
#include "utils/ModbusTag.h"
#include "ui/AddEditModbusTagDialog.h"
#include "ui/CMessageBox.h"
#include <QHBoxLayout>

class TagManagerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TagManagerDialog(QList<ModbusTag>& tags, QWidget* parent = nullptr);
    ~TagManagerDialog() = default;

protected:
    void mousePressEvent(QMouseEvent* event) override;

private slots:
    void onAddButtonClicked();
    void onEditButtonClicked();
    void onDeleteButtonClicked();

private:
    void setUI();
    void createComponents();
    void createLayout();
    void connectSignals();
    void populateList();
    QString dataTypeToString(ModbusTag::DataType type);

    QVBoxLayout* m_pMainLayout = nullptr;

    QList<ModbusTag>& m_tags; // 对外部主数据列表的引用
    QListWidget* m_pListWidget = nullptr;
    QPushButton* m_pAddButton = nullptr;
    QPushButton* m_pEditButton = nullptr;
    QPushButton* m_pDeleteButton = nullptr;
    QPushButton* m_pSaveButton = nullptr;
};

#endif //TAGMANAGERDIALOG_H
