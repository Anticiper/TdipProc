#include "qt_pub.h"
#include "wdt_ts_plot.h"
#include "ui_wdt_ts_plot.h"
#include "ex_code.h"

CWdtTsPlot::CWdtTsPlot(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CWdtTsPlot)
{
    ui->setupUi(this);

    m_trs_inited = false;

    m_VtWdtCh.push_back(ui->plotCH1);
    m_VtWdtCh.push_back(ui->plotCH2);
    m_VtWdtCh.push_back(ui->plotCH3);
    m_VtWdtCh.push_back(ui->plotCH4);
    m_VtWdtCh.push_back(ui->plotCH5);
    m_VtWdtCh.push_back(ui->plotCH6);
    ShowPlotByCheck(ui->chkCH1, ui->plotCH1);
    ShowPlotByCheck(ui->chkCH2, ui->plotCH2);
    ShowPlotByCheck(ui->chkCH3, ui->plotCH3);
    ShowPlotByCheck(ui->chkCH4, ui->plotCH4);
    ShowPlotByCheck(ui->chkCH5, ui->plotCH5);
    ShowPlotByCheck(ui->chkCH6, ui->plotCH6);
    for(int i=0; i<m_VtWdtCh.size(); i++){
        m_VtWdtCh.at(i)->GetPlot()->AddChannel(i);
        m_VtWdtCh.at(i)->GetPlot()->SetSmt(&m_tmt);
    }

    UpdateUiState();

    connect(ui->cmbStation, SIGNAL(currentIndexChanged(int)), this, SLOT(onCmbStationSel(int)));
    connect(ui->btnChangeTsDir, SIGNAL(clicked()), this, SLOT(onBtnChangeDir()));
    connect(ui->btnTsSectPrev, SIGNAL(clicked()), this, SLOT(onBtnPrevClicked()));
    connect(ui->btnTsSectNext, SIGNAL(clicked()), this, SLOT(onBtnNextClicked()));
    connect(ui->chkCH1, SIGNAL(clicked(bool)), this, SLOT(on_chkCH1_clicked(bool)));
    connect(ui->chkCH2, SIGNAL(clicked(bool)), this, SLOT(on_chkCH2_clicked(bool)));
    connect(ui->chkCH3, SIGNAL(clicked(bool)), this, SLOT(on_chkCH3_clicked(bool)));
    connect(ui->chkCH4, SIGNAL(clicked(bool)), this, SLOT(on_chkCH4_clicked(bool)));
    connect(ui->chkCH5, SIGNAL(clicked(bool)), this, SLOT(on_chkCH5_clicked(bool)));
    connect(ui->chkCH6, SIGNAL(clicked(bool)), this, SLOT(on_chkCH6_clicked(bool)));
}

CWdtTsPlot::~CWdtTsPlot()
{
    delete ui;
}

void CWdtTsPlot::UpdateShow(QString strTrsFileName)
{
    int i, num;
//    SipPrmMeas_S *prm = NULL;
    string strFile = ex_u2g(strTrsFileName.toStdString());

    m_VtStationNi.clear();
    if(strTrsFileName.isEmpty()){
        m_trs_inited = false;
    }else{
        if(!m_trs.ReadFile(strFile.data())){
            QMessageBox::warning(this, tr("提示"), tr("打开文件“%1”失败！").arg(strTrsFileName));
            m_trs_inited = false;
        }else{
            //添加电流站
//            prm = m_trs.GetPrmMeas();
//            if(prm->curr_station_used){
//                m_VtStationNi.push_back(prm->curr_station_name);
//            }
            //添加电压站
            num = m_trs.GetDevNum();
            for(i=0; i<num; i++){
                m_VtStationNi.push_back(tr(m_trs.GetPrmDev(i)->station_name));
            }
            m_trs_inited = true;
        }
    }
    ResetDataAndShow();
}

void CWdtTsPlot::UpdateUiState()
{
    bool bTrsSel = m_trs_inited;
    bool bStSel = false;
    if(bTrsSel){
        bStSel = (ui->cmbStation->currentIndex()>=0);
    }
    ui->cmbStation->setEnabled(bTrsSel);
    ui->btnTsSectPrev->setEnabled(bTrsSel && bStSel);
    ui->btnTsSectNext->setEnabled(bTrsSel && bStSel);
    ui->edtStart->setEnabled(bTrsSel && bStSel);
    ui->edtShowLen->setEnabled(bTrsSel && bStSel);
}

void CWdtTsPlot::ShowPlotByCheck(QCheckBox *chk, WdtMonitorCh *plot)
{
    plot->setHidden(!chk->isChecked());
    plot->SetName(chk->text());
}

void CWdtTsPlot::UpdateCmbStation()
{
    int nCnt, iSel;
    QString strName;

    nCnt = m_VtStationNi.size();
    iSel = ui->cmbStation->currentIndex();
    ui->cmbStation->blockSignals(true);
    ui->cmbStation->clear();
    for(int i=0; i<nCnt; i++){
        strName = tr("%1").arg(m_VtStationNi.at(i));
        ui->cmbStation->addItem(strName);
    }
    if(iSel>=0 && iSel<nCnt)
        ui->cmbStation->setCurrentIndex(iSel);
    ui->cmbStation->blockSignals(false);
}

void CWdtTsPlot::CheckAndUpdatePrm(int &idxStart, int &nLen)
{
    int nMaxRow;

    nMaxRow = m_tmt.GetRowNum();
    //检查参数并调整
    if(idxStart < 0)
        idxStart=0;
    if(idxStart >= nMaxRow)
        idxStart = nMaxRow;
    if(nLen < 0)
        nLen = 0;
    if(nLen+idxStart > nMaxRow)
        nLen = nMaxRow-idxStart;
}

