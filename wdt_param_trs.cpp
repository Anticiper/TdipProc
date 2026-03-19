#include <assert.h>
#include <QMessageBox>
#include <QFileDialog>
#include "wdt_param_trs.h"
#include "ui_wdt_param_trs.h"
#include "algo_pub.h"
#include "item_cmb.h"
#include "ex_code.h"
#include "qt_pub.h"
#include "tdip_pub.h"
#include "algo_geo.h"

#define   FILE_TDPM_VER   1

CWdtParamTrs::CWdtParamTrs(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CWdtParamTrs)
{
    ui->setupUi(this);
    m_trs_inited = false;
    m_change_params = true;

    CItemCmb *item_cmb = new CItemCmb(this);
    m_model_dev_lay = new ModelTdipDevLay(&m_vt_prm_dev, this);
    ui->tbDevLay->setItemDelegateForColumn(ModelTdipDevLay::COL_TYPE_CH_TYPE, item_cmb);
    ui->tbDevLay->setModel(m_model_dev_lay);
    //ui->tbDevLay->resizeColumnsToContents();
    ui->tbDevLay->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    QButtonGroup *btnGrp = new QButtonGroup(this);
    btnGrp->addButton(ui->rdCurrStationName);
    btnGrp->addButton(ui->rdCurrValue);

    ui->cmbSipLayout->addItem(tr("中梯装置"));
    ui->cmbSipLayout->addItem(tr("三极装置"));
    ui->cmbSipLayout->addItem(tr("四极装置"));

    m_cmb_sig_info.append(new SigTypeInfo_S{tr("方波"),FREQ_TYPE_SQUARE,0});
    m_cmb_sig_info.append(new SigTypeInfo_S{tr("过零方波-25%占空"),FREQ_TYPE_LINE,1});
    m_cmb_sig_info.append(new SigTypeInfo_S{tr("过零方波-50%占空"),FREQ_TYPE_LINE,2});
    m_cmb_sig_info.append(new SigTypeInfo_S{tr("过零方波-75%占空"),FREQ_TYPE_LINE,3});
    for(int i=0; i<m_cmb_sig_info.size(); i++){
        ui->cmbSigType->addItem(m_cmb_sig_info.at(i)->strName);
    }
    ui->cmbSigType->setCurrentIndex(0);
    /* 初始化电流传感器参数 */
    ui->cmbCurrResist->addItem(tr("12.5mV/A"));
    m_vt_curr_resist.append(0.0125);
    ui->cmbCurrResist->addItem(tr("25mV/A"));
    m_vt_curr_resist.append(0.025);
    ui->cmbCurrResist->addItem(tr("62.5mV/A"));
    m_vt_curr_resist.append(0.0625);
    ui->cmbCurrResist->addItem(tr("125mV/A"));
    m_vt_curr_resist.append(0.125);
    ui->cmbCurrResist->addItem(tr("1000mV/A"));
    m_vt_curr_resist.append(1);
    ui->cmbCurrResist->setCurrentIndex(1);
    /* 初始化软件支持的频率值 */
    ui->cmbSigBaseFreq->addItem(tr("1/256"));
    m_vt_sig_freq.append(1.0/256);
    ui->cmbSigBaseFreq->addItem(tr("1/128"));
    m_vt_sig_freq.append(1.0/128);
    ui->cmbSigBaseFreq->addItem(tr("1/64"));
    m_vt_sig_freq.append(1.0/64);
    ui->cmbSigBaseFreq->addItem(tr("1/32"));
    m_vt_sig_freq.append(1.0/32);
    ui->cmbSigBaseFreq->addItem(tr("1/16"));
    m_vt_sig_freq.append(1.0/16);
    ui->cmbSigBaseFreq->addItem(tr("1/8"));
    m_vt_sig_freq.append(1.0/8);
    ui->cmbSigBaseFreq->addItem(tr("1/4"));
    m_vt_sig_freq.append(1.0/4);
    ui->cmbSigBaseFreq->addItem(tr("1/2"));
    m_vt_sig_freq.append(1.0/2);
    ui->cmbSigBaseFreq->addItem(tr("1"));
    m_vt_sig_freq.append(1.0);
    ui->cmbSigBaseFreq->addItem(tr("2"));
    m_vt_sig_freq.append(2);
    ui->cmbSigBaseFreq->addItem(tr("4"));
    m_vt_sig_freq.append(4);
    ui->cmbSigBaseFreq->addItem(tr("8"));
    m_vt_sig_freq.append(8);
    ui->cmbSigBaseFreq->addItem(tr("16"));
    m_vt_sig_freq.append(16);
    ui->cmbSigBaseFreq->addItem(tr("32"));
    m_vt_sig_freq.append(32);
    ui->cmbSigBaseFreq->addItem(tr("64"));
    m_vt_sig_freq.append(64);
    ui->cmbSigBaseFreq->addItem(tr("128"));
    m_vt_sig_freq.append(128);
    ui->cmbSigBaseFreq->setCurrentIndex(4);

    UpdateShow(tr(""));

    ui->cmbSigType->setEnabled(false);
    ui->cmbSigBaseFreq->setEnabled(false);
    ui->rdCurrStationName->setEnabled(false);
    ui->rdCurrValue->setEnabled(false);
    ui->cmbCurrStationName->setEnabled(false);
    ui->edtFileName->setEnabled(false);

    connect(ui->btnSaveChange, SIGNAL(clicked()), this, SLOT(onBtnSaveChange()));
    connect(ui->edtMN, SIGNAL(textChanged(QString)), this, SLOT(onUpdateDevLayout()));
    connect(ui->edtDistLine2AB, SIGNAL(textChanged(QString)), this, SLOT(onUpdateDevLayout()));
    connect(ui->edtPosFirstPole, SIGNAL(textChanged(QString)), this, SLOT(onUpdateDevLayout()));
    connect(ui->cmbSipLayout, SIGNAL(currentIndexChanged(int)), this, SLOT(onUpdatePowerUiState()));
    connect(ui->btnImportPrm, SIGNAL(clicked()), this, SLOT(onBtnImportPrm()));
    connect(ui->btnOutputPrm, SIGNAL(clicked()), this, SLOT(onBtnOutputPrm()));
    connect(ui->btnOutputResToTxt, SIGNAL(clicked()), this, SLOT(onBtnSaveResToTxt()));
    connect(ui->cb_posB,SIGNAL(stateChanged(int)), this, SLOT(onPosBStateChanged()));
    connect(ui->cmbSigBaseFreq, SIGNAL(currentIndexChanged(int)), this, SLOT(onCmbFreqChanged()));

    ui->btnOutputResToTxt->setHidden(true);
}

