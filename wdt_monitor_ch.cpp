#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QString>
#include "wdt_monitor_ch.h"

WdtMonitorCh::WdtMonitorCh(QWidget *parent) :
    QWidget(parent)
{
    InitUi();
    connect(m_pPlot, SIGNAL(signalUpdated(int,double,double,double)),
            this, SLOT(onChUpdated(int,double,double,double)));
    onChUpdated(0,0,0,0);
}

void WdtMonitorCh::SetName(const QString &strName)
{
    m_pLbChName->setText(strName);
}

void WdtMonitorCh::InitUi()
{
    m_pPlot = new PlotMonitor(this);
    m_pLbChName =new QLabel(tr("通道号"), this);
    m_pLbStd = new QLabel(tr("均方差"), this);
    m_pLbMin = new QLabel(tr("最小值"),this);
    m_pLbMax = new QLabel(tr("最大值"),this);
    QSizePolicy Policy= m_pPlot->sizePolicy();
    Policy.setHorizontalPolicy(QSizePolicy::Expanding);
    Policy.setVerticalPolicy(QSizePolicy::Expanding);
    m_pPlot->setSizePolicy(Policy);
    QVBoxLayout *pLytRigth = new QVBoxLayout(NULL);
    pLytRigth->addStretch();
    pLytRigth->addWidget(m_pLbChName);
    pLytRigth->addWidget(m_pLbStd);
    pLytRigth->addWidget(m_pLbMin);
    pLytRigth->addWidget(m_pLbMax);
    pLytRigth->addStretch();
    QHBoxLayout *pLytMain = new QHBoxLayout(this);
    pLytMain->addWidget(m_pPlot);
    pLytMain->addLayout(pLytRigth);
    pLytMain->setMargin(0);
}

void WdtMonitorCh::onChUpdated(int idxCh, double dMinMv, double dMaxMv, double dStdMv)
{
    (void)idxCh;
//    m_pLbStd->setText(tr("").sprintf("STD:%9.2EmV",dStdMv));
//    m_pLbMin->setText(tr("").sprintf("MIN:%9.2EmV",dMinMv));
//    m_pLbMax->setText(tr("").sprintf("MAX:%9.2EmV",dMaxMv));
    m_pLbStd->setText(tr("").sprintf("STD:%.4fmV",dStdMv));
    m_pLbMin->setText(tr("").sprintf("MIN:%.4fmV",dMinMv));
    m_pLbMax->setText(tr("").sprintf("MAX:%.4fmV",dMaxMv));
}
