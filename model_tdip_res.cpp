#include <QColor>
#include <assert.h>
#include "model_tdip_res.h"
#include "algo_pub.h"

static QColor  g_color_text_err = QColor(176,23,31);//QColor(Qt::darkRed);
static QColor  g_color_background_overflow = QColor(245,222,179); //淡黄色

ModelTdipRes::ModelTdipRes(QObject *parent) :
    QAbstractTableModel(parent)
{
    m_geo_meas = NULL;
}

void ModelTdipRes::SetGeoMeas(CFormatTRS *geo_meas)
{
    m_geo_meas = (CFormatTRS*)geo_meas;
}

void ModelTdipRes::ClearData()
{
    m_geo_meas = NULL;
}

void ModelTdipRes::SetThreshold(CDataThreshold *threshold)
{
    m_threshold = threshold;
}

void ModelTdipRes::SetColItemsDisplay(const QVector<TdipColType_T> &items)
{
    m_display_items = items;
}

void ModelTdipRes::SetColItemDisplay(TdipColType_T item, bool display)
{
    if(display){
        if(!m_display_items.contains(item))
            m_display_items.push_back(item);
    } else {
        if(m_display_items.contains(item)){
            m_display_items.remove(m_display_items.indexOf(item));
        }
    }
}

QVector<ModelTdipRes::TdipColType_T> ModelTdipRes::GetDefaultColItems()
{
    QVector<ModelTdipRes::TdipColType_T> items = {
        COL_TYPE_DEV_NAME,
        COL_TYPE_CH_TYPE,
        COL_TYPE_CH_POS_M,
        COL_TYPE_ACQ_STATE,
        COL_TYPE_V1,
        COL_TYPE_V2,
        COL_TYPE_V3,
        COL_TYPE_V4,
        COL_TYPE_APPR,
        COL_TYPE_APPC
    };
    return items;
}

QVector<ModelTdipRes::TdipColType_T> ModelTdipRes::GetShowColItems()
{
    QVector<ModelTdipRes::TdipColType_T> items = {
        COL_TYPE_DEV_NAME,
        COL_TYPE_CH_TYPE,
        COL_TYPE_CH_POS_M,
        COL_TYPE_ACQ_STATE,
        COL_TYPE_VOLT_POSITIVE,
        COL_TYPE_VOLT_POSITIVE_ERR,
        COL_TYPE_VOLT_NEGATIVE,
        COL_TYPE_VOLT_NEGATIVE_ERR,
        COL_TYPE_APPR,
        COL_TYPE_APPR_ERR,
        COL_TYPE_APPC,
        COL_TYPE_APPC_ERR
    };
    return items;
}

int ModelTdipRes::rowCount(const QModelIndex &parent) const
{
    int row = 0;
    if(m_geo_meas != NULL)
        row = m_geo_meas->GetResultCal()->GetChNum();
    return row;
}

int ModelTdipRes::columnCount(const QModelIndex &parent) const
{
    return m_display_items.size();
}

