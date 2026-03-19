#include <QMessageBox>
#include <QFileDialog>
#include "wdt_tdip_res.h"
#include "ui_wdt_tdip_res.h"
#include "ex_code.h"
#include "qt_pub.h"
#include "shared_data.h"
#include "tdip_ts2spect.h"

CWdtTdipRes::CWdtTdipRes(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CWdtTdipRes)
{
    ui->setupUi(this);
    m_trs_inited = false;

    InitUiShowTable();
    InitUiShowPlotPos();

    ReadCfgThreshold(&m_threshold);
    m_dlg_data_limit = new CdlgLimitSettings(&m_threshold, this);
    ui->lytAppend->addWidget(m_wdt_res_plot_space);
    //初始化页面选择相关参数
    ui->cmbShowType->clear();
    ui->cmbShowType->addItem(tr("数据表格"));
    m_vt_wdt_show.append(m_wdt_res_table);

    ui->cmbShowType->addItem(tr("空间视图"));
    m_vt_wdt_show.append(m_wdt_res_plot_space);
    ui->cmbShowType->setCurrentIndex(0);

    onSwitchShowPage();
    UpdatePassRate();

    connect(ui->cmbShowType, SIGNAL(currentIndexChanged(int)), this, SLOT(onSwitchShowPage()));
}

CWdtTdipRes::~CWdtTdipRes()
{
    delete ui;
}

void CWdtTdipRes::SetTrsFileList(QStringList path_files_trs)
{
    m_path_files_trs.clear();
    m_path_files_trs = path_files_trs;
}

void CWdtTdipRes::UpdateShow(QString strTrsFileName)
{
    string strFile = ex_u2g(strTrsFileName.toStdString());
    if(strTrsFileName.isEmpty()){
        m_trs_inited = false;
    }else{
        if(!m_trs.ReadFile(strFile.data())){
            QMessageBox::warning(this, tr("提示"), tr("打开文件“%1”失败！").arg(strTrsFileName));
            m_trs_inited = false;
        }else{
            m_trs_inited = true;
        }
    }
    if(m_trs_inited){
        m_wdt_res_table->SetGeoMeas(&m_trs);
        m_wdt_res_plot_space->SetGeoMeas(&m_trs);
        onUpdateShowResult();
        UpdatePassRate();
        UpdateRecalUi(true);
    }else{
        m_wdt_res_table->Clear();
        UpdateRecalUi(false);
    }
    //根据索引切换页面
    onSwitchShowPage();
}

void CWdtTdipRes::InitUiShowTable()
{
    m_wdt_res_table = ui->wdtResTable;
    m_wdt_res_table->Init(&m_threshold);
}

void CWdtTdipRes::InitUiShowPlotPos()
{
    m_wdt_res_plot_space = new CWdtTdipResSpace(this);
    m_wdt_res_plot_space->Init();
}

void CWdtTdipRes::onSwitchShowPage()
{
    int sel = ui->cmbShowType->currentIndex();
    for(int i=0; i<m_vt_wdt_show.size(); i++){
        m_vt_wdt_show.at(i)->setHidden((i!=sel));
    }
}

void CWdtTdipRes::UpdatePassRate()
{
    CTdipResultCal::ResItem_S res_item;
    float pass_rate = 0;
    int ch_num, cal_num, cnt_pass;
    if(m_trs_inited){
        CTdipResultCal *res =  m_trs.GetResultCal();
        cal_num = m_trs.GetRes()->DoGetBuffCalNum();
        ch_num = res->GetChNum();
        if(cal_num > 0){
            cnt_pass = 0;
            for(int i=0; i<ch_num;i++){
                res_item = res->GetResItem(i, 0, 0);
                if(!m_threshold.IsQualifiedData(res_item.appr_value, res_item.appr_err_value,
                                                CDataThreshold::DATA_TYPE_APP))
                    continue;
                if(!m_threshold.IsQualifiedData(res_item.appc_value, res_item.appc_err_value,
                                                CDataThreshold::DATA_TYPE_APPC))
                    continue;
                cnt_pass++;
            }
            if(ch_num != 0){
                pass_rate = (float)cnt_pass/(float)ch_num;
            }
        }
    }
    ui->lbPassRate->setText(tr("").sprintf("合格率：%.1f%", pass_rate*100));
}

