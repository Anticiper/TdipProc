#ifndef FORMAT_TXT_ELEV_H
#define FORMAT_TXT_ELEV_H

#include <vector>
#include "format_txt.h"

using std::vector;

typedef struct elev_info_s{
    double x; //测点对应的X坐标
    double y; //测点对应的Y坐标
    double elev; //测点高程
}ElevInfo_S;

class CFormatTxtElev : public CFormatTxt
{

public:
    CFormatTxtElev();

    float GetElev(float pos_x, float pos_y); //获取对应位置的高程

protected:
    /*从文件读出所有数据*/
    virtual bool ReadData(ifstream &stm);
    /*将所有数据写入文件*/
    virtual bool WriteData(ofstream &stm);

public:
    vector<ElevInfo_S> m_data;
};

#endif // FORMAT_TXT_ELEV_H
