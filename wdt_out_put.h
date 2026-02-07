#ifndef WDT_OUT_PUT_H
#define WDT_OUT_PUT_H

#include <QWidget>
#include <QVector>
#include <QButtonGroup>
#include "sip_pub.h"
#include "format_trs.h"
#include "format_txt_shifting.h"
#include "data_limit.h"
#include "tdip_def.h"
#include "tdip_data_out.h"
#include "qt_pub.h"
#include "format_txt_coord.h"
#include "format_txt_site_coord.h"

namespace Ui {
class CWdtOutPut;
}

class CWdtOutPut : public QWidget
{
    Q_OBJECT

public:
    explicit CWdtOutPut(QStringList *file_names_srs, QWidget *parent = 0);
    ~CWdtOutPut();

    void ClearData();//清空数据
    void UpdateData(bool bSaveOrShow);
    void ResetPoleCmp();//通过获取第一个TRS文件的装置类型，来设置UI的二维测深极距是否能组合（任意装置不能组合）

private:
    void InitLimitData();
    void InitLimitTable();
    void onDataOut();
    void onPassRateOut();
    bool IsNeedCompseCh();
    //根据文件名对数据进行分类(将同装置多次供电的数据放到一个列表中)
     int ClassifyFileName(const QStringList *list_files,  //待分类的数据
                                 QVector<QStringList> *vt_files); //分类后的数据
     //获取装置类型
    SipLayout_T GetSipLayout();
    //返回第1个TRS文件的装置类型，无则默认返回四极测深装置
    SipLayout_T GetTrsLayout()const;
    //获取TRS的路径(打开的第1个TRS文件路径)
    QString GetFirstTrsDir()const;
    //将res_arr数组融合后输出到res_dest中(所有待融合数据的极距和AB距相同)
    void MergeRes(int arr_num, CFormatTRS res_arr[], CFormatTRS *res_dest, bool needCheckPosB);
    //将两个结果合为一个结果，存放在item_dest中
    void MergeResItem(CTdipResultCal::ResItem_S &item_src,
                                  CTdipResultCal::ResItem_S &item_dest);
private slots:
    void onUpdateUiShow();

    void onUpdateMaxErrValue();

    void on_btnOutData_2_clicked();

    void on_btnOutPassRate_2_clicked();

    void on_btnImportFileCoord_2_clicked();

    void on_btnImportFileElev_2_clicked();

    void on_btnImportFileShifting_2_clicked();

private:
    Ui::CWdtOutPut *ui;
    QStringList *m_file_names_trs;
    QString m_current_choose_trs;//当前选则的SRS文件，用于smt时序源重计算导出
    SipLayout_T m_first_layout;//首TRS文件的装置类型

    CFormatTxtShifting m_shifting;
    CFormatTxtElev m_elev;
    CFormatTxtCoord m_coord;
    CFormatTxtSiteCoord m_site_coord;

    QButtonGroup *m_grpOutDataType;
    QButtonGroup *m_grpSectOrInv;
    QButtonGroup *m_grpInvType, *m_grpTdipPrm, *m_grpPoleCmp, *m_grpInvApp;
    QButtonGroup *m_grpType;

    vector<LimitItem_S> m_vt_limit;
    QString m_strPathOut;
    QString m_strPassRatePathOut;
};

#endif // WDT_OUT_PUT_H