void CWdtTdipRes::on_btnErrSet_clicked()
{
    if(QDialog::Accepted == m_dlg_data_limit->exec()){
        UpdatePassRate();
        m_wdt_res_table->onUpdateShowResult();
    }
}

void CWdtTdipRes::on_btnSave_clicked()
{
    onSaveResToFile();
}

void CWdtTdipRes::showEvent(QShowEvent *event)
{
    if(event->type() == QEvent::Show){
        if(m_trs_inited)
            onUpdateShowResult();//在UpdateShowParam()之后
    }
    QWidget::showEvent(event);
}

void CWdtTdipRes::onUpdateShowResult()
{
    m_wdt_res_table->onUpdateShowResult();
    m_wdt_res_plot_space->onUpdateShowResult();
    UpdatePassRate();
}

void CWdtTdipRes::UpdateRecalUi(bool isInit)
{
    if(isInit){
        ui->edtStart->setText(tr("%1").arg(m_trs.GetPrmMeas()->t0));
        ui->edtCycleNum->setText(tr("%1").arg(m_trs.GetPrmMeas()->cycles_once_cal));
        ui->edtTime1->setText(tr("%1").arg(m_trs.GetPrmMeas()->t1));
        ui->edtLen1->setText(tr("%1").arg(m_trs.GetPrmMeas()->l1));
        ui->edtTime2->setText(tr("%1").arg(m_trs.GetPrmMeas()->t2));
        ui->edtLen2->setText(tr("%1").arg(m_trs.GetPrmMeas()->l2));
        UpdateTimeLabel(m_trs.GetPrmMeas()->sample_rate,
                        m_trs.GetPrmMeas()->sig_freq,
                        m_trs.GetPrmMeas()->harm_num);
    }else{
        ui->edtStart->setText(tr(""));
        ui->edtCycleNum->setText(tr(""));
        ui->edtTime1->setText(tr(""));
        ui->label_time1->setText(tr(""));
        ui->edtLen1->setText(tr(""));
        ui->edtLen1->setText(tr(""));
        ui->edtTime2->setText(tr(""));
        ui->label_time2->setText(tr(""));
        ui->edtLen2->setText(tr(""));
        ui->label_len2->setText(tr(""));
        ui->edtTmtPath->setText(tr(""));

    }
}

void CWdtTdipRes::onSaveResToFile()
{
    if(!m_trs_inited)
        return;

    QString strDefaultName = m_str_path;
    QString strFile;

    strDefaultName += tr("/%1").arg(m_trs.GetPrmMeas()->file_name);
    strFile = QFileDialog::getSaveFileName(this,
                                           tr("Save TRS"),
                                           strDefaultName,
                                           tr("TRS Files (*.TRS)"));
    if(strFile.isEmpty())
        return;
    m_str_path = GetPathFromFullFileName(strFile);
    string strFileName = ex_u2g(strFile.toStdString());
    FILE *pFile = ex_fopen(strFileName.data(), "wb");
    if(pFile == NULL){
        QMessageBox::warning(this, tr("提示"), tr("文件%1打开失败！").arg(strFile));
        return;
    }
    m_trs.WriteData(pFile);
    ex_fclose(pFile);
    QMessageBox::information(this, tr("提示"), tr("文件保存成功！"));
}

void CWdtTdipRes::ReadCfgThreshold(CDataThreshold *threshold)
{
    vector<LimitItem_S> *m_vt_limit = SharedData::instance()->GetLimitInfo();
    bool valid;
    float value;

    LimitItem_S item_app = m_vt_limit->at(LIMIT_TYPE_APP);
    valid = item_app.max_err_cp.valid;
    value = item_app.max_err_cp.value;
    if(valid){
        threshold->SetErrCp(CDataThreshold::DATA_TYPE_APP, value);
    }else{
        threshold->DisableErrCp(CDataThreshold::DATA_TYPE_APP);
    }

    LimitItem_S item_appc = m_vt_limit->at(LIMIT_TYPE_APPC);
    valid = item_appc.max_err_cp.valid;
    value = item_appc.max_err_cp.value;
    if(valid){
        threshold->SetErrCp(CDataThreshold::DATA_TYPE_APPC, value);
    }else{
        threshold->DisableErrCp(CDataThreshold::DATA_TYPE_APPC);
    }
}

