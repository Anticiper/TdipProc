#ifndef WIDGET_MONITOR_CH_H
#define WIDGET_MONITOR_CH_H

#include <QWidget>
#include <QTimer>
#include <QMap>
#include <QColor>
#include "qcustomplot.h"
#include "format_tmt.h"

class PlotMonitor : public QCustomPlot
{
    Q_OBJECT
public:
    explicit PlotMonitor(QWidget *parent = 0);
    explicit PlotMonitor(int idx_ch, QWidget *parent = 0);

    void SetSmt(CFormatTmt *smt);

    QCPGraph *AddChannel(int idx_ch, const QColor &color = Qt::blue);
    QCPGraph *AddChannel(int idx_ch, const QString &name, const QColor &color = Qt::blue);

    void Clear(void);
    void Update(int idxStart, int nLen);
    bool Finish(void) const { return m_is_finish; }

    void EnableYLabel(bool  enable);
signals:
    void signalUpdated(int idx_ch, double dMinMv, double dMaxMv, double dStdMv);

public slots:
protected:
    void    init(void);

    double    get_min(const QVector<double> &datas) const;
    double    get_max(const QVector<double> &datas) const;
    double    get_std(const QVector<double> &datas) const;

private:
    void ResetYRange(double min, double max);

private:
    CFormatTmt *m_tmt;

//    int  m_x_length;
    bool m_is_finish;

    QMap<int, QCPGraph*> m_map_graphs;
    QMap<int, QVector<double> > m_map_datas;
};

#endif // MONITOR_CHANNEL_H
