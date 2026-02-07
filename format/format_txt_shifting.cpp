#include <float.h>
#include <math.h>
#include <iomanip>
#include "format_txt_shifting.h"
#include "format_txt.h"
#include "clog.h"
#include <QObject>
#include <QString>

CFormatTxtShifting::CFormatTxtShifting()
{
}
//float CFormatTxtShifting::GetShifting(float pos_x, float pos_y)
//{
//    float ret = 0;
//    float diff = FLT_MAX;
//    float tmp;

//    for(int i=0; i<(int)m_data.size(); i++){
//        ShiftingInfo_S info = m_data.at(i);
//        tmp = fabs(pos_x-info.x)+fabs(pos_y-info.y);
//        if(tmp < diff){
//            diff = tmp;
//            ret = info.elev;
//        }
//    }
//    return ret;
//}

bool CFormatTxtShifting::ReadData(ifstream &stm)
{
    vector<string> words;
    string strLine;
    ShiftingInfo_S info;
    QString words_type;

    m_data.clear();
    getline(stm, strLine);
    while(getline(stm, strLine)){
        SplitStr(strLine, string(","), words);
        if(words.size() != 4)
            return false;
        string str = words.at(0);
        str.erase(std::remove_if(str.begin(), str.end(), [](char c) { return c == ' '; }), str.end());
        info.change_a = false;
        info.change_b = false;
        info.ex_id = 0;
        info.srs_name = str;
        info.x = QString(words.at(2).data()).trimmed().toFloat();
        info.y = QString(words.at(3).data()).trimmed().toFloat();
        words_type = QString(words.at(1).data()).trimmed();
        if(words_type == QObject::tr("A")){
            info.change_a = true;
        }else if(words_type == QObject::tr("B")){
            info.change_b = true;
        }else{
            info.ex_id = words_type.toInt();
        }
        m_data.push_back(info);
    }
    return true;
}

bool CFormatTxtShifting::WriteData(ofstream &stm)
{
    int width = 20;
    for(int i=0; i<(int)m_data.size(); i++){
        ShiftingInfo_S info = m_data.at(i);
        stm<<std::setw(width)<<info.srs_name
          <<std::setw(width)<<info.x
         <<std::setw(width)<<info.y
        <<std::endl;
    }
    return true;
}

void CFormatTxtShifting::clear()
{
    if(m_data.size() > 0)
        m_data.clear();
}
