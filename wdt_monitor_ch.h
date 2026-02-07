#ifndef WDT_MONITOR_CH_H
#define WDT_MONITOR_CH_H

#include <QWidget>
#include "plot_monitor.h"
#include <QLabel>

class WdtMonitorCh : public QWidget
{
    Q_OBJECT
public:
    explicit WdtMonitorCh(QWidget *parent = 0);

    void SetName(const QString &strName);
    PlotMonitor *GetPlot()const{return m_pPlot;}

private:
    void InitUi();

signals:

private slots:
    void onChUpdated(int idxCh, double dMinMv, double dMaxMv, double dStdMv);

private:
    PlotMonitor *m_pPlot;
    QLabel *m_pLbChName, *m_pLbStd, *m_pLbMin, *m_pLbMax;
    int m_idxCh;
};

#endif // WDT_MONITOR_CH_H
