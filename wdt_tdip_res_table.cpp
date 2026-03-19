#include "wdt_tdip_res_table.h"
#include "ui_wdt_tdip_res_table.h"

CWdtTdipResTable::CWdtTdipResTable(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CWdtTdipResTable)
{
    ui->setupUi(this);

    m_model_res = NULL;
    m_geo_meas = NULL;
}

CWdtTdipResTable::~CWdtTdipResTable()
{
    delete ui;
}

void CWdtTdipResTable::SetGeoMeas(CFormatTRS *geo_meas)
{
    m_geo_meas = geo_meas;
    m_model_res->SetGeoMeas(geo_meas);
}

void CWdtTdipResTable::Clear()
{
    if(m_model_res != NULL){
        m_model_res->ClearData();
        emit m_model_res->layoutChanged();
    }
}

void CWdtTdipResTable::Init(CDataThreshold *threshold)
{
    m_model_res = new ModelTdipRes(this);
    m_model_res->SetColItemsDisplay(m_model_res->GetShowColItems());
    ui->tbRes->setModel(m_model_res);
    //ui->tbRes->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_model_res->SetThreshold(threshold);
}

void CWdtTdipResTable::onUpdateShowResult()
{
    emit m_model_res->layoutChanged();
    ui->tbRes->resizeColumnsToContents();
    //ui->lbCurrent->setText(tr("").sprintf("%.4f", m_geo_meas->GetCurrentValue()));
}
