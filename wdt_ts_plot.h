#ifndef WDT_TS_PLOT_H
#define WDT_TS_PLOT_H

#include <QWidget>
#include "wdt_monitor_ch.h"
#include "format_trs.h"
#include "format_tmt.h"

namespace Ui {
class CWdtTsPlot;
}

class CWdtTsPlot : public QWidget
{
    Q_OBJECT
private:
    typedef enum update_type_n{
        TYPE_CURR = 0,
        TYPE_PREV,
        TYPE_NEXT,
    }UpdateType_S;


public:
    explicit CWdtTsPlot(QWidget *parent = 0);
    ~CWdtTsPlot();
    /* 利用SRS文件中的参数刷新页面显示，文件名为空则回到初始页面*/
    void UpdateShow(QString strTrsFileName);

private:
    void UpdateUiState();
    void ShowPlotByCheck(QCheckBox *chk, WdtMonitorCh *plot);
    void UpdateCmbStation();
    //检查输入的数据是否在允许范围内，不在这自动调整后返回正常值
    void CheckAndUpdatePrm(int &idxStart, int &nLen);
    //重新初始化设备列表和图形显示(一般是切换了SRS文件)
    void ResetDataAndShow();
    //根据采集站名，查找对应的时序数据，并读入内存
    bool ReadSmt(const QString strNi);
    //更新SMT的显示信息
    void UpdateStInfo();
    //更新显示起始点、显示长度信息，同时更新绘图页面
    void UpdatePrmAndPlot(UpdateType_S type);
    //根据SRS全路径文件名和采集时序的设备号求取SMT全路径文件名
    string GetSmtFileName(string file_srs, string dev_name);
    //根据选择的仪器名，读取时序数据并更新信息和绘图
    bool ReadSmtAndUpdateShow();

private slots:
    void onBtnChangeDir();
    void onBtnPrevClicked();
    void onBtnNextClicked();
//    void on_btnPrev_clicked();
//    void on_btnNext_clicked();
    void on_chkCH1_clicked(bool checked);
    void on_chkCH2_clicked(bool checked);
    void on_chkCH3_clicked(bool checked);
    void on_chkCH4_clicked(bool checked);
    void on_chkCH5_clicked(bool checked);
    void on_chkCH6_clicked(bool checked);
    void onCmbStationSel(int iSel);

private:
    Ui::CWdtTsPlot *ui;

    QVector<WdtMonitorCh*> m_VtWdtCh;
    QVector<QString> m_VtStationNi;
    CFormatTRS m_trs; //原始参数及数据
    CFormatTmt m_tmt;
    bool m_trs_inited;
};

#endif // WDT_TS_PLOT_H