void CWdtTdipRes::WriteCfgThreshold(CDataThreshold *threshold)
{
    vector<LimitItem_S> *m_vt_limit = SharedData::instance()->GetLimitInfo();
    bool valid;
    float value;

    valid  = threshold->IsValidErrCp(CDataThreshold::DATA_TYPE_APP);
    value  = threshold->GetErrCp(CDataThreshold::DATA_TYPE_APP);
    if(valid){
        DataLimitSetItem(&m_vt_limit->at(LIMIT_TYPE_APP).max_err_cp, value);
    }else{
        DataLimitDisableItem(&m_vt_limit->at(LIMIT_TYPE_APP).max_err_cp);
    }

    valid = threshold->IsValidErrCp(CDataThreshold::DATA_TYPE_APPC);
    value = threshold->GetErrCp(CDataThreshold::DATA_TYPE_APPC);
    if(valid){
        DataLimitSetItem(&m_vt_limit->at(LIMIT_TYPE_APPC).max_err_cp, value);
    }else{
        DataLimitDisableItem(&m_vt_limit->at(LIMIT_TYPE_APPC).max_err_cp);
    }
    SharedData::instance()->SetLimitInfo(m_vt_limit);
}
/**
 * @brief 选择时序目录
 */
void CWdtTdipRes::on_btnTmtDir_clicked()
{
    static QString strPath;
    QString strDefaultName;
    QString strFile;

    if(strPath.isEmpty()){
        strDefaultName = GetPathFromFullFileName(tr("%1").fromStdString(ex_g2u(m_trs.GetFullFileName())));
        if(strDefaultName.size() <=1 && m_path_files_trs.size() > 0){
            strDefaultName = m_path_files_trs.at(0);
        }
    }else{
        strDefaultName = strPath;
    }
    strFile = QFileDialog::getExistingDirectory(this,
                                           tr("选择TMT目录"),
                                           strDefaultName);
    if(strFile.isEmpty())
        return;
    strPath = strFile;
    ui->edtTmtPath->setText(strFile);
}

void CWdtTdipRes::on_btnTrsRecal_clicked()
{
    static QString outPath;
    QString strFile;
    QString strDefaultName, trsFullFileName;
    QString trs_base_name;
    //检查时序文件目录是否为空
    QString filePath = ui->edtTmtPath->text();
    if(filePath.isEmpty()){
        QMessageBox::warning(this, tr("警告"), tr("TMT时序文件目录为空！"));
        return;
    }
    //检查是否已经选择需要重计算的TRS文件
    trsFullFileName = tr("%1").fromStdString(ex_g2u(m_trs.GetFullFileName()));
    if(m_path_files_trs.size() < 1 || trsFullFileName.isEmpty()){
        QMessageBox::warning(this, tr("警告"), tr("请选择需要进行重计算的TRS文件！"));
        return;
    }
    //检查TRS文件是否正常
    trs_base_name = GetBaseNameFromFullFileName(trsFullFileName);
    if(!m_trs.ReadFile(ex_u2g(trsFullFileName.toStdString()).data())){
        QMessageBox::warning(this, tr("警告"), tr("%1.TRS文件读取失败！").arg(trs_base_name));
        return;
    }
    //选择导出目录
    if(outPath.isEmpty()){
        outPath = GetPathFromFullFileName(trsFullFileName);
    }
    strDefaultName = outPath + tr("/%1").arg(trs_base_name);

    strFile = QFileDialog::getSaveFileName(this,
                                           tr("Save TRS"),
                                           strDefaultName,
                                           tr("TRS Files (*.TRS)"));
    if(strFile.isEmpty())
        return;
    //更新参数
    TdipPrmMeas_S *prm_meas = m_trs.GetPrmMeas();
    float halfCycle = (1.0/prm_meas->sig_freq)/2.0;
    prm_meas->t0 = ui->edtStart->text().toFloat();
    prm_meas->t1 = ui->edtTime1->text().toFloat();
    prm_meas->t2 = ui->edtTime2->text().toFloat();
    prm_meas->t3 = prm_meas->t1 + halfCycle;
    prm_meas->t4 = prm_meas->t2 + halfCycle;
    prm_meas->l1 = ui->edtLen1->text().toFloat();
    prm_meas->l2 = ui->edtLen2->text().toFloat();
    prm_meas->l3 = prm_meas->l1;
    prm_meas->l4 = prm_meas->l2;
    prm_meas->cycles_once_cal = ui->edtCycleNum->text().toInt();
    //重计算
    QStringList list_dev_recal,list_dev_unrecal;
    if(!RecalTsToTrs(&m_trs, list_dev_recal, list_dev_unrecal)){
        return;
    }

    //存储
    string strFileName = ex_u2g(strFile.toStdString());
    outPath = GetPathFromFullFileName(strFile);
    FILE *pFile = ex_fopen(strFileName.data(), "wb");
    if(pFile == NULL){
        QMessageBox::warning(this, tr("警告"), tr("文件%1打开失败！").arg(strFile));
        return;
    }
    m_trs.WriteData(pFile);
    ex_fclose(pFile);
    QMessageBox::information(this, tr("提示"), tr("文件保存成功！\n重计算设备:%1\n未计算设备:%2").arg(list_dev_recal.join(",")).arg(list_dev_unrecal.join(",")));
}

