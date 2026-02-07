#include "cutils.h"
#include "plot_monitor.h"

PlotMonitor::PlotMonitor(QWidget *parent) :
    QCustomPlot(parent)
{
    init();
}

PlotMonitor::PlotMonitor(int idx_ch, QWidget *parent)
    :QCustomPlot(parent)
{
    init();

    AddChannel(idx_ch);
}

void PlotMonitor:: SetSmt(CFormatTmt *tmt)
{
    m_tmt = tmt;
}

void PlotMonitor::EnableYLabel(bool  enable)
{
    yAxis->setTickLabels(enable);
    yAxis->setTicks(enable);
    yAxis2->setTickLabels(enable);
    yAxis2->setTicks(enable);
    replot();
}

void PlotMonitor::Clear(void)
{
    QMap<int, QCPGraph*>::iterator itor_map = m_map_graphs.begin();
    for(; itor_map != m_map_graphs.end(); ++itor_map){
        QCPGraph *graph = itor_map.value();
        graph->clearData();
    }

    replot();
    m_map_datas.clear();
    m_is_finish = false;
}

void PlotMonitor::Update(int idxStart, int nLen)
{
    if(NULL == m_tmt) return;
    if(0 == nLen) return;

    uint32_t pos_start = 0;
    uint32_t len_ret  = 0;
    float *buf_r = new float[nLen];
    double *buf = new double[nLen];
    int i, range_x_min = pos_start;
    int range_x_max = range_x_min + nLen;
    double y_min, y_max;
    bool y_inited = false;
    bool is_updated = false;

    // CMonitorDataCh *monitor_ch = ;
    QMap<int, QCPGraph*>::iterator itor_graph = m_map_graphs.begin();
    for(; itor_graph != m_map_graphs.end(); ++itor_graph){
        int idx_ch = itor_graph.key();
        QCPGraph *graph = itor_graph.value();

        graph->clearData();
        m_map_datas[idx_ch].clear();

        if(m_tmt->GetDatasN(idx_ch, idxStart, nLen, buf_r)){
            len_ret = nLen;
            for(i=0; i<nLen; i++){
                buf[i] = buf_r[i];
            }
        }
        if(len_ret > 0){
            is_updated = true;

            QVector<double> x_vals;
            QVector<double> y_vals;
            for (uint32_t idx_pos = 0; idx_pos < len_ret; idx_pos++) {
                x_vals.push_back(pos_start + idx_pos);
                y_vals.push_back(buf[idx_pos]*1000); //change unit from V to mV
            }
            graph->addData(x_vals, y_vals);
            m_map_datas[idx_ch] += y_vals;
            graph->rescaleValueAxis();

            if(pos_start + len_ret > (uint32_t)range_x_max){
                range_x_max = pos_start + len_ret;
            }
        }
    }

    if(is_updated){
        if(range_x_max > range_x_min + nLen){
            range_x_min = range_x_max - nLen;
        }

        for(itor_graph = m_map_graphs.begin(); itor_graph != m_map_graphs.end(); ++itor_graph){
            QCPGraph *graph = itor_graph.value();
            int idx_ch = itor_graph.key();

            graph->removeDataBefore(range_x_min - 2 * nLen);
            if(m_map_datas[idx_ch].size() > nLen){
                m_map_datas[idx_ch].erase(m_map_datas[idx_ch].begin(), m_map_datas[idx_ch].end() - nLen);
            }

            double min = get_min(m_map_datas[idx_ch]);
            double max = get_max(m_map_datas[idx_ch]);
            double std = get_std(m_map_datas[idx_ch]);
            if(!y_inited){
                y_inited = true;
                y_min = min;
                y_max = max;
            }else{
                if(min < y_min)y_min = min;
                if(max > y_max)y_max = max;
            }
            emit signalUpdated(idx_ch, min, max, std);
        }
        ResetYRange(y_min, y_max);
        xAxis->setRange(range_x_min, range_x_max);
        replot();

        m_is_finish = false;
    } else {
        m_is_finish = true;
    }

    delete[] buf;
    delete[] buf_r;
}

QCPGraph *PlotMonitor::AddChannel(int idx_ch, const QColor &color)
{
    return AddChannel(idx_ch, tr("CH:%1").arg(idx_ch), color);
}

QCPGraph *PlotMonitor::AddChannel(int idx_ch, const QString &name, const QColor &color)
{
    QCPGraph *graph = addGraph();

    graph->setName(name);
    graph->setPen(QPen(color));
    graph->setAntialiasedFill(false);

    m_map_graphs[idx_ch] = graph;

    return graph;
}

void    PlotMonitor::init(void)
{
    m_tmt = NULL;

    setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

    xAxis->setAutoTickStep(true);
    axisRect()->setupFullAxesBox();

    connect(xAxis, SIGNAL(rangeChanged(QCPRange)), xAxis2, SLOT(setRange(QCPRange)));
    connect(yAxis, SIGNAL(rangeChanged(QCPRange)), yAxis2, SLOT(setRange(QCPRange)));
}

double    PlotMonitor::get_min(const QVector<double> &datas) const
{
    double min = 0.0;

    for (int i = 0; i < datas.size(); i++) {
        if(i == 0) min = datas[0];
        else {
            if(datas[i] < min) min = datas[i];
        }
    }

    return min;
}

double    PlotMonitor::get_max(const QVector<double> &datas) const
{
    double max = 0.0;

    for (int i = 0; i < datas.size(); i++) {
        if(i == 0) max = datas[0];
        else {
            if(datas[i] > max) max = datas[i];
        }
    }

    return max;
}

double    PlotMonitor::get_std(const QVector<double> &datas) const
{
    double sum = 0.0;
    double avg = 0.0;

    for (int i = 0; i < datas.size(); i++) {
        sum += datas[i];
    }

    avg = sum / datas.size();

    sum = 0.0;
    for (int i = 0; i < datas.size(); i++) {
        sum += (datas[i] - avg) * (datas[i] - avg);
    }

    return sqrt(sum / datas.size());
}

void PlotMonitor::ResetYRange(double min, double max)
{
    double rangeOld = fabs(yAxis->range().upper - yAxis->range().lower);
    double rangeNew = max-min;
    if(rangeNew < 0.5*rangeOld){
        yAxis->setRange(min, max);
    }
}
