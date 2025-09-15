/**
******************************************************************************
  * @file           : ModbusTagModel.h
  * @author         : wangxiangyu
  * @brief          : Modbus点位表的 TableView 模型
  ******************************************************************************
  */

#ifndef MODBUSTAGMODEL_H
#define MODBUSTAGMODEL_H

#include <QAbstractTableModel>
#include <QList>
#include "utils/ModbusTag.h"

class ModbusTagModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    // 构造函数接收一个指向真实数据源的指针
    explicit ModbusTagModel(QList<ModbusTag>* tags, QObject* parent = nullptr);

    // --- 必须重新实现的虚函数 ---
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

public slots:
    // 用于通知模型，点位配置列表已发生结构性变化（增/删/改）
    void layoutRefresh();
    // 用于通知模型，某个点位的实时值已更新
    void valueUpdate(int rowIndex);

private:
    QList<ModbusTag>* m_tags; // 指向外部数据源的指针
};

#endif // MODBUSTAGMODEL_H
