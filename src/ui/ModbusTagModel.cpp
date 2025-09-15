/**
  ******************************************************************************
  * @file           : ModbusTagModel.cpp
  * @author         : wangxiangyu
  * @brief          : Modbus点位表的 TableView 模型实现
  ******************************************************************************
  */
#include "ui/ModbusTagModel.h"

ModbusTagModel::ModbusTagModel(QList<ModbusTag>* tags, QObject* parent)
    : QAbstractTableModel(parent), m_tags(tags)
{
}

int ModbusTagModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid() || !m_tags) return 0;
    return m_tags->count();
}

int ModbusTagModel::columnCount(const QModelIndex& parent) const
{
    if (parent.isValid()) return 0;
    return 4; // 点位名称, 当前值, 十六进制, 原始值
}

QVariant ModbusTagModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    // 只处理用于显示的文本角色
    if (role != Qt::DisplayRole) return QVariant();

    // --- 核心修改点：区分水平和垂直表头 ---
    if (orientation == Qt::Horizontal)
    {
        // 如果是水平表头（列标题），则返回我们之前定义的列名
        switch (section)
        {
        case 0: return "点位名称";
        case 1: return "当前值";
        case 2: return "十六进制";
        case 3: return "原始值";
        default: return QVariant();
        }
    }
    else
    {
        // orientation == Qt::Vertical
        // 如果是垂直表头（行标题），则返回行号
        // section 是从0开始的行索引，我们+1使其从1开始显示
        return section + 1;
    }
}

QVariant ModbusTagModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || !m_tags || index.row() >= m_tags->size())
    {
        return QVariant();
    }

    if (role == Qt::DisplayRole)
    {
        const ModbusTag& tag = m_tags->at(index.row());
        switch (index.column())
        {
        case 0: return tag.name;
        case 1: return tag.currentValue.isValid() ? tag.currentValue : "N/A"; // 显示换算后的当前值
        case 2: // 将原始值显示为十六进制
            {
                if (!tag.rawValue.isValid()) return "N/A";
                bool ok;
                // 支持不同长度的原始值
                quint64 raw = tag.rawValue.toULongLong(&ok);
                return ok ? QString("0x%1").arg(raw, 4, 16, QChar('0')).toUpper() : "N/A";
            }
        case 3: return tag.rawValue.isValid() ? tag.rawValue : "N/A"; // 显示原始值
        default: return QVariant();
        }
    }
    return QVariant();
}

void ModbusTagModel::layoutRefresh()
{
    // beginResetModel/endResetModel 会通知所有连接的视图：数据已完全重置，请刷新所有内容
    this->beginResetModel();
    this->endResetModel();
}

void ModbusTagModel::valueUpdate(int rowIndex)
{
    if (rowIndex < 0 || rowIndex >= m_tags->size()) return;

    // 创建一个只包含已更改单元格的索引范围
    QModelIndex topLeft = index(rowIndex, 1); // "当前值"列
    QModelIndex bottomRight = index(rowIndex, 3); // "原始值"列

    // 发射 dataChanged 信号，只通知视图刷新指定区域，效率更高
    emit dataChanged(topLeft, bottomRight, {Qt::DisplayRole});
}
