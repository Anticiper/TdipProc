#ifndef MODEL_TDIP_RES_H
#define MODEL_TDIP_RES_H

#include <QAbstractTableModel>
#include <QVector>
#include "format_trs.h"
#include "data_threshold.h"

class ModelTdipRes : public QAbstractTableModel
{
    Q_OBJECT
public:
    typedef enum ColType{
        COL_TYPE_DEV_NAME = 0,
        COL_TYPE_CH_TYPE,
        COL_TYPE_CH_POS_M,
        COL_TYPE_ACQ_STATE,
        COL_TYPE_V1,
        COL_TYPE_V2,
        COL_TYPE_V3,
        COL_TYPE_V4,
        COL_TYPE_VOLT_POSITIVE,
        COL_TYPE_VOLT_POSITIVE_ERR,
        COL_TYPE_VOLT_NEGATIVE,
        COL_TYPE_VOLT_NEGATIVE_ERR,
        COL_TYPE_APPR,//视电阻率
        COL_TYPE_APPR_ERR,//视电阻率误差
        COL_TYPE_APPC,//视极化率
        COL_TYPE_APPC_ERR,//视极化率误差
        COL_TYPE_CNT,
    }TdipColType_T;

public:
    explicit ModelTdipRes(QObject *parent = 0);

    void SetGeoMeas(CFormatTRS *geo_meas);
    void SetThreshold(CDataThreshold *threshold);
    void ClearData();
    void    SetColItemsDisplay(const QVector<TdipColType_T>& items);
    void    SetColItemDisplay(TdipColType_T item, bool display = true); //设置项目是否显示(有顺序)
    const   QVector<TdipColType_T>& GetColItems(void) const;
    static  QVector<TdipColType_T> GetDefaultColItems();
    static  QVector<TdipColType_T> GetShowColItems();

    int     rowCount(const QModelIndex &parent = QModelIndex()) const;
    int     columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;

private:
    TdipColType_T GetColType(int column)const ;

signals:

public slots:

private:
    QVector<TdipColType_T> m_display_items;
    CFormatTRS *m_geo_meas;
    CDataThreshold *m_threshold; //误差限制
};

#endif // MODEL_TDIP_RES_H