CWdtParamTrs::~CWdtParamTrs()
{
    delete ui;
}

void CWdtParamTrs::UpdateShow(QString strTrsFileName)
{
    int i, num;
    string strFile = ex_u2g(strTrsFileName.toStdString());
    if(strTrsFileName.isEmpty()){
        m_trs_inited = false;
    }else{
        if(!m_trs.ReadFile(strFile.data())){
            QMessageBox::warning(this, tr("提示"), tr("打开文件“%1”失败！").arg(strTrsFileName));
            m_trs_inited = false;
        }else{
            m_prm_meas = *m_trs.GetPrmMeas();
            num = m_trs.GetDevNum();
            ResetPrmDev(&m_vt_prm_dev, num);
            for(i=0; i<num; i++){
                (*m_vt_prm_dev[i]) = *m_trs.GetPrmDev(i); //因需要检查参数是否修改，只能使用副本
            }
            m_trs_inited = true;
        }
    }
    if(m_trs_inited){
        UpdateData(false);
    }else{
        UpdateDataUninited();
    }
    ui->btnSaveChange->setEnabled(m_trs_inited);
    ui->btnOutputResToTxt->setEnabled(m_trs_inited);
    ui->btnImportPrm->setEnabled(m_trs_inited);
    ui->btnOutputPrm->setEnabled(m_trs_inited);
    ui->btnSaveChange->setEnabled(m_trs_inited);
    ui->btnSaveChangeAll->setEnabled(m_trs_inited);
    ui->btnSaveChangeThis->setEnabled(m_trs_inited);
}

void CWdtParamTrs::SetTrsFileList(QStringList path_files_trs)
{
    m_path_files_trs.clear();
    m_path_files_trs = path_files_trs;
}

void CWdtParamTrs::onCmbFreqChanged()
{
    int freqType = ui->cmbSigType->currentIndex();
    float sig_freq = m_vt_sig_freq[ui->cmbSigBaseFreq->currentIndex()];

    if(sig_freq*16 < 15.0/2){  //15采样率
        UpdateTimeLabel(15, sig_freq, freqType);
    }else if(sig_freq*16 < 150.0/2){//150采样率
        UpdateTimeLabel(150, sig_freq, freqType);
    }else{//2400采样率
        UpdateTimeLabel(2400, sig_freq, freqType);
    }
}

