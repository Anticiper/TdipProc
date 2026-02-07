#ifndef FORMAT_TXT_SITE_COORD_H
#define FORMAT_TXT_SITE_COORD_H

#include "format_txt.h"

class CFormatTxtSiteCoord : public CFormatTxt
{
public:
    typedef struct site_coord_s{
        string site_name; //测点名称
        int   line_no;  //测线号
        double x_c; //测点在测线上的相对坐标
        double x; //测点对应的绝对X坐标
        double y; //测点对应的绝对Y坐标
    }SiteCoord_S;

public:
    CFormatTxtSiteCoord();
    //获取测点的坐标信息, 找到返回true,否则返回false
    bool GetSiteCoord(int line_no, //测点所在的线号
                             double x_c, //测点在测线上的相对位置
                             double dist_err, //允许的距离误差
                             SiteCoord_S *coord);

protected:
    /*从文件读出所有数据*/
    virtual bool ReadData(ifstream &stm);
    /*将所有数据写入文件*/
    virtual bool WriteData(ofstream &stm);

private:
    vector<SiteCoord_S> m_vt_coord;
};

#endif // FORMAT_TXT_SITE_COORD_H
