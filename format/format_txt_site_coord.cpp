#include <math.h>
#include <iomanip>
#include "format_txt_site_coord.h"

CFormatTxtSiteCoord::CFormatTxtSiteCoord()
{
}

bool CFormatTxtSiteCoord::GetSiteCoord(int line_no, //测点所在的线号
                                       double x_c, //测点在测线上的相对位置
                                       double dist_err, //允许的距离误差
                                       SiteCoord_S *coord)
{
    bool found = false;
    SiteCoord_S coord_found = {string(""), line_no, x_c, 0, 0};
    SiteCoord_S tmp;
    for(int i=0; i<m_vt_coord.size(); i++){
        tmp = m_vt_coord.at(i);
        if(tmp.line_no==line_no
                && fabs(tmp.x_c-x_c)<=dist_err){
            coord_found = tmp;
            found = true;
            break;
        }
    }
    *coord = coord_found;
    return found;
}

bool CFormatTxtSiteCoord::ReadData(std::ifstream &stm)
{
    vector<string> words;
    string strLine;
    SiteCoord_S info;

    m_vt_coord.clear();
    getline(stm, strLine); //第1行为标题
    while(getline(stm, strLine)){
        SplitStr(strLine, string(" \t"), words);
        if(words.size() != 5)
            return false;
        info.site_name = words.at(0);
        info.line_no = atoi(words.at(1).data());
        info.x_c = atof(words.at(2).data());
        info.x = atof(words.at(3).data());
        info.y = atof(words.at(4).data());
        m_vt_coord.push_back(info);
    }
    return true;
}

bool CFormatTxtSiteCoord::WriteData(std::ofstream &stm)
{
    int width = 16;
    for(int i=0; i<m_vt_coord.size(); i++){
        SiteCoord_S info = m_vt_coord.at(i);
        stm<<std::setw(20)<<info.site_name
          <<std::setw(width)<<info.line_no
             <<std::setw(width)<<info.x_c
         <<std::setw(width)<<info.x
        <<std::setw(width)<<info.y
        <<std::endl;
    }
    return true;
}
