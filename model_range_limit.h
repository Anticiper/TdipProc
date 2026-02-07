#ifndef MODEL_RANGE_LIMIT_H
#define MODEL_RANGE_LIMIT_H

#include <QAbstractTableModel>
#include <vector>
#include "data_limit.h"

using std::vector;

class CModelRangeLimit : public QAbstractTableModel
{
    Q_OBJECT
public:
    typedef enum ColType{
        COL_TYPE_MIN = 0, //最小值
        COL_TYPE_MAX,  //
        COL_TYPE_MAX_STD, //最大STD
        COL_TYPE_MAX_ERR, //最大相对误差
        COL_TYPE_CNT,
    }ColType_T;

public:
    explicit CModelRangeLimit(vector<LimitItem_S> *vt_limit, QObject *parent = 0);

    int     rowCount(const QModelIndex &parent = QModelIndex()) const;
    int     columnCount(const QModelIndex &parent = QModelIndex()) const;
    bool    setData(const QModelIndex &index, const QVariant &value, int role);
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;

private:
    bool CanEdit(const QModelIndex &index)const;

signals:

public slots:

private:
    vector<LimitItem_S> *m_vt_limit;
};

#endif // MODEL_RANGE_LIMIT_H
