#ifndef WDT_TDIP_RES_H
#define WDT_TDIP_RES_H

#include <QWidget>
#include "wdt_tdip_res_space.h"
#include "wdt_tdip_res_table.h"
#include "dlg_limit_settings.h"
#include "format_trs.h"
#include "data_threshold.h"

namespace Ui {
class CWdtTdipRes;
}

class CWdtTdipRes : public QWidget
{
    Q_OBJECT

public:
    explicit CWdtTdipRes(QWidget *parent = 0);
    ~CWdtTdipRes();

    void UpdateShow(QString strTrsFileName);
    void SetTrsFileList(QStringList path_files_trs);

private:
    void InitUiShowTable();
    void InitUiShowPlotPos();
    void UpdatePassRate(); //更新统计的合格率
    void onUpdateShowResult();
    static void ReadCfgThreshold(CDataThreshold *threshold);
    static void WriteCfgThreshold(CDataThreshold *threshold);

protected slots:
    virtual void showEvent(QShowEvent * event);
    void onSaveResToFile();
    void onSwitchShowPage();

private slots:
    void on_btnErrSet_clicked();

    void on_btnSave_clicked();

private:
    Ui::CWdtTdipRes *ui;

    bool m_trs_inited;
    CWdtTdipResTable *m_wdt_res_table;
    CdlgLimitSettings *m_dlg_data_limit;
    CWdtTdipResSpace *m_wdt_res_plot_space;  //空间视图
    QVector<QWidget*> m_vt_wdt_show;
    QString m_str_path;
    //CDataThreshold m_threshold;
    CFormatTRS m_trs; //原始参数及数据
    QStringList m_path_files_trs;
    QString m_strTrsPathOutSingle;
    CDataThreshold m_threshold;
};

#endif // WDT_TDIP_RES_H
