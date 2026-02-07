#ifndef WDT_MAIN_H
#define WDT_MAIN_H

#include <QWidget>
#include <QFileDialog>
#include <QStringListModel>
#include "format_trs.h"
#include "wdt_out_put.h"
#include "wdt_gps_pos.h"
#include "wdt_param_trs.h"
#include "wdt_tdip_res.h"
#include "wdt_ts_plot.h"

namespace Ui {
class CWdtMain;
}

class CWdtMain : public QWidget
{
    Q_OBJECT

public:
    explicit CWdtMain(QWidget *parent = 0);
    ~CWdtMain();

private:
    static void UpdateFilesFromPathFiles(const QStringList &paths_files, QStringList &files); //根据带路径的文件名更新文件名
    void UpdateGrpFileTitle();

private slots:
    void onBtnImportData();
    void onBtnClearData();
    void onListClicked(const QModelIndex &index);
    //删除位置信息
    void ClearPos(QVector<CItemGpsPos*> *vt_pos);
    //读取文件
    bool ReadTrs(const QString strTrsFileName, CFormatTRS *trs);
    //从TRS中获取位置信息(不负责清空vt_pos中原始已存储的位置信息）
    void GetPosFromTrs(CFormatTRS *trs, QVector<CItemGpsPos *> *vt_pos);
    //判断采集站电极是否沿测线反方向放置
    bool IsChReverse(const TdipPrmDev_S *prm);
    //获取第1个有效通道的位置,无有效道时返回-1
    //reverse=true表示从反方向查找
    int GetFirstValidChIdx(const TdipPrmDev_S *prm, bool reverse);
private:
    Ui::CWdtMain *ui;

    CWdtOutPut *m_wdt_output;
    CWdtParamTrs *m_wdt_prm_trs;
    CWdtGpsPos *m_wdt_gps_pos;
    CWdtTdipRes *m_wdt_tdip_res;
    CWdtTsPlot *m_wdt_ts_plot;

    QStringList m_path_files_trs; //带路径的文件名
    QStringListModel *m_model_files;

    QVector<CItemGpsPos*> m_vt_pos;
};

#endif // WDT_MAIN_H