void CWdtTdipRes::on_btnTrsOutTxt_clicked()
{
    QString strTmp;
    string strFileName;

    //UpdateToSrs();
    strFileName = m_trs.GetFullFileName();
    strTmp = tr("%1").fromStdString(ex_g2u(strFileName));
    if(strTmp.isEmpty())
        return;
    /* 同时将TRS打印到txt文件中 */
    strFileName.replace(strFileName.length()-3,3,"txt");
    m_trs.SetOutputTxtFormat(false);
    if(m_trs.WriteFileToTxt(strFileName.data())){
        QMessageBox::information(this, tr("提示"), tr("文件“%1”已导出到TXT文件中!").arg(strTmp));
    }else{
        QMessageBox::warning(this, tr("提示"), tr("文件“%1”导出失败!").arg(strTmp));
    }
}

bool CWdtTdipRes::RecalTsToTrs(CFormatTRS *trs,
                               QStringList &list_dev_recal,
                                QStringList &list_dev_unrecal)
{
    CTdipTs2Spect Ts2Spect;
    vector<CTdipTs2Spect::TsChInfo_S> vt_chs;
    CTdipTs2Spect::TsChInfo_S ch_curr;
    TdipPrmMeas_S *prm_meas = NULL;
    TdipPrmDev_S *prm_dev = NULL;
    CTdipResultCal *res = trs->GetResultCal();
    string dev_name, file_trs, file_tmt;
    bool bad_file, m_curr_station_used;
    int idx_dev;
    CTdipResult spect_array_u,spect_array_i;
    CTdipResultItem currResultItem;
    float current_value;

    prm_meas = trs->GetPrmMeas();
    file_trs = trs->GetFullFileName();
    m_curr_station_used = prm_meas->curr_station_used;
    current_value = prm_meas->curr_value;
    file_tmt = GetTmtFileName(trs->GetFullFileName(), prm_meas->curr_station_name);

    Ts2Spect.SetSignalInfo(prm_meas);
    Ts2Spect.ClearDevList();
    list_dev_recal.clear();
    list_dev_unrecal.clear();

    if(m_curr_station_used && !Ts2Spect.IsParamMatch(file_tmt.data())){
        QMessageBox::warning(this, tr("警告"), tr("未找到电流数据:\n%1").arg(ex_g2u(file_tmt).data()));
        return false;
    }
    ch_curr.file_tmt = file_tmt;
    ch_curr.idx_ch_in_tmt = 3;
    vt_chs.resize(res->GetChNum());
    //计算电压数据
    for(int iCh=0; iCh<res->GetChNum(); iCh++){
        idx_dev = res->GetChInfo(iCh).idx_dev;
        prm_dev = trs->GetPrmDev(idx_dev);
        dev_name = string(prm_dev->station_name);
        file_tmt = GetTmtFileName(file_trs, dev_name);
        /* 判断是否为坏文件 */
        bad_file = false;
        if(!Ts2Spect.IsParamMatch(file_tmt.data())){
            bad_file = true;
        }
        /* 填充通道对应的信息 */
        if(bad_file){
            vt_chs[iCh].file_tmt = string("");
        }else{
            vt_chs[iCh].file_tmt = file_tmt;
        }
        vt_chs[iCh].idx_ch_in_tmt = res->GetChInfo(iCh).idx_dev_ch;
    }
    if(m_curr_station_used){
        Ts2Spect.CalToSectArrWithTrs(vt_chs, &ch_curr, trs, &spect_array_u, true);
    }else{
        Ts2Spect.CalToSectArrWithTrs(vt_chs, NULL, trs, &spect_array_u, true);
    }
    if(spect_array_u.GetCaledNum() < 1){
        QMessageBox::warning(this, tr("警告"), tr("时序数据获取失败！\n请检查时序目录是否正确、对应仪器的TMT时序文件是否存在？"));
        return false;
    }
    //已计算和未计算设备填充
    QString devName;
    vector<int> list_recal = Ts2Spect.GetDevChIdList(1);
    vector<int> list_unrecal = Ts2Spect.GetDevChIdList(0);
    if(list_recal.size() > 0){
        for(int i=0;i<list_recal.size();i++){
            prm_dev = trs->GetPrmDev(list_recal.at(i));
            devName = tr("%1").fromStdString(ex_g2u(prm_dev->station_name)).trimmed();
            if(!list_dev_recal.contains(devName))
                list_dev_recal.push_back(devName);
        }
    }
    if(list_unrecal.size() > 0){
        for(int i=0;i<list_unrecal.size();i++){
            prm_dev = trs->GetPrmDev(list_unrecal.at(i));
            devName = tr("%1").fromStdString(ex_g2u(prm_dev->station_name)).trimmed();
            if(!list_dev_unrecal.contains(devName))
                list_dev_unrecal.push_back(devName);
        }
    }
    //计算电流数据的次场电压数据
    if(m_curr_station_used){
        vt_chs.resize(1);
        vt_chs[0] = ch_curr;
        Ts2Spect.CalToSectArrWithTrs(vt_chs, NULL, trs, &spect_array_i, false);
        currResultItem = spect_array_i.GetItem(3, 0);
        double currK = (currResultItem.item_data.u1 - currResultItem.item_data.u3)/2.0;
        current_value = currK/prm_meas->curr_resist;
    }
    trs->SetTdipResult(&spect_array_u, &spect_array_i);
    Ts2Spect.CalRes(trs, &spect_array_u, current_value);
    return true;
}