void CWdtParamTrs::UpdateData(bool bSaveOrShow)
{
    int index = 0;
    TdipPrmMeas_S *prm_meas = NULL;

    assert(m_trs_inited==true);
    prm_meas = &m_prm_meas;
    if(bSaveOrShow){
        sprintf(prm_meas->project_name, "%s", ui->edtProjName->text().toStdString().data());
        prm_meas->line_no = ui->edtLineNo->text().toInt();
        prm_meas->Ax = ui->edtPosAx->text().toFloat();
        prm_meas->Bx = ui->edtPosBx->text().toFloat();
        if(prm_meas->layout_type == SIP_MID_GRADIENT){
            prm_meas->Ay = ui->edtDistLine2AB->text().toFloat();
            prm_meas->By = prm_meas->Ay;
        }else if(prm_meas->layout_type == SIP_MID_GRADIENT){
            prm_meas->Ay = ui->edtDistLine2AB->text().toFloat();
            prm_meas->By = prm_meas->Ay;
        }else{
            prm_meas->Ay = 0;
            prm_meas->By = ui->edtPosBy->text().toFloat();
        }
        prm_meas->layout_type = (SipLayout_T)ui->cmbSipLayout->currentIndex();
        prm_meas->freq_type = m_cmb_sig_info.at(ui->cmbSigType->currentIndex())->freq_type;
        prm_meas->sig_freq = m_vt_sig_freq[ui->cmbSigBaseFreq->currentIndex()];
        prm_meas->cycles_once_cal = ui->edtCycle->text().toInt();
        prm_meas->harm_num = m_cmb_sig_info.at(ui->cmbSigType->currentIndex())->harm_num;
//**        prm_meas->start_time = m_geo_meas->GetMaxGpsTime()+30; //**60;
        sprintf(prm_meas->file_name, "%s", ui->edtFileName->text().toStdString().data());
        prm_meas->mn_len = ui->edtMN->text().toFloat();
        prm_meas->first_pole_x = ui->edtPosFirstPole->text().toFloat();
        prm_meas->curr_station_used = ui->rdCurrStationName->isChecked();
        sprintf(prm_meas->curr_station_name,"%s", ui->cmbCurrStationName->currentText().toStdString().data());
        prm_meas->curr_resist = m_vt_curr_resist.at(ui->cmbCurrResist->currentIndex());
        prm_meas->curr_value = ui->edtCurrValue->text().toFloat();
        float halfCycle = (1.0/prm_meas->sig_freq)/2.0;
        prm_meas->t0 = ui->edtT0->text().toFloat();
        prm_meas->t1 = ui->edtT1->text().toFloat();
        prm_meas->t2 = ui->edtT2->text().toFloat();
        prm_meas->t3 = prm_meas->t1 + halfCycle;
        prm_meas->t4 = prm_meas->t2 + halfCycle;
        prm_meas->l1 = ui->edtL1->text().toFloat();
        prm_meas->l2 = ui->edtL2->text().toFloat();
        prm_meas->l3 = prm_meas->l1;
        prm_meas->l4 = prm_meas->l2;
        //log_info("=====>Mx=%f Nx=%f",m_vt_prm_dev.at(0)->pos_m_x[1], m_vt_prm_dev.at(0)->pos_n_x[1]);
    }else{
        ui->edtMN->blockSignals(true);
        ui->edtPosFirstPole->blockSignals(true);
        ui->edtDistLine2AB->blockSignals(true);
        if(m_change_params){
            UpdateCurrStationCmb();
            ui->edtProjName->setText(tr("%1").arg(prm_meas->project_name));
            ui->edtLineNo->setText(tr("%1").arg(prm_meas->line_no));
            ui->edtPosAx->setText(tr("%1").arg(prm_meas->Ax));
            ui->edtPosAy->setText(tr("%1").arg(prm_meas->Ay));
            ui->edtPosBx->setText(tr("%1").arg(prm_meas->Bx));
            ui->edtPosBy->setText(tr("%1").arg(prm_meas->By));
            ui->cmbSipLayout->setCurrentIndex(prm_meas->layout_type);
            index = IndexOfSigInfo(prm_meas->freq_type, prm_meas->harm_num);
            ui->cmbSigType->setCurrentIndex(index);
            ui->cmbSigBaseFreq->setCurrentIndex(GetIdxOf(m_vt_sig_freq, prm_meas->sig_freq));
            ui->edtFileName->setText(tr("%1").arg(prm_meas->file_name));

            ui->rdCurrStationName->setChecked(prm_meas->curr_station_used);
            index = algo_index_of(prm_meas->curr_resist, m_vt_curr_resist.data(), m_vt_curr_resist.size(), 1E-5);
            ui->cmbCurrResist->setCurrentIndex(index);
            ui->rdCurrValue->setChecked(!prm_meas->curr_station_used);
            ui->cmbCurrStationName->setCurrentText(tr("%1").arg(prm_meas->curr_station_name));
            ui->edtCurrValue->setText(tr("%1").arg(prm_meas->curr_value));
        }
        if(prm_meas->curr_station_used){
            index = GetDevIdx(tr("%1").arg(prm_meas->curr_station_name), &m_vt_prm_dev);
        }else{
            index = -1;
        }
        ui->edtMN->setText(tr("%1").arg(prm_meas->mn_len));
        ui->edtPosFirstPole->setText(tr("%1").arg(prm_meas->first_pole_x));
        if(m_vt_prm_dev.size() > 0)
            ui->edtDistLine2AB->setText(tr("%1").arg(m_vt_prm_dev[0]->dist2AB));
        ui->edtMN->blockSignals(false);
        ui->edtPosFirstPole->blockSignals(false);
        ui->edtDistLine2AB->blockSignals(false);
        onCmbFreqChanged();
        ui->edtCycle->setText(tr("%1").arg(prm_meas->cycles_once_cal));
        ui->edtT0->setText(tr("%1").arg(prm_meas->t0));
        ui->edtT1->setText(tr("%1").arg(prm_meas->t1));
        ui->edtT2->setText(tr("%1").arg(prm_meas->t2));
        ui->edtL1->setText(tr("%1").arg(prm_meas->l1));
        ui->edtL2->setText(tr("%1").arg(prm_meas->l2));
        UpdateDevLayoutShow(index, prm_meas->first_pole_x, prm_meas->mn_len);
        UpdateCurrUiState();
    }
}

void CWdtParamTrs::ResetPrmDev(QVector<TdipPrmDev_S *> *vt_prm, int dev_num)
{
    int i;
    if(vt_prm->size() == dev_num) //一样的缓存，不需要重新申请
        return;
    for(i=0; i<vt_prm->size(); i++){
        delete vt_prm->at(i);
    }
    vt_prm->clear();
    for(i=0; i<dev_num; i++){
        vt_prm->append(new TdipPrmDev_S);
    }
}

void CWdtParamTrs::UpdateDataUninited()
{
    ui->edtProjName->setText(tr(""));
    ui->edtLineNo->setText(tr(""));
    ui->edtPosAx->setText(tr(""));
    ui->edtPosBx->setText(tr(""));
    ui->cmbSipLayout->setCurrentText(tr(""));
    ui->cmbSigType->setCurrentText(tr(""));
    ui->cmbSigBaseFreq->setCurrentText(tr(""));
    ui->edtFileName->setText(tr(""));
    ui->edtMN->setText(tr(""));
    ui->edtPosFirstPole->setText(tr(""));
    ui->rdCurrStationName->setChecked(true);
    ui->cmbCurrResist->setCurrentText(tr(""));
    ui->rdCurrValue->setChecked(false);
    ui->cmbCurrStationName->setCurrentText(tr("%1"));
    ui->edtCurrValue->setText(tr(""));
    ResetPrmDev(&m_vt_prm_dev, 0);
    UpdateDevLayoutShow(-1, 0, 20);
//    m_model_dev_lay->SetMnLen(20);
//    m_model_dev_lay->SetFirstPoleX(0);
    //    m_model_dev_lay->SetCurrDevIdx(-1);
    ui->edtCycle->setText(tr(""));
    ui->edtT0->setText(tr(""));
    ui->edtT1->setText(tr(""));
    ui->edtT2->setText(tr(""));
    ui->edtL1->setText(tr(""));
    ui->edtL2->setText(tr(""));
    ui->label_t1->setText(tr(""));
    ui->label_t2->setText(tr(""));
    ui->label_l1->setText(tr(""));
    ui->label_l2->setText(tr(""));
}