QVariant ModelTdipRes::data(const QModelIndex &index, int role) const
{
    QVariant ret = QVariant();
    if(!index.isValid())return ret;
    int row = index.row();
    int idx_ch_res = row;
    int cal_num;
    QString strTmp;

    TdipColType_T col_type = GetColType(index.column());
    CResult::ChInfo_S ch_info;
    CTdipResultCal::ResItem_S res_item;

    if(m_geo_meas == NULL)
        return QVariant();

    CTdipResultCal *res =  m_geo_meas->GetResultCal();
    cal_num = m_geo_meas->GetRes()->DoGetBuffCalNum();
    ch_info = res->GetChInfo(idx_ch_res);
    res_item = res->GetResItem(idx_ch_res, 0, 0);

    if(role == Qt::DisplayRole){
        ret = tr("");
        switch(col_type){
        case COL_TYPE_DEV_NAME:
            strTmp = tr("%1").arg(ch_info.dev_name.data());
            strTmp += tr("_%1").arg(ch_info.idx_dev_ch+1);
            ret = strTmp;
            break;
        case COL_TYPE_CH_TYPE:
            if(ch_info.ch_type == CH_TYPE_E){
                strTmp = tr("ex%1").arg(ch_info.idx_seq+1);
            }else if(ch_info.ch_type == CH_TYPE_H){
                strTmp = tr("hy");
            }else{
                strTmp = tr("invalid");
            }
            ret = strTmp;
            break;
        case COL_TYPE_CH_POS_M:
            ret = tr("").sprintf("%.1f", ch_info.pos_m);
            break;
        case COL_TYPE_ACQ_STATE:
            ret = tr("已计算%1次").arg(cal_num);
            break;
        case COL_TYPE_V1:
            if(cal_num > 0)
                ret = tr("").sprintf("%.3f", res_item.u1);
            break;
        case COL_TYPE_V2:
            if(cal_num > 0)
                ret = tr("").sprintf("%.3f", res_item.u2);
            break;
        case COL_TYPE_V3:
            if(cal_num > 0)
                ret = tr("").sprintf("%.3f", res_item.u3);
            break;
        case COL_TYPE_V4:
            if(cal_num > 0)
                ret = tr("").sprintf("%.3f", res_item.u4);
            break;
        case COL_TYPE_VOLT_POSITIVE:
            if(cal_num > 0)
                ret = tr("").sprintf("%.3f", res_item.v_positive);
            break;
        case COL_TYPE_VOLT_POSITIVE_ERR:
            if(cal_num > 1)
                ret = tr("").sprintf("%.3f", res_item.positive_err);
            break;
        case COL_TYPE_VOLT_NEGATIVE:
            if(cal_num > 0)
                ret = tr("").sprintf("%.3f", res_item.v_negative);
            break;
        case COL_TYPE_VOLT_NEGATIVE_ERR:
            if(cal_num > 1)
                ret = tr("").sprintf("%.3f", res_item.negative_err);
            break;
        case COL_TYPE_APPR:
            if(cal_num > 0)
                ret = tr("").sprintf("%.3f", res_item.appr_value);
            break;
        case COL_TYPE_APPR_ERR:
            if(cal_num > 1)
                ret = tr("").sprintf("%.3f", res_item.appr_err_value);
            break;
        case COL_TYPE_APPC:
            if(cal_num > 0)
                ret = tr("").sprintf("%.3f", res_item.appc_value);
            break;
        case COL_TYPE_APPC_ERR:
            if(cal_num > 1)
                ret = tr("").sprintf("%.3f", res_item.appc_err_value);
            break;
        default:
            break;
        }
    }else if(role == Qt::TextAlignmentRole){
        ret = Qt::AlignCenter;
    }else if(role == Qt::ForegroundRole){//字体颜色
        switch(col_type){
        case COL_TYPE_APPR_ERR:
            if(cal_num > 1){
                if(!m_threshold->IsQualifiedData(res_item.appr_value, res_item.appr_err_value,
                                                CDataThreshold::DATA_TYPE_APP))
                    return g_color_text_err;
            }
            break;
        case COL_TYPE_APPC_ERR:
            if(cal_num > 1){
                if(!m_threshold->IsQualifiedData(res_item.appc_value, res_item.appc_err_value,
                                                CDataThreshold::DATA_TYPE_APPC))
                    return g_color_text_err;
            }
            break;
        }
    }else if(role == Qt::BackgroundColorRole){ //背景色
        if(res_item.bad){
            return g_color_background_overflow;
        }
    }
    return ret;
}

QVariant ModelTdipRes::headerData(int section, Qt::Orientation orientation, int role) const
{
    QVariant Var;
    TdipColType_T col_type;
    if(role == Qt::DisplayRole){
        if(orientation == Qt::Horizontal){
            col_type = GetColType(section);
            switch(col_type){
            case COL_TYPE_DEV_NAME:
                Var = tr("仪器_通道");
                break;
            case COL_TYPE_CH_TYPE:
                Var = tr("通道ID");
                break;
            case COL_TYPE_CH_POS_M:
                Var = tr("M位置(m)");
                break;
            case COL_TYPE_ACQ_STATE:
                Var = tr("采集进度");
                break;
            case COL_TYPE_V1:
                Var = tr("U+(mV)");
                break;
            case COL_TYPE_V2:
                Var = tr("△U+(mV)");
                break;
            case COL_TYPE_V3:
                Var = tr("U-(mV)");
                break;
            case COL_TYPE_V4:
                Var = tr("△U-(mV)");
                break;
            case COL_TYPE_VOLT_POSITIVE:
                Var = tr("一次场电压(mV)");
                break;
            case COL_TYPE_VOLT_POSITIVE_ERR:
                Var = tr("一次场误差(%)");
                break;
            case COL_TYPE_VOLT_NEGATIVE:
                Var = tr("二次场电压(mV)");
                break;
            case COL_TYPE_VOLT_NEGATIVE_ERR:
                Var = tr("二次场误差(%)");
                break;
            case COL_TYPE_APPR:
                Var = tr("视电阻率(Ω·m)");
                break;
            case COL_TYPE_APPR_ERR:
                Var = tr("视电阻率误差(%)");
                break;
            case COL_TYPE_APPC:
                Var = tr("视极化率(%)");
                break;
            case COL_TYPE_APPC_ERR:
                Var = tr("视极化率误差(%)");
                break;
            default:
                break;
            }
        }
    }
    return Var;
}

Qt::ItemFlags ModelTdipRes::flags(const QModelIndex &index) const
{
    Qt::ItemFlags FlagsRet = QAbstractItemModel::flags(index);
    return FlagsRet;
}

ModelTdipRes::TdipColType_T ModelTdipRes::GetColType(int column) const
{
    assert(column>=0 && column<m_display_items.size());
    return m_display_items[column];
}

const QVector<ModelTdipRes::TdipColType_T> &ModelTdipRes::GetColItems() const
{
    return m_display_items;
}