std::string CWdtTdipRes::GetTmtFileName(std::string file_trs,
                                       std::string dev_name)
{
    string file_tmt;
    string strTrs = ex_g2u(file_trs);
    QString strTmt, strPath, strDev, strBase;

    strPath = ui->edtTmtPath->text();

    strDev = tr("%1").fromStdString(dev_name);
    strBase = GetBaseNameFromFullFileName(tr("%1").fromStdString(strTrs));
    strTmt = tr("%1/%2/%3.tmt")
            .arg(strPath)
            .arg(strDev)
            .arg(strBase);
    file_tmt = ex_u2g(strTmt.toStdString());
    return file_tmt;
}

int CWdtTdipRes::GetLenForOneCycle()
{
    int len = (0.5+m_trs.GetPrmMeas()->sample_rate/m_trs.GetPrmMeas()->sig_freq);

    return len;
}

void CWdtTdipRes::UpdateTimeLabel(int sampleRate, double freq, int freqType)
{
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
    case 1://过零方波-25%占空
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
    if(max_l2.toFloat() > 5)
        max_l2 = tr("5");

    ui->label_time1->setText(tr("时间设置区间(0,%1)").arg(t1));
    if(freqType == 0){
        ui->label_time2->setText(tr("时间设置区间(0,%1)").arg(t2));
    }else{
        ui->label_time2->setText(tr("时间设置区间(%1,%2)").arg(t1).arg(t2));
    }
    ui->label_len1->setText(tr("长度设置区间为(0,%1)").arg(max_l1));
    ui->label_len2->setText(tr("长度设置区间为(0,%1)").arg(max_l2));
}

QString CWdtTdipRes::FormatValue(double value)
{
    QString format = tr("%1").arg(value);
    int index = format.indexOf(".");
    if(index <= 0)
        return format;
    return format.mid(0, index+4);
}
