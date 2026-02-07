#include "wdt_main.h"
#include "ui_wdt_main.h"
#include "qt_pub.h"
#include "ex_code.h"

CWdtMain::CWdtMain(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CWdtMain)
{
    ui->setupUi(this);
    setWindowTitle(tr("TdipProc-V%1.%2.%3").arg(VERSION_MAJ).arg(VERSION_MID).arg(VERSION_MIN));

    m_model_files = new QStringListModel(this);
    ui->listOpenFiles->setModel(m_model_files);

    ui->splitter->setStretchFactor(0, 2);
    ui->splitter->setStretchFactor(1, 3);

    m_wdt_output = new CWdtOutPut(&m_path_files_trs,this);
    m_wdt_prm_trs = new CWdtParamTrs(this);
    m_wdt_tdip_res = new CWdtTdipRes(this);
    m_wdt_gps_pos = new CWdtGpsPos(this);
    m_wdt_ts_plot = new CWdtTsPlot(this);
    m_wdt_gps_pos->ResetVtPos(&m_vt_pos);

    ui->tabRight->addTab(m_wdt_output, tr("数据导出"));
    ui->tabRight->addTab(m_wdt_prm_trs, tr("TRS参数编辑"));
    ui->tabRight->addTab(m_wdt_tdip_res, tr("采集结果"));
    ui->tabRight->addTab(m_wdt_ts_plot, tr("时序显示"));
    ui->tabRight->addTab(m_wdt_gps_pos, tr("采集站分布"));

    UpdateGrpFileTitle();

    connect(ui->btnImportData, SIGNAL(clicked()), this, SLOT(onBtnImportData()));
    connect(ui->btnClearFile, SIGNAL(clicked()), this, SLOT(onBtnClearData()));
    connect(ui->listOpenFiles, SIGNAL(clicked(QModelIndex)), this, SLOT(onListClicked(QModelIndex)));
}

CWdtMain::~CWdtMain()
{
    delete ui;
}

void CWdtMain::UpdateFilesFromPathFiles(const QStringList &paths_files, QStringList &files)
{
    QString strName;
    files.clear();
    for(int i=0; i<paths_files.size(); i++){
        strName = GetFileNameFromFullFileName(paths_files.at(i));
        files.append(strName);
    }
}

void CWdtMain::UpdateGrpFileTitle()
{
    ui->grpPowerFileList->setTitle(tr("供电数据文件，共%1个").arg(m_path_files_trs.size()));
}

void CWdtMain::onBtnClearData()
{
    QStringList strFileList;

    m_path_files_trs.clear();
    m_model_files->setStringList(strFileList);
    m_wdt_output->ClearData();
    m_wdt_prm_trs->UpdateShow(tr(""));
    m_wdt_tdip_res->UpdateShow(tr(""));
    m_wdt_ts_plot->UpdateShow(tr(""));

    ClearPos(&m_vt_pos);
    m_wdt_gps_pos->UpdateDataShow(true);

    UpdateGrpFileTitle();
}

void CWdtMain::onBtnImportData()
{
    static QString strPath;
    QStringList strFileList;
    QString suffix = tr("%1").arg(CFormatTRS::Suffix().data());
    QFileDialog dlg(this,
                    tr("Open %1").arg(suffix),
                    strPath,
                    tr("%1 Files (*.%1)").arg(suffix));
    dlg.setFileMode(QFileDialog::ExistingFiles);
    if(QDialog::Rejected == dlg.exec())
        return;
    strFileList = dlg.selectedFiles();
    if(strFileList.size()==0)
        return;
    strPath = GetPathFromFullFileName(strFileList.at(0));
    m_path_files_trs += strFileList;
    UpdateFilesFromPathFiles(m_path_files_trs, strFileList);
    m_model_files->setStringList(strFileList);
    UpdateGrpFileTitle();

    m_wdt_output->ResetPoleCmp();
    m_wdt_prm_trs->SetTrsFileList(m_path_files_trs);
    m_wdt_tdip_res->SetTrsFileList(m_path_files_trs);
}

