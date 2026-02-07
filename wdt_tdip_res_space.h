#ifndef WDT_TDIP_RES_SPACE_H
#define WDT_TDIP_RES_SPACE_H

#include <QWidget>
#include <QWidget>
#include "format_trs.h"
#include "custom_plot.h"

namespace Ui {
class CWdtTdipResSpace;
}

class CWdtTdipResSpace : public QWidget
{
    Q_OBJECT

public:
    explicit CWdtTdipResSpace(QWidget *parent = 0);
    ~CWdtTdipResSpace();

    void SetGeoMeas(CFormatTRS *geo_meas);
    void Init();
    void onUpdateShowResult();

public slots:
    void onUpdateResPlot(); //绘制图形，并将曲线添加到数组中

private:
    void onUpdateAxis();
    void UpdatePlotAxisY();  //根据选择更新Y轴的显示
    void UpdatePlotAxisX();
    void UpdatePlotAxisYForOne(CCustomPlot *plot, int type);

    void ShowGraph(CCustomPlot *plot, const QVector<QCPGraph*> *graphs);
    void UpdateResPlot(CCustomPlot *plot, int type, QVector<QCPGraph*> *graphs);//更新视图，并将添加的曲线填充到graphs中
    QCPGraph *AddDataGraph(CCustomPlot *plot, int type); //将数据项添加视图中，返回添加的视图曲线

    //获得有计算结果的数据,获取结果的单位与绘图显示的单位相同
    void GetVtRes(int type, //数据类型
                          QVector<double> &VtX, //X坐标对应的数组
                          QVector<double> &VtY, //Y坐标对应的数组
                          QVector<double> &VtStd); //Y坐标对应的方差
    void SortByXUp(QVector<double> &VtX, //返回的频率数组
                     QVector<double> &VtY, //返回的计算结果数组
                     QVector<double> &VtStd); //返回的计算结果均方差数组
private:
    Ui::CWdtTdipResSpace *ui;

    CFormatTRS *m_geo_meas;
    QVector<QCPGraph*> m_graphs_plot1; //振幅视图中的曲线，每个选项对应一个
    QVector<QCPGraph*> m_graphs_plot2; //相位视图中的曲线，每个选项对应一个
};

#endif // WDT_TDIP_RES_SPACE_H
