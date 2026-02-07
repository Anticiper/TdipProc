#ifndef FORMAT_TXT_COORD_H
#define FORMAT_TXT_COORD_H

#include "format_txt.h"

class CFormatTxtCoord : public CFormatTxt
{
public:
    typedef struct line_coord_s{
        string start_name; //文件名称(可能只包含文件名的前部分字符)
        float dist_ab; //测线0点与AB的垂距
        double x; //测线0点对应的X坐标
        double y; //测线0点对应的Y坐标
        double angle; //测线的方位角(北偏东的角度，单位为rad)
    }LineCoord_S;

public:
    CFormatTxtCoord();
    //获取测线0点坐标和方位角信息
    LineCoord_S GetLineCoord(string file_name, //供电数据的文件名
                             float dist2AB); //测线与AB之间的垂距(负表示小号测线方向)

protected:
    /*从文件读出所有数据*/
    virtual bool ReadData(ifstream &stm);
    /*将所有数据写入文件*/
    virtual bool WriteData(ofstream &stm);

private:
    bool IsStartWith(const string &str_all, const string &str_start);

private:
    vector<LineCoord_S> m_vt_coord;
};

#endif // FORMAT_TXT_COORD_H
