#ifndef WDT_PARAM_TRS_H
#define WDT_PARAM_TRS_H

#include <QWidget>
#include "format_trs.h"
#include "model_tdip_dev_lay.h"

namespace Ui {
class CWdtParamTrs;
}

class CWdtParamTrs : public QWidget
{
    Q_OBJECT

    typedef struct {
        QString strName; //显示的名称
        FreqType_N freq_type; //信号类型(多频/扩频)
        int harm_num; //信号阶数
    }SigTypeInfo_S;

public:
    explicit CWdtParamTrs(QWidget *parent = 0);
    ~CWdtParamTrs();

    /* 利用TRS文件中的参数刷新页面显示，文件名为空则回到初始页面*/
    void UpdateShow(QString strTrsFileName);
    void UpdateData(bool bSaveOrShow);
    void SetTrsFileList(QStringList path_files_trs);

private:
    void UpdateDataUninited();
    void UpdateCurrStationCmb();
    void UpdateCurrUiState();
    void UpdateDevLayoutShow(int idx_dev_curr, float first_pole_x, float mn_len);
    void UpdateTimeLabel(int sampleRate, double freq, int freqType);
    QString FormatValue(double value);
    //释放之前的内存，重新申请内存
    void ResetPrmDev(QVector<TdipPrmDev_S*> *vt_prm, int dev_num);
    /*获取设备索引，未找到时返回-1*/
    static int  GetDevIdx(QString strDevName, QVector<TdipPrmDev_S*> *vt_prm);
    void EnablePosB();
    bool ImportPrm(FILE *pFile);
    void OutputPrm(FILE *pFile);
    void UpdateToTrs();
    bool UpdateToAllTrs(CFormatTRS *srs);
    float GetCurrRatioN2O(const TdipPrmMeas_S *prm_new, const TdipPrmMeas_S *prm_old);
    void UpdateChInfoAndApp(CTdipResultCal *res);
    bool UpdateChInfoAndAppAll(CTdipResultCal *res, CFormatTRS *target_trs);
    bool UpdateParamToOtherTrs(CFormatTRS *target_trs);
    //装置系数计算
    float TdipPubCalK(int idx_dev_ch,TdipPrmMeas_S *prm_meas,TdipPrmDev_S *prm_dev, CResult::ChInfo_S ch_info);
    int  IndexOfSigInfo(FreqType_N freq_type, int harm_num);
    int GetIdxOf(const QVector<float> VtArray, float Value); //返回Value在数组中的索引
private slots:
    void onBtnSaveChange();
    void onBtnSaveResToTxt();
    void onUpdatePowerUiState();
    void onUpdateDevLayout();
    void onBtnImportPrm();
    void onBtnOutputPrm();
    void onPosBStateChanged();
    void onCmbFreqChanged();

    void on_btnSaveChangeThis_clicked();

    void on_btnSaveChangeAll_clicked();

private:
    Ui::CWdtParamTrs *ui;

    ModelTdipDevLay *m_model_dev_lay;

    bool m_trs_inited;
    CFormatTRS m_trs; //原始参数及数据
    QVector<TdipPrmDev_S*> m_vt_prm_dev;//与UI保持一致的布极参数(因需要检查参数是否修改所以使用副本)
    TdipPrmMeas_S m_prm_meas; //与UI保持一致的测量参数(因需要检查参数是否修改所以使用副本)
    QVector<SigTypeInfo_S*> m_cmb_sig_info;
    QVector<float> m_vt_sig_freq;
    QVector<float> m_vt_curr_resist;
    bool m_change_params;
    QStringList m_path_files_trs;
};

#endif // WDT_PARAM_TRS_H
