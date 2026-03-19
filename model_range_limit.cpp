#include <assert.h>
#include <QColor>
#include "model_range_limit.h"
#include "data_limit.h"
#include "tdip_def.h"

CModelRangeLimit::CModelRangeLimit(vector<LimitItem_S> *vt_limit, QObject *parent) :
    QAbstractTableModel(parent)
{
    assert(vt_limit != NULL);
    m_vt_limit = vt_limit;
}

int CModelRangeLimit::rowCount(const QModelIndex &parent) const
{
    return m_vt_limit->size();
}

int CModelRangeLimit::columnCount(const QModelIndex &parent) const
{
    return 2;
}

bool CModelRangeLimit::setData(const QModelIndex &index, const QVariant &value, int role)
{
     Qt::ItemFlags FlagsRet = QAbstractItemModel::flags(index);
    if(!index.isValid())return FlagsRet;
    int col = index.column();
    int row = index.row();
    bool bRet = false;
    float fValue;
    LimitItem_S *item = &((*m_vt_limit)[row]);
    FloatItem_S *float_item = NULL;
    if(role == Qt::EditRole){
        switch(col){
        case  COL_TYPE_MIN:
            float_item = &item->min;
            break;
        case  COL_TYPE_MAX:
            float_item = &item->max;
            break;
//        case  COL_TYPE_MAX_STD:
//            float_item = &item->max_err_abs;
//            break;
        case  COL_TYPE_MAX_ERR:
            float_item = &item->max_err_cp;
            break;
        default:
            assert(0);
            break;
        }
        float_item->value = value.toFloat();
        bRet = true;
    }
    return bRet;
}

QVariant CModelRangeLimit::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();
    int col = index.column();
    int row = index.row();
    QVariant ret;

    LimitItem_S item = m_vt_limit->at(row);
    FloatItem_S float_item;
    if(role == Qt::DisplayRole || role == Qt::EditRole){
        switch (col) {
        case  COL_TYPE_MIN:
            float_item = item.min;
            break;
        case  COL_TYPE_MAX:
            float_item = item.max;
            break;
//        case  COL_TYPE_MAX_STD:
//            float_item = item.max_err_abs;
//            break;
        case  COL_TYPE_MAX_ERR:
            float_item = item.max_err_cp;
            break;
        default:
            assert(0);
            break;
        }
        if(float_item.valid){
            ret = tr("").sprintf("%g", float_item.value);
        }
    }else if(role == Qt::BackgroundColorRole){
        if(!CanEdit(index))
            return QColor(240,240,240);
    }else if(role==Qt::TextAlignmentRole){
        return (int)(Qt::AlignCenter);
    }
    return ret;
}

QVariant CModelRangeLimit::headerData(int section, Qt::Orientation orientation, int role) const
{
    QVariant Var;

    assert(m_vt_limit->size() == LIMIT_TYPE_CNT);
    if(role == Qt::DisplayRole){
        if (orientation == Qt::Horizontal){
            switch(section){
            case  COL_TYPE_MIN:
                Var = tr("最小值");
                break;
            case  COL_TYPE_MAX:
                Var = tr("最大值");
                break;
//            case  COL_TYPE_MAX_STD:
//                Var = tr("最大STD");
//                break;
            case  COL_TYPE_MAX_ERR:
                Var = tr("最大相对误差(%)");
                break;
            default:
                break;
            }
        }else{
            switch(section){
            case LIMIT_TYPE_APP:
                Var = tr("视电阻率(Ω·m)");
                break;
            case LIMIT_TYPE_APPC:
                Var = tr("M(mV/V)");
                break;
            default:
                break;
            }
        }
    }
    return Var;
}

Qt::ItemFlags CModelRangeLimit::flags(const QModelIndex &index) const
{
    Qt::ItemFlags FlagsRet = QAbstractItemModel::flags(index);
    bool bEdit = false;

    bEdit = CanEdit(index);
    if(bEdit){
        FlagsRet |= Qt::ItemIsEditable;
    }
    return FlagsRet;
}

bool CModelRangeLimit::CanEdit(const QModelIndex &index)const
{
    int col = index.column();
    int row = index.row();
    bool bEdit = true;

    LimitItem_S item = m_vt_limit->at(row);
    if(!index.isValid())
        return false;
    switch (col) {
    case  COL_TYPE_MIN:
        bEdit = item.min.valid;
        break;
    case  COL_TYPE_MAX:
        bEdit = item.max.valid;
        break;
//    case  COL_TYPE_MAX_STD:
//        bEdit = item.max_err_abs.valid;
//        break;
    case  COL_TYPE_MAX_ERR:
        bEdit = item.max_err_cp.valid;
        break;
    default:
        break;
    }
    return bEdit;
}
