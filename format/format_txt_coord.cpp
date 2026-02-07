#include <math.h>
#include <iomanip>
#include "format_txt_coord.h"
#include "algo_pub.h"

CFormatTxtCoord::CFormatTxtCoord()
{
}

CFormatTxtCoord::LineCoord_S CFormatTxtCoord::GetLineCoord(std::string file_name, float dist2AB)
{
    LineCoord_S coord = {string(""), 0, 0, 0, 0};
    LineCoord_S tmp;
    float diff_min = 0.01;
    for(int i=0; i<m_vt_coord.size(); i++){
        tmp = m_vt_coord.at(i);
        if(IsStartWith(file_name, tmp.start_name)
                && fabs(tmp.dist_ab-dist2AB)<=diff_min){
            coord = tmp;
            break;
        }
    }
    return coord;
}

bool CFormatTxtCoord::ReadData(ifstream &stm)
{
    vector<string> words;
    string strLine;
    LineCoord_S info;

    m_vt_coord.clear();
    getline(stm, strLine); //第1行为标题
    while(getline(stm, strLine)){
        SplitStr(strLine, string(" \t"), words);
        if(words.size() != 5)
            return false;
        info.start_name = words.at(0);
        info.dist_ab = atof(words.at(1).data());
        info.x = atof(words.at(2).data());
        info.y = atof(words.at(3).data());
        info.angle = algo_ph_deg2rad(atof(words.at(4).data()));
        m_vt_coord.push_back(info);
    }
    return true;
}

bool CFormatTxtCoord::WriteData(ofstream &stm)
{
    int width = 14;
    for(int i=0; i<m_vt_coord.size(); i++){
        LineCoord_S info = m_vt_coord.at(i);
        stm<<std::setw(20)<<info.start_name
          <<std::setw(width)<<info.dist_ab
         <<std::setw(width)<<info.x
        <<std::setw(width)<<info.y
        <<std::setw(width)<<info.angle
        <<std::endl;
    }
    return true;
}

bool CFormatTxtCoord::IsStartWith(const string &str_all, const string &str_start)
{
    bool ret = false;
    const char *str1,*str2;

    str1 = str_all.data();
    str2 = str_start.data();
    if(0 == str_all.find(str_start,0))
        ret = true;
    return ret;
}