void CWdtTsPlot::ResetDataAndShow()
{
    UpdateCmbStation();
    if(m_trs_inited){
        ReadSmtAndUpdateShow();
    }else{
        ui->lbTsInfo->setText(tr("未选择TRS文件！"));
        for(int i=0; i<m_VtWdtCh.size(); i++){
            m_VtWdtCh.at(i)->GetPlot()->Clear();
        }
    }
    UpdateUiState();
}

bool CWdtTsPlot::ReadSmt(const QString strNi)
{
    bool ret = true;
    string file_smt, file_trs;

    file_trs = m_trs.GetFullFileName();
    file_smt = GetSmtFileName(file_trs, strNi.toStdString());
    if(!m_tmt.ReadFile(file_smt.data()))
        ret = false;
    return ret;
}

void CWdtTsPlot::UpdateStInfo()
{
    int nRow, nSec;
    int nSampleRate = m_tmt.GetPrmMeas().sample_rate;//15;
    QString strShow;

    nRow = m_tmt.GetRowNum();
    nSec = nRow/nSampleRate;
    strShow = tr("").sprintf("采样率=%d,点数=%d,时长=%dm%ds",
                 nSampleRate, nRow, nSec/60, nSec%60);
    ui->lbTsInfo->setText(strShow);
}

void CWdtTsPlot::UpdatePrmAndPlot(CWdtTsPlot::UpdateType_S type)
{
    int idxStart, nLen;

    idxStart = ui->edtStart->text().toInt();
    nLen = ui->edtShowLen->text().toInt();
    switch(type){
    case TYPE_CURR:
        break;
    case TYPE_PREV:
        idxStart -= nLen;
        break;
    case TYPE_NEXT:
        idxStart += nLen;
        break;
    }
    CheckAndUpdatePrm(idxStart, nLen);
    for(int i=0; i<m_VtWdtCh.size(); i++){
        m_VtWdtCh.at(i)->GetPlot()->Update(idxStart, nLen);
    }
    if(idxStart < m_tmt.GetRowNum())
        ui->edtStart->setText(tr("%1").arg(idxStart));
//    ui->edtShowLen->setText(tr("%1").arg(nLen));
}

std::string CWdtTsPlot::GetSmtFileName(std::string file_trs, std::string dev_name)
{
    string file_smt;
    string strTrs = ex_g2u(file_trs);
    QString strSmt, strPath, strDev, strBase;

//    strPath = GetPathFromFullFileName(tr("").fromStdString(strTrs));
    strPath = ui->edtTsDir->text();
    strDev = tr("").fromStdString(dev_name);
    strBase = GetBaseNameFromFullFileName(tr("").fromStdString(strTrs));
    strSmt = tr("%1/%2/%3.tmt")
            .arg(strPath)
            .arg(strDev)
            .arg(strBase);
    file_smt = ex_u2g(strSmt.toStdString());
    return file_smt;
}

bool CWdtTsPlot::ReadSmtAndUpdateShow()
{
    bool ret = true;
    QString strNi;

    strNi = ui->cmbStation->currentText();
    if(ReadSmt(strNi)){
        UpdateStInfo();
        UpdatePrmAndPlot(TYPE_CURR);
    }else{
        ui->lbTsInfo->setText(tr("SMT文件读取失败！"));
        for(int i=0; i<m_VtWdtCh.size(); i++){
            m_VtWdtCh.at(i)->GetPlot()->Clear();
        }
        ret = false;
    }
    return ret;
}

void CWdtTsPlot::onBtnChangeDir()
{
    QString strPathSmt;
    string strTmp;
    QString strFullFileNameTrs;

    strPathSmt = ui->edtTsDir->text();
    if(strPathSmt.isEmpty()){
        strTmp = ex_g2u(m_trs.GetFullFileName());
        strFullFileNameTrs = tr("").fromStdString(strTmp);
        strPathSmt = GetPathFromFullFileName(strFullFileNameTrs);
    }
    strPathSmt = QFileDialog::getExistingDirectory(this,
                                                     tr("选择SMT目录"),
                                                     strPathSmt);
    ui->edtTsDir->setText(strPathSmt);
}

void CWdtTsPlot::onBtnPrevClicked()
{
    UpdatePrmAndPlot(TYPE_PREV);
}

void CWdtTsPlot::onBtnNextClicked()
{
    UpdatePrmAndPlot(TYPE_NEXT);
}

void CWdtTsPlot::on_chkCH1_clicked(bool checked)
{
    ui->plotCH1->setHidden(!checked);
}

void CWdtTsPlot::on_chkCH2_clicked(bool checked)
{
    ui->plotCH2->setHidden(!checked);
}

void CWdtTsPlot::on_chkCH3_clicked(bool checked)
{
    ui->plotCH3->setHidden(!checked);
}

void CWdtTsPlot::on_chkCH4_clicked(bool checked)
{
    ui->plotCH4->setHidden(!checked);
}

void CWdtTsPlot::on_chkCH5_clicked(bool checked)
{
    ui->plotCH5->setHidden(!checked);
}

void CWdtTsPlot::on_chkCH6_clicked(bool checked)
{
    ui->plotCH6->setHidden(!checked);
}

void CWdtTsPlot::onCmbStationSel(int iSel)
{
    if(iSel<0 || iSel>=m_VtStationNi.size())
        return;
    ReadSmtAndUpdateShow();
}
