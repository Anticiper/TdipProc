#ifndef FORMAT_TXT_SHIFTING_H
#define FORMAT_TXT_SHIFTING_H

#include <vector>
#include "format_txt.h"

using std::vector;

typedef struct shifting_info_s{
    string srs_name; //文件名称(可能只包含文件名的前部分字符)
    double x; //X坐标偏移值
    double y; //Y坐标偏移值
    int ex_id;//偏移的电道号 <0 标识所有通道都需要偏移
    bool change_a;//是否A供电点偏移
    bool change_b;//是否B供电点偏移
}ShiftingInfo_S;

class CFormatTxtShifting : public CFormatTxt
{

public:
    CFormatTxtShifting();

    //float GetShifting(float pos_x, float pos_y); //获取对应位置偏移
    void clear();

protected:
    /*从文件读出所有数据*/
    virtual bool ReadData(ifstream &stm);
    /*将所有数据写入文件*/
    virtual bool WriteData(ofstream &stm);

public:
    vector<ShiftingInfo_S> m_data;
};

#endif // FORMAT_TXT_SHIFTING_H