void CWdtMain::onListClicked(const QModelIndex &index)
{
    QString strTrsFileName;
    CFormatTRS trs;

    if(index.isValid()){
        if(index.row()>=0 && index.row()<m_path_files_trs.size()){
            strTrsFileName = m_path_files_trs.at(index.row());
        }
    }
    m_wdt_prm_trs->UpdateShow(strTrsFileName);
    m_wdt_tdip_res->UpdateShow(strTrsFileName);
    m_wdt_ts_plot->UpdateShow(strTrsFileName);

    ClearPos(&m_vt_pos);
    if(ReadTrs(strTrsFileName, &trs)){
        GetPosFromTrs(&trs, &m_vt_pos);
    }
    m_wdt_gps_pos->ResetVtPos(&m_vt_pos);
    m_wdt_gps_pos->UpdateDataShow(true);
}

void CWdtMain::GetPosFromTrs(CFormatTRS *trs, QVector<CItemGpsPos *> *vt_pos)
{
    int i, num, ch_idx;
    CItemGpsPos *item = NULL;
    TdipPrmDev_S *prm = NULL;
    bool reverse; //是否反序放置(0-仪器放在M位置，1-仪器放在N位置)

    num = trs->m_vt_prm_dev.size();
    assert(trs->m_gps_lon.size() == num);
    assert(trs->m_gps_lat.size() == num);
    assert(trs->m_gps_alti.size() == num);

    TdipPrmMeas_S *prm_meas = trs->GetPrmMeas();
//    bool isAllGradient = (prm_meas->layout_type == SIP_ALL_GRADIENT);

    for(i=0; i<num; i++){
        prm = &trs->m_vt_prm_dev[i];
        reverse = IsChReverse(prm);
        ch_idx = GetFirstValidChIdx(prm, false);
        if(ch_idx < 0)
            continue;
        item = new CItemGpsPos(tr("%1").arg(prm->station_name));
        if(trs->m_prm_meas.curr_station_used){
            if(strcmp(trs->m_prm_meas.curr_station_name, prm->station_name)==0){
                item->SetChecked(false); //默认不选择电流站
            }
        }
        if(reverse){  //反向放置时，仪器位于第一有效道的N电极处
            item->SetPosX(prm->pos_n_x[ch_idx]);
            item->SetPosY(prm->pos_n_y[ch_idx]);
        }else{ //正向放置时，仪器位于第一有效道的M电极处
            item->SetPosX(prm->pos_m_x[ch_idx]);
            item->SetPosY(prm->pos_m_y[ch_idx]);
        }
        item->SetGpsLon(trs->m_gps_lon[i]);
        item->SetGpsLat(trs->m_gps_lat[i]);
        item->SetGpsAlti(trs->m_gps_alti[i]);
//        if(isAllGradient){
//            item->SetSipChannelPolePoint(srs->m_sip_pole_point);
//            item->SetSipDevChannel(GetSipDevChannel(prm));
//        }
        vt_pos->append(item);
    }
}

bool CWdtMain::ReadTrs(const QString strTrsFileName, CFormatTRS *trs)
{
    string strFile = ex_u2g(strTrsFileName.toStdString());

    if(strTrsFileName.isEmpty())
        return false;
    if(!trs->ReadFile(strFile.data())){
        QMessageBox::warning(this, tr("提示"), tr("打开文件“%1”失败！").arg(strTrsFileName));
        return false;
    }
    return true;
}

void CWdtMain::ClearPos(QVector<CItemGpsPos*> *vt_pos)
{
    int i=0;

    for(i=0; i<vt_pos->size(); i++){
        delete vt_pos->at(i);
    }
    vt_pos->clear();
}

bool CWdtMain::IsChReverse(const TdipPrmDev_S *prm)
{
    bool reverse = false;
    int idx_first, idx_last;

    idx_first = GetFirstValidChIdx(prm, false);
    idx_last = GetFirstValidChIdx(prm, true);
    if(idx_first>=0 && idx_last>=0){
        reverse = (prm->ch_seq_idx[idx_first] > prm->ch_seq_idx[idx_last]);
    }
    return reverse;
}

int CWdtMain::GetFirstValidChIdx(const TdipPrmDev_S *prm, bool reverse)
{
    int idx_ch = -1;
    int i;

    if(!reverse){ //正向查找
        for(i=0; i<MAX_CH_NUM; i++){
            if(prm->ch_type[i] != CH_TYPE_INVALID){
                idx_ch = i;
                break;
            }
        }
    }else{//反向查找
        for(i=MAX_CH_NUM-1; i>=0; i--){
            if(prm->ch_type[i] != CH_TYPE_INVALID){
                idx_ch = i;
                break;
            }
        }
    }
    return idx_ch;
}