void CWdtParamTrs::UpdateDevLayoutShow(int idx_dev_curr, float first_pole_x, float mn_len)
{
    m_model_dev_lay->SetMnLen(mn_len);
    m_model_dev_lay->SetFirstPoleX(first_pole_x);
    m_model_dev_lay->SetCurrDevIdx(idx_dev_curr);
    emit m_model_dev_lay->layoutChanged();
    //ui->tbDevLay->resizeColumnsToContents();
}

void CWdtParamTrs::UpdateCurrStationCmb()
{
    int nCnt = m_vt_prm_dev.size();
    QString strName;

    ui->cmbCurrStationName->blockSignals(true);
    ui->cmbCurrStationName->clear();
    for(int i=0; i<nCnt; i++){
        strName = tr("%1").arg(m_vt_prm_dev[i]->station_name);
        ui->cmbCurrStationName->addItem(strName);
    }
    ui->cmbCurrStationName->blockSignals(false);
}

void CWdtParamTrs::UpdateCurrUiState()
{
    bool station_used = false;

    station_used = ui->rdCurrStationName->isChecked();
    ui->edtCurrValue->setEnabled(!station_used);
    ui->cmbCurrResist->setEnabled(station_used);
}

int CWdtParamTrs::GetDevIdx(QString strDevName, QVector<TdipPrmDev_S*> *vt_prm)
{
    int idx_dev = -1;

    for(int i=0; i<vt_prm->size(); i++){
        if(strDevName==tr("%1").arg(vt_prm->at(i)->station_name)){
            idx_dev = i;
            break;
        }
    }
    return idx_dev;
}

void CWdtParamTrs::onUpdatePowerUiState()
{
    bool pole3 = (ui->cmbSipLayout->currentIndex() == SIP_POLE3_SOUNDING);
    //供电点配置
    ui->lbBy->setVisible(pole3);
    ui->edtPosBy->setVisible(pole3);

    ui->cb_posB->setVisible(pole3);
    EnablePosB();
}

void CWdtParamTrs::EnablePosB()
{
    bool pole3 = (ui->cmbSipLayout->currentIndex() == SIP_POLE3_SOUNDING);
    ui->edtPosBx->setEnabled(ui->cb_posB->isChecked() || !pole3);
    ui->lbBx->setEnabled(ui->cb_posB->isChecked() || !pole3);
    ui->edtPosBy->setEnabled(ui->cb_posB->isChecked());
    ui->lbBy->setEnabled(ui->cb_posB->isChecked());
}

void CWdtParamTrs::onBtnSaveChange()
{
    QString strTmp;
    string strFileName;

    strFileName = m_trs.GetFullFileName();
    strTmp = tr("%1").fromStdString(ex_g2u(strFileName));

    if(QMessageBox::Yes != QMessageBox::question(this,
                          tr("提示"),
                          tr("确认修改并覆盖文件“%1”？\n注意：在修改覆盖前，建议备份原始采集数据文件！").arg(strTmp)
                          )){
        return;
    }
    UpdateData(true);
    UpdateToTrs();

    if(m_trs.WriteFile(strFileName.data())){
        QMessageBox::information(this, tr("提示"), tr("文件“%1”已保存!").arg(strTmp));
    }else{
        QMessageBox::warning(this, tr("提示"), tr("文件“%1”保存失败!").arg(strTmp));
    }
}

void CWdtParamTrs::onBtnSaveResToTxt()
{
    QString strTmp;
    string strFileName;

    UpdateToTrs();
    strFileName = m_trs.GetFullFileName();
    strTmp = tr("%1").fromStdString(ex_g2u(strFileName));
    /* 同时将SRS打印到txt文件中 */
    assert(strFileName.size() >= 4);
    strFileName.erase(strFileName.size()-4, 4);
    strFileName += "_.txt";
    m_trs.SetOutputTxtFormat(true);
    if(m_trs.WriteFileToTxt(strFileName.data())){
        QMessageBox::information(this, tr("提示"), tr("文件“%1”已导出到TXT文件中!").arg(strTmp));
    }else{
        QMessageBox::warning(this, tr("提示"), tr("文件“%1”导出失败!").arg(strTmp));
    }
}
void CWdtParamTrs::onUpdateDevLayout()
{
    float mn_len, first_pole_x, dist_ab;
    int i, idx_ch;
    int idx_dev_curr = -1;

    mn_len = ui->edtMN->text().toFloat();
    first_pole_x = ui->edtPosFirstPole->text().toFloat();
    dist_ab = ui->edtDistLine2AB->text().toFloat();
    for(i=0; i<m_vt_prm_dev.size(); i++){
        m_vt_prm_dev[i]->dist2AB = dist_ab;
        if(m_vt_prm_dev[i]->pos_valid){
            for(idx_ch=0; idx_ch<MAX_CH_NUM; idx_ch++){
                m_vt_prm_dev[i]->pos_m_x[idx_ch] = PubGetPoleMPos(first_pole_x,
                                                          mn_len,
                                                          m_vt_prm_dev[i]->ch_seq_idx[idx_ch]);
                m_vt_prm_dev[i]->pos_n_x[idx_ch] = m_vt_prm_dev[i]->pos_m_x[idx_ch]+mn_len;
            }
        }
    }
    if(ui->rdCurrStationName->isChecked()){
        idx_dev_curr = GetDevIdx(ui->cmbCurrStationName->currentText(),&m_vt_prm_dev);
    }
    UpdateDevLayoutShow(idx_dev_curr, first_pole_x, mn_len);
}

