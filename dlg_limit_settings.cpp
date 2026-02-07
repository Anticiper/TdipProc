#include "dlg_limit_settings.h"
#include "ui_dlg_limit_settings.h"

CdlgLimitSettings::CdlgLimitSettings(CDataThreshold *threshold, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CdlgLimitSettings)
{
    ui->setupUi(this);
    setWindowTitle(tr("误差设置"));

    m_threshold = threshold;
    UpdateData(false);

    connect(ui->chkErrAppr, SIGNAL(clicked()), this, SLOT(onChkApprErr()));
    connect(ui->chkErrAppc, SIGNAL(clicked()), this, SLOT(onChkAppcErr()));
    connect(ui->btnRestore, SIGNAL(clicked()), this, SLOT(onBtnRestoreDefault()));
    connect(ui->btnSure, SIGNAL(clicked()), this, SLOT(onBtnOk()));
    connect(ui->btnCancel, SIGNAL(clicked()), this, SLOT(onBtnCancel()));
}

CdlgLimitSettings::~CdlgLimitSettings()
{
    delete ui;
}

void CdlgLimitSettings::UpdateData(bool bSaveOrShow)
{
    if(bSaveOrShow){  //保存UI输入到数据
        if(!ui->chkErrAppr->isChecked()){
            m_threshold->DisableErrCp(CDataThreshold::DATA_TYPE_APP);
        }else{
            m_threshold->SetErrCp(CDataThreshold::DATA_TYPE_APP,
                                  ui->edtApprErr->text().toFloat());
        }
        if(!ui->chkErrAppc->isChecked()){
            m_threshold->DisableErrCp(CDataThreshold::DATA_TYPE_APPC);
        }else{
            m_threshold->SetErrCp(CDataThreshold::DATA_TYPE_APPC,
                                   ui->edtAppcErr->text().toFloat());
        }
    }else{ //将数据显示到UI
        ui->chkErrAppr->setChecked(m_threshold->IsValidErrCp(CDataThreshold::DATA_TYPE_APP));
        ui->chkErrAppc->setChecked(m_threshold->IsValidErrCp(CDataThreshold::DATA_TYPE_APPC));
        ui->edtApprErr->setText(tr("").sprintf("%.1f",
                                              m_threshold->GetErrCp(CDataThreshold::DATA_TYPE_APP)));
        ui->edtAppcErr->setText(tr("").sprintf("%.1f",
                                              m_threshold->GetErrCp(CDataThreshold::DATA_TYPE_APPC)));
        UpdateUiState();
    }
}

void CdlgLimitSettings::UpdateUiState()
{
    ui->edtApprErr->setEnabled(ui->chkErrAppr->isChecked());
    ui->edtAppcErr->setEnabled(ui->chkErrAppc->isChecked());
}

void CdlgLimitSettings::onBtnRestoreDefault()
{
    m_threshold->DisableAll();
    m_threshold->SetErrCp(CDataThreshold::DATA_TYPE_APP, 1.0);
    m_threshold->SetErrCp(CDataThreshold::DATA_TYPE_CP, 0.5);

    UpdateData(false);
}

void CdlgLimitSettings::onBtnOk()
{
    UpdateData(true);
    accept();//返回QDialog::Accepted
    close();
}

void CdlgLimitSettings::onBtnCancel()
{
    reject(); //返回QDialog::Rejected
    close();
}

void CdlgLimitSettings::onChkApprErr()
{
    UpdateUiState();
}

void CdlgLimitSettings::onChkAppcErr()
{
    UpdateUiState();
}

