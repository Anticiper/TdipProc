#include <QMessageBox>
#include <QFileDialog>
#include "wdt_tdip_res.h"
#include "ui_wdt_tdip_res.h"
#include "ex_code.h"
#include "qt_pub.h"
#include "shared_data.h"

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
    }else{
        m_wdt_res_table->Clear();
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
