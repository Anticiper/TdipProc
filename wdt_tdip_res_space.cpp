#include "wdt_tdip_res_space.h"
#include "ui_wdt_tdip_res_space.h"

CWdtTdipResSpace::CWdtTdipResSpace(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CWdtTdipResSpace)
{
    ui->setupUi(this);
    m_geo_meas = NULL;
}

CWdtTdipResSpace::~CWdtTdipResSpace()
{
    delete ui;
}

void CWdtTdipResSpace::SetGeoMeas(CFormatTRS *geo_meas)
{
    m_geo_meas = geo_meas;
}

void CWdtTdipResSpace::Init()
{
    onUpdateAxis();
    connect(ui->chkShowErrBar, SIGNAL(clicked()), this, SLOT(onUpdateResPlot()));
    connect(ui->chkLine, SIGNAL(clicked()), this, SLOT(onUpdateResPlot()));
}

void CWdtTdipResSpace::onUpdateAxis()
{
    UpdatePlotAxisX();
    UpdatePlotAxisY();
}

void CWdtTdipResSpace::onUpdateResPlot()
{
    UpdateResPlot(ui->wdtPlotAmp, 1, &m_graphs_plot1);
    UpdateResPlot(ui->wdtPlotAmc, 2, &m_graphs_plot2);
}

void CWdtTdipResSpace::UpdatePlotAxisX()
{
    ui->wdtPlotAmp->xAxis->setLabel(tr("M位置(m)"));
    ui->wdtPlotAmc->xAxis->setLabel(tr("M位置(m)"));
}

void CWdtTdipResSpace::UpdatePlotAxisY()
{
    UpdatePlotAxisYForOne(ui->wdtPlotAmp, 1);
    UpdatePlotAxisYForOne(ui->wdtPlotAmc, 2);
}

void CWdtTdipResSpace::onUpdateShowResult()
{
    onUpdateResPlot();
    //ui->lbCurrent->setText(tr("").sprintf("%.4f", m_geo_meas->GetCurrentValue()));
}

void CWdtTdipResSpace::UpdatePlotAxisYForOne(CCustomPlot *plot, int type)
{
    plot->SetRescaleType(CCustomPlot::AXIS_Y, CCustomPlot::RSC_CUSTOM_RANGE);
    switch(type){
    case 1:
        plot->yAxis->setLabel(tr("视电阻率(Ω·m)"));
        plot->SetYScaleType(AXIS_TYPE_LOG);
        plot->SetRescaleCustomRange(CCustomPlot::AXIS_Y,QPair<double,double>(1,1E11));
        break;
    case 2:
        plot->yAxis->setLabel(tr("视极化率(%)"));
        plot->SetRescaleCustomRange(CCustomPlot::AXIS_Y,QPair<double,double>(-100,100));
        break;
    }
}

void CWdtTdipResSpace::ShowGraph(CCustomPlot *plot, const QVector<QCPGraph*> *graphs)
{
    int num = graphs->size();

    for(int i=0; i<num; i++){
        graphs->at(i)->setVisible(true);
    }
    plot->replot();
}

void CWdtTdipResSpace::UpdateResPlot(CCustomPlot *plot,
                                int res_type,
                                QVector<QCPGraph *> *graphs)
{
    QCPGraph *graph_new;

    for(int idx=0; idx<graphs->size(); idx++){
        plot->removeGraph(graphs->at(idx));
    }
    graphs->clear();
    graph_new = AddDataGraph(plot, res_type);
    graphs->append(graph_new);
    plot->rescaleAxes1();
    ShowGraph(plot, graphs);
}

QCPGraph *CWdtTdipResSpace::AddDataGraph(CCustomPlot *plot, int type)
{
    QVector<double> VtX1, VtY1, VtStd1;
    QPen pen(QColor(0, 85, 255));
    QCPGraph *graph = NULL;
    QCPGraph::LineStyle line_add = ui->chkLine->isChecked()?QCPGraph::lsLine:QCPGraph::lsNone;
    QCPScatterStyle scat = QCPScatterStyle::ssDisc;

    GetVtRes(type, VtX1, VtY1, VtStd1);
    SortByXUp(VtX1, VtY1, VtStd1);
//    if(ui->chkShowErrBar->isChecked())
//        graph = plot->addGraph1(VtX1, VtY1, VtStd1, tr(""), pen, line_add, scat);
//    else
        graph = plot->addGraph1(VtX1, VtY1, tr(""), pen, line_add, scat);
    return graph;
}

void CWdtTdipResSpace::GetVtRes(int type,
                                   QVector<double> &VtX,
                                   QVector<double> &VtY,
                                   QVector<double> &VtStd)
{
    CTdipResultCal *res =  m_geo_meas->GetResultCal();
    CTdipResultCal::ResItem_S item;
     double pos_m;
    int cnt = m_geo_meas->GetRes()->DoGetBuffCalNum();
    CResult::ChInfo_S ch_info;

    VtX.clear();
    VtY.clear();
    VtStd.clear();
    for(int iCh=0; iCh<res->GetChNum(); iCh++){
        ch_info = res->GetChInfo(iCh);
        item = res->GetResItem(iCh, 0, 0);
        //根据计算类型赋值参数
        pos_m = ch_info.pos_m;
        //pos_m = PubGetPoleMPos(prm_meas->first_pole_x, prm_meas->mn_len, ch_info.idx_seq);
        //将计算结果添加到输出数组中
        if(cnt > 0){
            VtX.append(pos_m);
            if(type == 1){
                VtY.append(item.appr_value);
                VtStd.append(item.appr_err_value);
            }else if(type == 2){
                VtY.append(item.appc_value);
                VtStd.append(item.appc_err_value);
            }
        }
    }
}

void CWdtTdipResSpace::SortByXUp(QVector<double> &VtX, QVector<double> &VtY, QVector<double> &VtStd)
{
    int num = VtX.size();
    int i,j,ret;
    double Freq,Value,Std;

    for(i=0; i<num-1; i++) {
        ret = 0;
        for(j=0; j<num-i-1; j++) {
            if(VtX[j]>VtX[j+1]) {
                Freq = VtX[j];
                VtX[j] = VtX[j+1];
                VtX[j+1] = Freq;

                Value = VtY[j];
                VtY[j] = VtY[j+1];
                VtY[j+1] = Value;

                Std = VtStd[j];
                VtStd[j] = VtStd[j+1];
                VtStd[j+1] = Std;
                ret = 1;
            }
        }
        if(ret == 0)
            break;
    }
}
