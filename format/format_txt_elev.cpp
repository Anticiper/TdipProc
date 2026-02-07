#include <float.h>
#include <math.h>
#include <iomanip>
#include "format_txt_elev.h"
#include "format_txt.h"

CFormatTxtElev::CFormatTxtElev()
{
}

float CFormatTxtElev::GetElev(float pos_x, float pos_y)
{
    float elev_ret = 0;
    float diff = FLT_MAX;
    float tmp;

    for(int i=0; i<(int)m_data.size(); i++){
        ElevInfo_S info = m_data.at(i);
        tmp = fabs(pos_x-info.x)+fabs(pos_y-info.y);
        if(tmp < diff){
            diff = tmp;
            elev_ret = info.elev;
        }
    }
    return elev_ret;
}

bool CFormatTxtElev::ReadData(ifstream &stm)
{
    vector<string> words;
    string strLine;
    ElevInfo_S info;

    m_data.clear();
    while(getline(stm, strLine)){
        SplitStr(strLine, string(" \t"), words);
        if(words.size() != 3)
            return false;
        info.x = atof(words.at(0).data());
        info.y = atof(words.at(1).data());
        info.elev = atof(words.at(2).data());
        m_data.push_back(info);
    }
    return true;
}

bool CFormatTxtElev::WriteData(ofstream &stm)
{
    int width = 14;
    for(int i=0; i<(int)m_data.size(); i++){
        ElevInfo_S info = m_data.at(i);
        stm<<std::setw(width)<<info.x
          <<std::setw(width)<<info.y
         <<std::setw(width)<<info.elev
        <<std::endl;
    }
    return true;
}