void CWdtParamTrs::onBtnImportPrm()
{
    static QString strPath;
    QString strDefaultName;
    QString strFile, strBaseName;
    string strFileName;
    FILE *pFile;

    if (QMessageBox::question(this, tr("参数覆盖"), tr("是否仅导入测线电极相关设置?"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
        m_change_params = false;
    } else {
        m_change_params = true;
    }

    strFileName = m_trs.GetFullFileName();
    strDefaultName = tr("%1").fromStdString(ex_g2u(strFileName));
    if(strPath.isEmpty()){
        strDefaultName.replace(tr(".TRS"), tr(".TPM"), Qt::CaseInsensitive);
    }else{
        strBaseName = GetBaseNameFromFullFileName(strDefaultName);
        strDefaultName = strPath+tr("/%1").arg(strBaseName);
    }
//    strDefaultName += tr("/%1").arg(m_prm_meas.file_name);
    strFile = QFileDialog::getOpenFileName(this,
                                           tr("Open Tdip param"),
                                           strDefaultName,
                                           tr("TPM Files (*.TPM)"));
    if(strFile.isEmpty())
        return;
    strPath = GetPathFromFullFileName(strFile);
    strFileName = ex_u2g(strFile.toStdString());
    pFile = ex_fopen(strFileName.data(), "rb");
    if(pFile == NULL){
        QMessageBox::warning(this, tr("提示"), tr("文件%1打开失败！").arg(strFile));
        return;
    }
    if(!ImportPrm(pFile)){
        QMessageBox::warning(this, tr("提示"), tr("参数导入失败！请检查采集站数量是否匹配！"));
    }else{
        QMessageBox::information(this, tr("提示"), tr("参数导入成功！"));
        UpdateData(false);
        UpdateToTrs();
    }
    ex_fclose(pFile);
    m_change_params = true;
}

void CWdtParamTrs::onBtnOutputPrm()
{
    static QString strPath;
    QString strDefaultName;
    QString strFile, strBaseName;
    string strFileName;
    FILE *pFile;

    strFileName = m_trs.GetFullFileName();
    strDefaultName = tr("%1").fromStdString(ex_g2u(strFileName));
    if(strPath.isEmpty()){
        strDefaultName.replace(tr(".TRS"), tr(".TPM"), Qt::CaseInsensitive);
    }else{
        strBaseName = GetBaseNameFromFullFileName(strDefaultName);
        strDefaultName = strPath+tr("/%1").arg(strBaseName);
    }
    strFile = QFileDialog::getSaveFileName(this,
                                           tr("Save Tdip param"),
                                           strDefaultName,
                                           tr("TPM Files (*.TPM)"));
    if(strFile.isEmpty())
        return;
    strPath = GetPathFromFullFileName(strFile);
    strFileName = ex_u2g(strFile.toStdString());
    pFile = ex_fopen(strFileName.data(), "wb");
    if(pFile == NULL){
        QMessageBox::warning(this, tr("提示"), tr("文件%1打开失败！").arg(strFile));
        return;
    }
    UpdateData(true);
    OutputPrm(pFile);
    ex_fclose(pFile);
    QMessageBox::information(this, tr("提示"), tr("参数导出成功！"));
}

void CWdtParamTrs::onPosBStateChanged(){
    EnablePosB();
}

void CWdtParamTrs::on_btnSaveChangeThis_clicked()
{
    QString strTmp;
    string strFileName;

    strFileName = m_trs.GetFullFileName();
    strTmp = tr("%1").fromStdString(ex_g2u(strFileName));

    if(QMessageBox::Yes != QMessageBox::question(this,
                          tr("提示"),
                          tr("确认修改并覆盖文件“%1”？\n注意：在修改覆盖前，建议备份原始采集数据文件！").arg(strTmp)
                          )){
        return;
    }
    UpdateData(true);
    UpdateToTrs();

    if(m_trs.WriteFile(strFileName.data())){
        QMessageBox::information(this, tr("提示"), tr("文件“%1”已保存!").arg(strTmp));
    }else{
        QMessageBox::warning(this, tr("提示"), tr("文件“%1”保存失败!").arg(strTmp));
    }
}

void CWdtParamTrs::on_btnSaveChangeAll_clicked()
{
    QString strTmp;
    string strFileName;
    QString curFilePath, filePath, baseFileName;
    QStringList errSrsFile;

    if(QMessageBox::Yes != QMessageBox::question(this,
                          tr("提示"),
                          tr("确认修改并覆盖所有文件？\n注意：在修改覆盖前，建议备份原始采集数据文件！")
                          )){
        return;
    }
    //先更新当前的srs
    UpdateData(true);
    UpdateToTrs();
    //遍历修改其它srs
    curFilePath = tr("%1").fromStdString(ex_g2u(m_trs.GetFullFileName()));
    for(int i=0; i<m_path_files_trs.size();i++){
        CFormatTRS trs;
        strFileName = ex_u2g(m_path_files_trs.at(i).toStdString());
        baseFileName = GetBaseNameFromFullFileName(m_path_files_trs.at(i));
        filePath = tr("%1").fromStdString(ex_g2u(strFileName));
        if(curFilePath == filePath){
            if(!m_trs.WriteFile(strFileName.data())){
                QMessageBox::information(this, tr("提示"), tr("文件“%1”已保存!").arg(strTmp));
                errSrsFile.append(baseFileName);
            }
        }else{
            if(!trs.ReadFile(strFileName.data())){
                errSrsFile.append(baseFileName);
                continue;
            }
            if(!UpdateToAllTrs(&trs)){
                errSrsFile.append(baseFileName);
                continue;
            }
            if(!trs.WriteFile(strFileName.data())){
                errSrsFile.append(baseFileName);
            }
        }
    }
    if(errSrsFile.size() > 0){
        QMessageBox::warning(this, tr("提示"), tr("文件“%1”保存失败!").arg(errSrsFile.join(",")));
    }else{
        QMessageBox::information(this, tr("提示"), tr("所有文件已保存!"));
    }
}

bool CWdtParamTrs::ImportPrm(FILE *pFile)
{
    char station_name[MAX_FILE_NAME_LEN];
    char file_name[MAX_FILE_NAME_LEN];
    bool ret = true;
    int i, ver, acq_num, acq_num_read;

    acq_num = m_vt_prm_dev.size();
    ver = ex_fread_s32(pFile, 0);
    //内存中的文件名不覆盖，需要保存
    strncpy(file_name, m_prm_meas.file_name, MAX_FILE_NAME_LEN);
    if(!TdipPubReadPrmMeas(pFile, &m_prm_meas)){
        return false;
    }
    strncpy(m_prm_meas.file_name, file_name, MAX_FILE_NAME_LEN);
    acq_num_read = ex_fread_s32(pFile, 0);
    if(acq_num_read != acq_num){
        return false;
    }
    for(i=0; i<acq_num; i++){
        //设备名不能覆盖，需要保存
        strncpy(station_name, m_vt_prm_dev[i]->station_name, MAX_FILE_NAME_LEN);
        TdipPubReadPrmDev(pFile, m_vt_prm_dev[i]);
        strncpy(m_vt_prm_dev[i]->station_name, station_name, MAX_FILE_NAME_LEN);
    }
    return ret;
}

void CWdtParamTrs::OutputPrm(FILE *pFile)
{
    int ver = FILE_TDPM_VER;
    int i, acq_num;

    acq_num = m_vt_prm_dev.size();

    ex_fwrite_s32(pFile, ver);
    TdipPubWritePrmMeas(pFile, &m_prm_meas);
    ex_fwrite_s32(pFile, acq_num);
    for(i=0; i<acq_num; i++){
        TdipPubWritePrmDev(pFile, m_vt_prm_dev[i]);
    }
}

void CWdtParamTrs::UpdateToTrs()
{
    UpdateData(true);
    //更新CTdipResultCal中的通道信息和电阻率信息
    UpdateChInfoAndApp(m_trs.GetResultCal());
    //更新测量参数和布极参数(前面计算需要使用，确保最后一步更新)
    m_trs.SetPrmMeas(&m_prm_meas);
    assert(m_trs.GetDevNum() == m_vt_prm_dev.size());
    for(int i=0; i<m_trs.GetDevNum(); i++){
        m_trs.SetPrmDev(i, m_vt_prm_dev[i]);
    }
}

bool CWdtParamTrs::UpdateToAllTrs(CFormatTRS *trs)
{
    bool result = true;
    //更新CResultSip中的通道信息和电阻率信息
    if(!UpdateChInfoAndAppAll(trs->GetResultCal(), trs)){
        return false;
    }
    //更新测量参数和布极参数(前面计算需要使用，确保最后一步更新)
    if(!UpdateParamToOtherTrs(trs)){
        return false;
    }
    return result;
}

void CWdtParamTrs::UpdateChInfoAndApp(CTdipResultCal *res)
{
    CResult::ChInfo_S ch_info, ch_info_new;
    TdipPrmDev_S *prm_dev_new = NULL;
    TdipPrmDev_S *prm_dev_old = NULL;
    TdipPrmMeas_S *prm_old = m_trs.GetPrmMeas();
    TdipPrmMeas_S *prm_new = &m_prm_meas;
    int i, ch_num, idx_seq_n, idx_dev_ch;
    double eq_cp_err = 1E-5;
    double k_old, k_new, app_ratio;
    float pos_m, pos_n;
    float c_multi_ratio = 1; //电流值修改的乘积比值

    assert(res != NULL);
    c_multi_ratio = GetCurrRatioN2O(prm_new, prm_old);
    ch_num = res->GetChNum();
    for(i=0; i<ch_num; i++){
        //检查通道的装置参数是否有变化
        ch_info = res->GetChInfo(i);
        assert(ch_info.idx_dev>=0 && ch_info.idx_dev<m_vt_prm_dev.size());
        assert(ch_info.idx_dev_ch>=0 && ch_info.idx_dev_ch < MAX_CH_NUM);
        prm_dev_new = m_vt_prm_dev[ch_info.idx_dev];
        prm_dev_old = m_trs.GetPrmDev(ch_info.idx_dev);
        idx_dev_ch = ch_info.idx_dev_ch;
        idx_seq_n = prm_dev_new->ch_seq_idx[idx_dev_ch];
        //根据修改参数更新res中单通道的ch_info信息
        ch_info_new = ch_info;
        if(prm_dev_new->pos_valid){
            pos_m = prm_dev_new->pos_m_x[idx_dev_ch];
            pos_n = prm_dev_new->pos_n_x[idx_dev_ch];
        }else{
            pos_m = PubGetPoleMPos(prm_new->first_pole_x,prm_new->mn_len,idx_seq_n);
            pos_n = pos_m+prm_new->mn_len;
        }
        ch_info_new.dev_name = string(prm_dev_new->station_name);
        ch_info_new.idx_seq = idx_seq_n;
        ch_info_new.line_no = prm_new->line_no;
        ch_info_new.pos_m = pos_m;
        ch_info_new.e_len = fabs(pos_n-pos_m);
        ch_info_new.dist2AB = prm_dev_new->dist2AB;
        ch_info_new.ch_type = prm_dev_new->ch_type[idx_dev_ch];
        //检查装置系数和采样电阻是否变化，变化则重新计算视电阻率
        k_old = TdipPubCalK(idx_dev_ch, prm_old,prm_dev_old,ch_info);
        k_new = TdipPubCalK(idx_dev_ch, prm_new,prm_dev_new,ch_info_new);
        app_ratio = (k_new/k_old)/c_multi_ratio;
        if(!algo_is_equal_cp(1.0, app_ratio, eq_cp_err)){
            res->AppMulti(i, app_ratio);
        }
        res->SetChInfo(i, ch_info_new);
    }
}

bool CWdtParamTrs::UpdateChInfoAndAppAll(CTdipResultCal *res, CFormatTRS *target_trs)
{
    bool result = true;
    CResult::ChInfo_S ch_info, ch_info_new;
    TdipPrmDev_S *prm_dev_new = NULL;
    TdipPrmDev_S *prm_dev_old = NULL;
    TdipPrmMeas_S *prm_old = NULL;
    TdipPrmMeas_S *prm_new = NULL;
    QString dev_name,old_dev_name;
    int i, ch_num, idx_seq_n, idx_dev_ch;
    double eq_cp_err = 1E-5;
    double k_old, k_new, app_ratio;
    float pos_m, pos_n;
    float c_multi_ratio = 1; //电流值修改的乘积比值 因为只修改电极部分，电流相关参数没有变动，所以电流值乘积比值不变

    if(res == NULL || target_trs == NULL)
        return false;
    if(target_trs->GetDevNum() != m_trs.GetDevNum()){
        return false;
    }
    prm_old = target_trs->GetPrmMeas();
    prm_new = target_trs->GetPrmMeas();
    //更新prm_new的首电极坐标和电极距用于后面新旧装置系数对比计算
    prm_new->first_pole_x = m_trs.GetPrmMeas()->first_pole_x;
    prm_new->mn_len = m_trs.GetPrmMeas()->mn_len;
    //遍历通道更新通道信息
    ch_num = res->GetChNum();
    for(i=0; i<ch_num; i++){
        //检查通道的装置参数是否有变化
        ch_info = res->GetChInfo(i);
        if(ch_info.idx_dev < 0 || ch_info.idx_dev >= m_trs.GetDevNum()){
            result = false;
            break;
        }
        if(ch_info.idx_dev_ch <0 || ch_info.idx_dev_ch >= MAX_CH_NUM){
            result = false;
            break;
        }
        prm_dev_new = m_trs.GetPrmDev(ch_info.idx_dev);
        prm_dev_old = target_trs->GetPrmDev(ch_info.idx_dev);
        dev_name = tr("%1").fromStdString(prm_dev_new->station_name).trimmed();
        old_dev_name = tr("%1").fromStdString(prm_dev_old->station_name).trimmed();
        if((dev_name != old_dev_name) || (prm_dev_new->ch_num != prm_dev_old->ch_num)){
            result = false;
            break;
        }
        idx_dev_ch = ch_info.idx_dev_ch;
        idx_seq_n = prm_dev_new->ch_seq_idx[idx_dev_ch];
        //根据修改参数更新res中单通道的ch_info信息
        ch_info_new = ch_info;
        if(prm_dev_new->pos_valid){
            pos_m = prm_dev_new->pos_m_x[idx_dev_ch];
            pos_n = prm_dev_new->pos_n_x[idx_dev_ch];
        }else{
            pos_m = PubGetPoleMPos(prm_new->first_pole_x,prm_new->mn_len,idx_seq_n);
            pos_n = pos_m+prm_new->mn_len;
        }
        ch_info_new.dev_name = string(prm_dev_new->station_name);
        ch_info_new.idx_seq = idx_seq_n;
        ch_info_new.line_no = prm_new->line_no;
        ch_info_new.pos_m = pos_m;
        ch_info_new.e_len = fabs(pos_n-pos_m);
        ch_info_new.dist2AB = prm_dev_new->dist2AB;
        ch_info_new.ch_type = prm_dev_new->ch_type[idx_dev_ch];
        //检查装置系数和采样电阻是否变化，变化则重新计算视电阻率
        k_old = TdipPubCalK(idx_dev_ch, prm_old,prm_dev_old,ch_info);
        k_new = TdipPubCalK(idx_dev_ch, prm_new,prm_dev_new,ch_info_new);
        app_ratio = (k_new/k_old)/c_multi_ratio;
        if(!algo_is_equal_cp(1.0, app_ratio, eq_cp_err)){
            //log_info("=====>iCh[%d] k_old=%.4f k_new=%.4f",i,k_old,k_new);
            res->AppMulti(i, app_ratio);
        }
        res->SetChInfo(i, ch_info_new);
    }
    return result;
}

bool CWdtParamTrs::UpdateParamToOtherTrs(CFormatTRS *target_trs)
{
    TdipPrmMeas_S *prm_meas = NULL;
    TdipPrmDev_S *prm_dev = NULL;
    TdipPrmDev_S *old_prm_dev = NULL;
    QString dev_name,old_dev_name;
    bool result = true;
    //判断是否一致并且相同
    if(target_trs->GetDevNum() != m_trs.GetDevNum()){
        return false;
    }
    prm_meas = m_trs.GetPrmMeas();
    //修改电极距，首电极坐标
    target_trs->GetPrmMeas()->first_pole_x = prm_meas->first_pole_x;
    target_trs->GetPrmMeas()->mn_len = prm_meas->mn_len;
    //修改电极坐标
    for(int idx_dev=0; idx_dev < target_trs->GetDevNum();idx_dev++){
        prm_dev = m_trs.GetPrmDev(idx_dev);
        old_prm_dev = target_trs->GetPrmDev(idx_dev);
        dev_name = tr("%1").fromStdString(prm_dev->station_name).trimmed();
        old_dev_name = tr("%1").fromStdString(old_prm_dev->station_name).trimmed();
        if((dev_name != old_dev_name) || (prm_dev->ch_num != old_prm_dev->ch_num)){
            result = false;
            break;
        }
        old_prm_dev->dist2AB = prm_dev->dist2AB;
        old_prm_dev->pos_valid = prm_dev->pos_valid;
        for(int iCh=0; iCh < prm_dev->ch_num; iCh++){
            old_prm_dev->ch_type[iCh] = prm_dev->ch_type[iCh];
            old_prm_dev->ch_seq_idx[iCh] = prm_dev->ch_seq_idx[iCh];
            old_prm_dev->pos_m_x[iCh] = prm_dev->pos_m_x[iCh];
            old_prm_dev->pos_m_y[iCh] = prm_dev->pos_m_y[iCh];
            old_prm_dev->pos_n_x[iCh] = prm_dev->pos_n_x[iCh];
            old_prm_dev->pos_n_y[iCh] = prm_dev->pos_n_y[iCh];
        }
    }
    return result;
}

float CWdtParamTrs::GetCurrRatioN2O(const TdipPrmMeas_S *prm_new,
                                    const TdipPrmMeas_S *prm_old)
{
    float RatioN2O = 1; //新电流值与老电流值比值
    assert(prm_new->curr_station_used == prm_old->curr_station_used);
    if(prm_new->curr_station_used){
        RatioN2O = prm_old->curr_resist/prm_new->curr_resist;
    }else{
        RatioN2O = prm_new->curr_value/prm_old->curr_value;
    }
    return RatioN2O;
}

float CWdtParamTrs::TdipPubCalK(int idx_dev_ch,TdipPrmMeas_S *prm_meas,TdipPrmDev_S *prm_dev, CResult::ChInfo_S ch_info)
{
    float k = 1;
    float Ax,Ay,Bx,By,Mx,My,Nx,Ny;

    Ax = prm_meas->Ax;
    Bx = prm_meas->Bx;
    Ay = prm_meas->Ay;
    By = prm_meas->By;

    if(prm_meas->layout_type == SIP_ALL_GRADIENT){
        Mx = prm_dev->pos_m_x[idx_dev_ch];
        Nx = prm_dev->pos_n_x[idx_dev_ch];
        My = prm_dev->pos_m_y[idx_dev_ch];
        Ny = prm_dev->pos_n_y[idx_dev_ch];
    }else{
        Mx = ch_info.pos_m;
        Nx = Mx + ch_info.e_len;
        My = Ay + prm_dev->dist2AB;
        Ny = Ay + prm_dev->dist2AB;
    }
    log_info("=====>idx_dev_ch=%d Ax=%.4f Ay=%.4f Bx=%.4f By=%.4f Mx=%f My=%f Nx=%f Ny=%f",
             idx_dev_ch,Ax,Ay,Bx,By,Mx,My,Nx,Ny);
//    if(prm_meas->layout_type == SIP_POLE3_SOUNDING){
//        k = algo_cal_k_factor_pole3(Ax, Ay, Mx, My, Nx, Ny);
//    }else{
        k = algo_cal_k_factor(Ax, Ay, Bx, By, Mx, My, Nx, Ny);
//    }
    return k;
}

void CWdtParamTrs::UpdateTimeLabel(int sampleRate, double freq, int freqType)
{
    int once_row = (int)(0.5 + sampleRate/freq);
    double t0=0;
    double max_t1=0;
    double max_t2=0;
    QString max_l1;//L1与L3最大长度
    QString max_l2;//L2与L4最大长度
    double time_one_cycle = 1.0/freq;

    switch (freqType) {
    case 0://方波
        max_t1 = 0.5*time_one_cycle;
        max_t2 = max_t1;
        max_l1 = FormatValue(max_t1);
        max_l2 = max_l1;
        break;
    case 1://25%占空
        max_t1 = 0.125*time_one_cycle;
        max_t2 = 0.5*time_one_cycle;
        max_l1 = FormatValue(max_t1);
        max_l2 = FormatValue(3*max_t1);
        break;
    case 2://50%占空
        max_t1 = 0.25*time_one_cycle;//1/4周期
        max_t2 = 0.5*time_one_cycle;//1/2周期
        max_l1 = FormatValue(max_t1);
        max_l2 = max_l1;
        break;
    case 3://75%占空
        max_t1 = 0.375*time_one_cycle;//1/4周期
        max_t2 = 0.5*time_one_cycle;//1/2周期
        max_l1 = FormatValue(max_t1);
        max_l2 = FormatValue(max_t1/3);;
        break;
    }
    QString t1 = FormatValue(max_t1);
    QString t2 = FormatValue(max_t2);

    ui->edtT0->setText(FormatValue(t0));
    ui->label_t1->setText(tr("(0,%1)").arg(t1));
    if(freqType == 0){
        ui->label_t2->setText(tr("(0,%1)").arg(t2));
    }else{
        ui->label_t2->setText(tr("(%1,%2)").arg(t1).arg(t2));
    }
    ui->label_l1->setText(tr("长度设置区间为(0,%1)").arg(max_l1));
    ui->label_l2->setText(tr("长度设置区间为(0,%1)").arg(max_l2));
}

QString CWdtParamTrs::FormatValue(double value)
{
//    int truncatedValue = static_cast<int>(value * 10);
//    double result = truncatedValue / 10.0;
//    return QString::number(result, 'f', 1);
    QString format = tr("%1").arg(value);
    int index = format.indexOf(".");
    if(index <= 0)
        return format;
    return format.mid(0, index+3);
}

int CWdtParamTrs::GetIdxOf(const QVector<float> VtArray, float Value)
{
    int idx_ret = 0;
    for(int i=0; i<VtArray.size(); i++){
        if(fabs(Value-VtArray[i]) < 1E-6){
            idx_ret = i;
            break;
        }
    }
    return idx_ret;
}

int CWdtParamTrs::IndexOfSigInfo(FreqType_N freq_type, int harm_num)
{
    int idx = 0;
    for(int i=0; i<m_cmb_sig_info.size(); i++){
        if(m_cmb_sig_info.at(i)->freq_type == freq_type
                && m_cmb_sig_info.at(i)->harm_num == harm_num){
            idx = i;
            break;
        }
    }
    return idx;
}
