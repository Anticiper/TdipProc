#ifndef TDIP_DATA_OUT_H
#define TDIP_DATA_OUT_H

#include <vector>
#include <list>
#include <string>
#include "sip_pub.h"
#include "data_limit.h"
#include "format_txt_site_coord.h"
#include "format_txt_elev.h"
#include "format_txt_shifting.h"
#include "tdip_result_cal.h"
#include "format_trs.h"
#include "tdip_def.h"

using std::string;
using std::vector;
using std::list;

class CTdipDataOut
{
public:
    typedef struct inv_info_s{
        float inv_thickness;  //反演厚度(单位为m)
        bool resistivity; //1-视电阻率，0-视电阻
        SipLayout_T layout; //装置类型(3极/4极)
        bool add_system_err; //是否加入系统误差
        float system_err_app; //加入的视电阻率相对误差
        float system_err_cp; //加入的相对相位误差
        bool is_xcoord_move;  //是否偏移x坐标
        float xcoord_move_value; //x坐标的偏移值
    }InvInfo_S;

    typedef struct measure_point_info_s{
        float Mx;//测量点M极X坐标
        float My;//测量点M极Y坐标
        int qualified_num;//测量点总合格数
        int total_num;//测量点总数
        float pass_rate;//合格率
    }MeasurePointInfo_S;

public:
    CTdipDataOut();

    void SetLimitInfo(vector<LimitItem_S> *vt_limit);
    vector<LimitItem_S>* GetLimitInfo(){return m_vt_limit;}
    void SetOutFile(const string &strOutFileName); //设置输出全路径文件名
    void SetInputFileBaseName(const string &strOutFileName); //设置输入SRS文件的基本文件名
    void SetShiftingXY();
    void SetCoord(CFormatTxtSiteCoord *Coord){m_coord = Coord;}
    void SetElevation(CFormatTxtElev *Elev){m_elev = Elev;}
    void SetInvInfo(const InvInfo_S &inv_info){m_inv_info=inv_info;} //设置反演输出信息
    void SetShiftingInfo(CFormatTxtShifting *Shifting){m_shifting=Shifting;}//设置相对坐标偏移列表信息
    void Open(); //打开文件
    void Close(); //关闭文件
    void ClearShifting();
    void SetType(bool isType3){m_type_3 = isType3;}//设置装置类型
    void SetSipLayout(SipLayout_T layout){sipLayout = layout;}

    //输出多个res的检查点误差或一致性误差，check_type
    void OutputErr(CFormatTRS res_arr[],  //多次供电的res数据结果(相同装置系数)
                   vector<string>  *vt_files, //多次供电的res不带路径和后缀的文件名
                   int arr_num);//供电次数
    //输出中梯扫描数据
    void OutputMidGradient(CFormatTRS *res, string file_name_srs, CFormatTRS *trs);
    //二维拟断面
    void OutputSect(CFormatTRS *res, CFormatTRS *trs);
    //二维反演
    void OutputRes2dinv(CFormatTRS *res, CFormatTRS *trs);
    //三维反演
    void OutputRes3dinv(CFormatTRS *srs);
    //三维反演高程数据导出
    void PrintRes3Topography();
    //初始化测量点信息
    void initMeasurePointInfo();
    void ResetPassRateData();
    //导出供电点合格率
    void OutPassRateForPower(string file_name_srs,CFormatTRS *trs);
    void PrintPassRateForPower(int idx_file, string file_name_trs, CFormatTRS *trs);
    //导出测量点合格率
    void OutPassRateForPoint();
private:
    int  GetPoleNum();
    void ReplaceDataCntInPrint(const string &file_name, int pos, int data_cnt); //将数据点数的统计信息打印到文件中对应的位置
    /* 将多个计算结果的通道按通道位置建立索引信息(函数会检查谐波数及装置系数，所有res必须一致)
     * vt_info: 返回的按通道位置升序排列的数组
    */
    void GetChMap(CFormatTRS res_arr[], int arr_num, list<ChNode_S> *list_chs, bool needCheckPosB);
    //判断通道是否有效
    bool IsChValid(int idx_ch_res, CTdipResultCal *res, CFormatTRS *trs, bool is_check_limit);
    //判断当前通道数据是否超过误差
    bool IsWithinLimits(const CTdipResultCal::ResItem_S *item);
    bool IsWithinLimitOne(float value, float err, const LimitItem_S &limit);
    //从结果文件中获取对应的值
    void GetDataFromRes(CFormatTRS res_arr[],
                                 const vector<ChPos_S> *vt_node,
                                 int type,//1:视电阻率2：视电阻率误差 3：视极化率4：视极化率误差
                                 vector<float> *vt_data);
    void PrintErrdHead(int max_acq_num); //所有通道中最大的重采次数
    void PrintErrBody(CFormatTRS res_arr[], //多次供电的结果数据
                      vector<string>  *vt_files, //多次供电的res不带路径和后缀的文件名
                      int arr_num, //供电次数
                      list<ChNode_S> *list_chs, //按通道m位置排列的索引信息
                      int max_acq_num);
    void PrintPlaneHead(int idx_file);
    void PrintPlaneBody(int idx_file, CFormatTRS *res, string file_name_srs, CFormatTRS *trs);
    void PrintSectionHead(int idx_file);
    void PrintSectionBody(int idx_file, CFormatTRS *res,  CFormatTRS *trs);
    void PrintRes2dHead(int idx_file, CFormatTRS *trs);
    void PrintRes2dBody(int idx_file,CFormatTRS *res, CFormatTRS *trs);
    void PrintRes3dHead(int idx_file, CFormatTRS *trs);
    void PrintRes3dBody(int idx_file, CFormatTRS *trs);

    void GetShiftingValue(int idx_ch, float &axShifting, float &ayShifting, float &bxShifting, float &byShifting,
                          float &mxShifting, float &myShifting, float &nxShifting, float &nyShifting);
    float GetPassRate(CFormatTRS *srs);
    void AddMeasurePoint(float mx, float my, int qualified);
private:
    bool m_first_out; //是否是打开输出后的第1次输出
    vector<LimitItem_S> *m_vt_limit;
    string m_base_file_name_out;
    string m_base_file_name_in;
    CFormatTxtSiteCoord *m_coord;
    CFormatTxtElev *m_elev;
    CFormatTxtShifting *m_shifting;
    vector<ShiftingInfo_S> m_shifting_info;
    InvInfo_S m_inv_info;
    vector<FILE*> m_vt_files; //打印数据的文件指针
    vector<string> m_vt_file_names; //所有打开的文件名
    vector<int> m_vt_pos_file_data_cnt; //文件填写数据点数的位置(小于0表示文件中无数据点信息)
    vector<int> m_vt_data_cnt; //数据点数统计值
    bool m_type_3;//是否为三级装置 true:三级 false：四极
    SipLayout_T sipLayout;//装置类型
    vector<MeasurePointInfo_S> m_measure_point;
    int qualifiedNum;//单个供电点的合格通道数
    int totalNum;//单个供电点的总通道数
};

#endif // TDIP_DATA_OUT_H
