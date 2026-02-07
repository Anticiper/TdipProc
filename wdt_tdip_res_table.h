#ifndef WDT_TDIP_RES_TABLE_H
#define WDT_TDIP_RES_TABLE_H

#include <QWidget>
#include "format_trs.h"
#include "model_tdip_res.h"
#include "data_threshold.h"

namespace Ui {
class CWdtTdipResTable;
}

class CWdtTdipResTable : public QWidget
{
    Q_OBJECT

public:
    explicit CWdtTdipResTable(QWidget *parent = 0);
    ~CWdtTdipResTable();

    void SetGeoMeas(CFormatTRS *geo_meas);
    void Init(CDataThreshold *threshold);
    void onUpdateShowResult();//更新计算结果的显示
    void Clear();
private:
    Ui::CWdtTdipResTable *ui;

    ModelTdipRes *m_model_res;
    CFormatTRS *m_geo_meas;
};

#endif // WDT_TDIP_RES_TABLE_H
