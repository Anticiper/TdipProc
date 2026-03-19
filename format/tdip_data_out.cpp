#include <assert.h>
#include <QObject>
#include "tdip_data_out.h"
#include "algo_pub.h"
#include "algo_geo.h"

CTdipDataOut::CTdipDataOut()
{
    m_first_out = true;
}

void CTdipDataOut::SetLimitInfo(vector<LimitItem_S> *vt_limit)
{
    m_vt_limit = vt_limit;
}

void CTdipDataOut::SetOutFile(const std::string &strOutFileName)
{
    m_base_file_name_out = strOutFileName;
}

void CTdipDataOut::SetInputFileBaseName(const std::string &strOutFileName)
{
    m_base_file_name_in = strOutFileName;
}

void CTdipDataOut::SetShiftingXY()
{
    m_shifting_info.clear();
    //每个srs文件内的x，y偏移量
    if(m_shifting->m_data.size()>0){
        for(int i=0;i<m_shifting->m_data.size();i++){
            ShiftingInfo_S shifting_info = m_shifting->m_data.at(i);
            if(m_base_file_name_in.find(shifting_info.srs_name) != std::string::npos){
                m_shifting_info.push_back(shifting_info);
            }
        }
    }
}

void CTdipDataOut::OutputErr(CFormatTRS res_arr[],
               vector<string>  *vt_files,
               int arr_num)
{
    string file_name;
    FILE *file = NULL;
    char buff[50];
    list<ChNode_S> list_chs;
    list<ChNode_S>::iterator iter;
    int max_acq_num = 0;

    assert(res_arr != NULL);
    assert(arr_num > 0);
    bool needCheckPosB = res_arr[0].GetPrmMeas()->layout_type != SIP_POLE3_SOUNDING;
    GetChMap(res_arr, arr_num, &list_chs, needCheckPosB);
    for(iter=list_chs.begin(); iter!=list_chs.end(); iter++){
        if(iter->vt_node.size() > max_acq_num){
            max_acq_num = iter->vt_node.size();
        }
    }
    if(m_first_out){
        m_first_out = false;
        sprintf(buff, "_Err.dat");
        file_name = m_base_file_name_out+string(buff);
        file = ex_fopen(file_name.data(), "wb");
        assert(file != NULL);
        m_vt_files.push_back(file);
        m_vt_file_names.push_back(file_name);
        m_vt_pos_file_data_cnt.push_back(-1);
        m_vt_data_cnt.push_back(0);
        PrintErrdHead(max_acq_num);
    }
    PrintErrBody(res_arr, vt_files, arr_num, &list_chs, max_acq_num);
}

void CTdipDataOut::PrintErrdHead(int max_acq_num)
{
    FILE *file = m_vt_files[0];
    int i = 0;
    char buff[50];
    int pole_num = GetPoleNum();

    assert(pole_num >= 3);
    fprintf(file, "%14s", "a_pos(m)");
    if(pole_num == 4){
        fprintf(file, "%14s", "b_pos(m)");
    }
    fprintf(file, "%14s", "m_pos(m)");
    fprintf(file, "%10s", "site");
    fprintf(file, "%14s", "Coord_X(m)");
    fprintf(file, "%14s", "Coord_Y(m)");
    for(i=0; i<max_acq_num; i++){
        sprintf(buff, "R%d(Ω·m)", i+1);
        fprintf(file, "%16s", buff);
    }
    fprintf(file, "%14s", "Err(%)");

    for(i=0; i<max_acq_num; i++){
        sprintf(buff, "ηs%d(%)", i+1);
        fprintf(file, "%14s", buff);
    }
    fprintf(file, "%14s", "Err(%)");

    for(i=0; i<max_acq_num; i++){
        sprintf(buff, "Dev%d", i+1);
        fprintf(file, "%9s", buff);
    }
    for(i=0; i<max_acq_num; i++){
        sprintf(buff, "File%d", i+1);
        fprintf(file, "%22s", buff);
    }
    fprintf(file, "\n");
}

void CTdipDataOut::PrintErrBody(CFormatTRS res_arr[], //多次供电的结果数据
                  vector<string>  *vt_files, //多次供电的res不带路径和后缀的文件名
                  int arr_num, //供电次数
                  list<ChNode_S> *list_chs, //按通道m位置排列的索引信息
                  int max_acq_num)
{
    FILE *file = m_vt_files[0];
    list<ChNode_S>::iterator iter;
    CResult::ChInfo_S info;
    int iAcq, idx_res, idx_ch_in_res;
    float avg, std, err;
    float err_dist,nextMx;
    vector<float> vt_data;
    char strNull[] = "*";
    string file_name;
    int pole_num = GetPoleNum();
    bool coord_found;
    CFormatTxtSiteCoord::SiteCoord_S coord_m, coord_n;

    assert(res_arr != NULL);
    assert(arr_num > 0);
    for(iter=list_chs->begin(); iter!=list_chs->end(); iter++){
        if(iter->vt_node.size() < 2) //只打印有2次采集及以上的数据
            continue;
        /*仅输出有效数据，利用第1个数据判断*/
        idx_res = iter->vt_node[0].res_idx;
        idx_ch_in_res = iter->vt_node[0].ch_idx_in_res;
        if(!IsChValid(idx_ch_in_res, res_arr[idx_res].GetResultCal(), &res_arr[0], true))
            continue;
        assert(pole_num >= 3);
        fprintf(file, "%14.2f", res_arr[0].GetResultCal()->GetPosAx());
        if(pole_num == 4){
            fprintf(file, "%14.2f", res_arr[0].GetResultCal()->GetPosBx());
        }
        info = res_arr[idx_res].GetResultCal()->GetChInfo(idx_ch_in_res);
        err_dist = info.e_len*0.05;
        coord_found = m_coord->GetSiteCoord(info.line_no, info.pos_m, err_dist, &coord_m);
        if(coord_found){
            nextMx = info.pos_m+info.e_len;
            coord_found = m_coord->GetSiteCoord(info.line_no, nextMx, err_dist, &coord_n);
        }
        fprintf(file, "%14.2f", iter->pos_m);
        if(coord_found){
            fprintf(file, "%10s", coord_m.site_name.data());
            fprintf(file, "%14.2lf", (coord_m.x+coord_n.x)/2);
            fprintf(file, "%14.2lf", (coord_m.y+coord_n.y)/2);
        }else{
            fprintf(file, "%10s", strNull);
            fprintf(file, "%14s", strNull);
            fprintf(file, "%14s", strNull);
        }
        /*打印电阻率及误差*/
        GetDataFromRes(res_arr, &iter->vt_node, 1, &vt_data);
        for(iAcq=0; iAcq<max_acq_num; iAcq++){
            if(iAcq < vt_data.size()){
                fprintf(file, "%14.2f", vt_data[iAcq]);
            }else{
                fprintf(file, "%14s", strNull);
            }
        }
        GetDataFromRes(res_arr, &iter->vt_node, 2, &vt_data);
        //std = algo_std_f(vt_data.data(), vt_data.size());
        avg = algo_avg_f(vt_data.data(), vt_data.size());
        //err = std/avg;
        fprintf(file, "%14.2f", avg);
        /*打印视极化率及误差*/
        GetDataFromRes(res_arr, &iter->vt_node, 3, &vt_data);
        for(iAcq=0; iAcq<max_acq_num; iAcq++){
            if(iAcq < vt_data.size()){
                fprintf(file, "%14.2f", vt_data[iAcq]);
            }else{
                fprintf(file, "%14s", strNull);
            }
        }
        GetDataFromRes(res_arr, &iter->vt_node, 4, &vt_data);
        //std = algo_std_f(vt_data.data(), vt_data.size());
        avg = algo_avg_f(vt_data.data(), vt_data.size());
        fprintf(file, "%14.2f", avg);

        /* 打印仪器_通道字符串*/
        for(iAcq=0; iAcq<max_acq_num; iAcq++){
            if(iAcq < iter->vt_node.size()){
                fprintf(file, "%9s", iter->vt_node[iAcq].dev_ch.data());
            }else{
                fprintf(file, "%9s", strNull);
            }
        }

        /* 打印文件名*/
        for(iAcq=0; iAcq<max_acq_num; iAcq++){
            if(iAcq < iter->vt_node.size()){
                idx_res = iter->vt_node[iAcq].res_idx;
                assert(idx_res>=0 && idx_res<vt_files->size());
                file_name = vt_files->at(idx_res);
                fprintf(file, "%22s", file_name.data());
            }else{
                fprintf(file, "%22s", strNull);
            }
        }

        fprintf(file, "\n");
    }
}

void CTdipDataOut::OutputMidGradient(CFormatTRS *res, string file_name_srs, CFormatTRS *trs)
{
    //第一次输出时打开文件并输出数据头信息
    string file_name;
    FILE *file = NULL;
    if(m_first_out){
        m_first_out = false;
        file_name = m_base_file_name_out+string("_plane.dat");
        file = ex_fopen(file_name.data(), "wb");
        assert(file != NULL);
        m_vt_files.push_back(file);
        m_vt_file_names.push_back(file_name);
        m_vt_pos_file_data_cnt.push_back(-1);
        m_vt_data_cnt.push_back(0);
        PrintPlaneHead(0);
    }
    PrintPlaneBody(0, res, file_name_srs, trs);
}

void CTdipDataOut::PrintPlaneHead(int idx_file)
{
    char buff[50];
    FILE *file = m_vt_files[idx_file];
    int m_harm_num = 1;
    fprintf(file, "%-20s", "file_name");
    fprintf(file, "%-10s", "site");
    fprintf(file, "%-18s", "M_N(m)");
    fprintf(file, "%14s", "Coord_X(m)");
    fprintf(file, "%14s", "Coord_Y(m)");
    for(int i=0; i<m_harm_num; i++){
        sprintf(buff, "R%d(Ω·m)", i+1);
        fprintf(file, "%16s", buff);
    }
    for(int i=0; i<m_harm_num; i++){
        sprintf(buff, "lg10(R%d)", i+1);
        fprintf(file, "%14s", buff);
    }
    for(int i=0; i<m_harm_num; i++){
        sprintf(buff, "Err%d(%)", i+1);
        fprintf(file, "%14s", buff);
    }
    for(int i=0; i<m_harm_num; i++){
        sprintf(buff, "ηs%d(%)", i+1);
        fprintf(file, "%14s", buff);
    }
    for(int i=0; i<m_harm_num; i++){
        sprintf(buff, "Err%d(%)", i+1);
        fprintf(file, "%14s", buff);
    }
    fprintf(file, "\n");
}

void CTdipDataOut::PrintPlaneBody(int idx_file, CFormatTRS *res, string file_name_srs, CFormatTRS *trs)
{
    char buff[50];
    float value;
//    double coord_x,coord_y;
    FILE *file = m_vt_files[idx_file];
    CFormatTxtSiteCoord::SiteCoord_S coord_m, coord_n;
    float err_dist,nextMx;
    bool coord_found;
    double pos_x, pos_y;
    CResult::ChInfo_S info, next_info;
    bool hasNext = false;
    int ch_num = res->GetResultCal()->GetChNum();
    int m_harm_num = 1;

    for(int iCh=0; iCh<ch_num; iCh++){
        info = res->GetResultCal()->GetChInfo(iCh);
        if((iCh+1) < ch_num){
            hasNext = true;
            next_info = res->GetResultCal()->GetChInfo(iCh+1);
        }
        if(!IsChValid(iCh, res->GetResultCal(), trs, true))
            continue;
        err_dist = info.e_len*0.05;
        coord_found = m_coord->GetSiteCoord(info.line_no, info.pos_m, err_dist, &coord_m);
        if(coord_found){
            if(hasNext){
                nextMx = next_info.pos_m;
                coord_found = m_coord->GetSiteCoord(info.line_no, nextMx, err_dist, &coord_n);
            }else{
                coord_found = false;
            }
        }
        fprintf(file, "%-20s", file_name_srs.data());
        if(coord_found){
            fprintf(file, "%-10s", coord_m.site_name.data());
            pos_x = (coord_m.x+coord_n.x)/2;
            pos_y = (coord_m.y+coord_n.y)/2;
        }else{
            fprintf(file, "%-10s", "*");
            pos_x = 0;
            pos_y = 0;
        }
        sprintf(buff, "%.1f_%.1f", info.pos_m, info.pos_m+info.e_len);
        fprintf(file, "%-18s", buff);
        fprintf(file, "%14.2lf", pos_x);
        fprintf(file, "%14.2lf", pos_y);
        for(int iF=0; iF<m_harm_num; iF++){
            value = res->GetResultCal()->GetResItem(iCh,0,0).appr_value;
            fprintf(file, "%14f", value);
        }
        for(int iF=0; iF<m_harm_num; iF++){
            value = res->GetResultCal()->GetResItem(iCh,0,0).appr_value;
            fprintf(file, "%14f", log10(value));
        }
        for(int iF=0; iF<m_harm_num; iF++){
            value = res->GetResultCal()->GetResItem(iCh,0,0).appr_err_value;
            fprintf(file, "%14f", value);
        }
        for(int iF=0; iF<m_harm_num; iF++){
            value = res->GetResultCal()->GetResItem(iCh,0,0).appc_value;
            fprintf(file, "%14f", value);
        }
        for(int iF=0; iF<m_harm_num; iF++){
            value = res->GetResultCal()->GetResItem(iCh,0,0).appc_err_value;
            fprintf(file, "%14f", value);
        }
        fprintf(file, "\n");
    }
}

void CTdipDataOut::OutputSect(CFormatTRS *res, CFormatTRS *trs)
{
    //第一次输出时打开文件并输出数据头信息
    string file_name;
    FILE *file = NULL;
    if(m_first_out){
        m_first_out = false;
        file_name = m_base_file_name_out+string("_sect.dat");
        file = ex_fopen(file_name.data(), "wb");
        assert(file != NULL);
        m_vt_files.push_back(file);
        m_vt_file_names.push_back(file_name);
        m_vt_pos_file_data_cnt.push_back(-1);
        m_vt_data_cnt.push_back(0);
        PrintSectionHead(0);
    }
    assert(m_vt_files.size() == 1);
    PrintSectionBody(0, res, trs);
}

void CTdipDataOut::PrintSectionHead(int idx_file)
{
    FILE *file = m_vt_files[idx_file];
    char buff[50];

    int m_harm_num = 1;
    fprintf(file, "%-14s", "MN/2(m)");
    if(m_inv_info.layout == SIP_POLE3_SOUNDING){
        fprintf(file, "%-14s", "AO/10(m)");
        fprintf(file, "%-14s", "eq_pos(m)");
        fprintf(file, "%-14s", "eq_depth(m)");
        fprintf(file, "%-8s", "P_Left");
    }else{// if(m_inv_info.layout == SIP_POLE4_SOUNDING){
        fprintf(file, "%-14s", "-AB/5(m)");
        fprintf(file, "%-14s", "-PC/2d5(m)"); //MN中心点离最近供电点的距离
    }
    fprintf(file, "%-14s", "VD2AB(m)");
    fprintf(file, "%-14s", "Alt(m)");
    for(int i=0; i<m_harm_num; i++){
        sprintf(buff, "R%d(Ω·m)", i+1);
        fprintf(file, "%-16s", buff);
    }
    for(int i=0; i<m_harm_num; i++){
        sprintf(buff, "lg10(R%d)", i+1);
        fprintf(file, "%-14s", buff);
    }
    for(int i=0; i<m_harm_num; i++){
        sprintf(buff, "Err%d(%)", i+1);
        fprintf(file, "%-14s", buff);
    }
    for(int i=0; i<m_harm_num; i++){
        sprintf(buff, "ηs%d(%)", i+1);
        fprintf(file, "%-14s", buff);
    }
    for(int i=0; i<m_harm_num; i++){
        sprintf(buff, "Err%d(%)", i+1);
        fprintf(file, "%-14s", buff);
    }
    fprintf(file, "\n");
}

void CTdipDataOut::PrintSectionBody(int idx_file, CFormatTRS *res, CFormatTRS *trs)
{
    FILE *file = m_vt_files[idx_file];
    float value;
    float ax,bx,ay,by,mx,my,nx,ny;
    float ab_2,mn_2,pc_2d5;
    float eq_pos, eq_depth;
    CResult::ChInfo_S info;
    int idx_dev,idx_dev_ch;
    TdipPrmDev_S *prm_dev;

    int m_harm_num = 1;
    ax = res->GetResultCal()->GetPosAx();
    bx = res->GetResultCal()->GetPosBx();
    ay = res->GetResultCal()->GetPosAy();
    by = res->GetResultCal()->GetPosBy();
    ab_2 = (float)(ax+bx)/2;
    for(int iCh=0; iCh<res->GetResultCal()->GetChNum(); iCh++){
        info = res->GetResultCal()->GetChInfo(iCh);
        idx_dev = info.idx_dev;
        idx_dev_ch = info.idx_dev_ch;
        prm_dev = res->GetPrmDev(idx_dev);
        //通道过滤
        if(!IsChValid(iCh, res->GetResultCal(), trs, true)){
            continue;
        }
        if(sipLayout == SIP_ALL_GRADIENT){
            mx = prm_dev->pos_m_x[idx_dev_ch];
            nx = prm_dev->pos_n_x[idx_dev_ch];
            my = prm_dev->pos_m_y[idx_dev_ch];
            ny = prm_dev->pos_n_y[idx_dev_ch];
        }else{
            mx = info.pos_m;
            my = ay + info.dist2AB;
            nx = mx + info.e_len;
            ny = ay + info.dist2AB;
        }
        mn_2 = (float)(mx+nx)/2;
        fprintf(file, "%-14f", mn_2);
        if(m_inv_info.layout == SIP_POLE3_SOUNDING){
            eq_pos = ax+(mn_2-ax)/2;
            eq_depth = -fabs(ax-mn_2)/3;
//            fprintf(file, "%-14f", ax/5);
            fprintf(file, "%-14f", ax/10);
            fprintf(file, "%-14f", eq_pos);
            fprintf(file, "%-14f", eq_depth);
            if(ax < mn_2){
                fprintf(file, "%-8d", 1);
            }else{
                fprintf(file, "%-8d", 0);
            }
        }else{
            fprintf(file, "%-14f", -fabs(ax-bx)/5);
            if(mn_2<ab_2){
                pc_2d5 = (ax-mx)/2.5;
            }else{
                pc_2d5 = (nx-bx)/2.5;
            }
            fprintf(file, "%-14f", pc_2d5);
        }
        fprintf(file, "%-14f", info.dist2AB);
        fprintf(file, "%-14f", m_elev->GetElev(mn_2, 0));
        for(int iF=0; iF<m_harm_num; iF++){
            value = res->GetResultCal()->GetResItem(iCh,0,0).appr_value;
            fprintf(file, "%-14f", value);
        }
        for(int iF=0; iF<m_harm_num; iF++){
            value = res->GetResultCal()->GetResItem(iCh,0,0).appr_value;
            fprintf(file, "%-14f", log10(value));
        }
        for(int iF=0; iF<m_harm_num; iF++){
            value = res->GetResultCal()->GetResItem(iCh,0,0).appr_err_value;
            fprintf(file, "%-14f", value);
        }
        for(int iF=0; iF<m_harm_num; iF++){
            value = res->GetResultCal()->GetResItem(iCh,0,0).appc_value;
            fprintf(file, "%-14f", value);
        }
        for(int iF=0; iF<m_harm_num; iF++){
            value = res->GetResultCal()->GetResItem(iCh,0,0).appc_err_value;
            fprintf(file, "%-14f", value);
        }
        fprintf(file, "\n");
    }
}

void CTdipDataOut::OutputRes2dinv(CFormatTRS *res, CFormatTRS *trs)
{
    string file_name;
    FILE *file = NULL;
    char buff[50];
    if(m_first_out){
        m_first_out = false;
        sprintf(buff, "_Res2dinv.dat");
        file_name = m_base_file_name_out+string(buff);
        file = ex_fopen(file_name.data(), "wb");
        assert(file != NULL);
        m_vt_files.push_back(file);
        m_vt_file_names.push_back(file_name);
        m_vt_pos_file_data_cnt.push_back(0);
        m_vt_data_cnt.push_back(0);
        PrintRes2dHead(0, res);
    }
    PrintRes2dBody(0, res, trs);
}

void CTdipDataOut::PrintRes2dHead(int idx_file, CFormatTRS *res)
{
    FILE *file = m_vt_files[idx_file];
    int pos_file_data_cnt = 0;

    pos_file_data_cnt += fprintf(file, "General array with IP\n");  //标题说明
    pos_file_data_cnt += fprintf(file, "%f\n", m_inv_info.inv_thickness); //反演层厚度
    pos_file_data_cnt += fprintf(file, "11\n"); //装置类型
    pos_file_data_cnt += fprintf(file, "0\n"); //装置类型
    pos_file_data_cnt += fprintf(file, "Type of measurement (0=app. resistivity,1=resistance)\n");
    pos_file_data_cnt += fprintf(file, "%d\n", !m_inv_info.resistivity); //0-视电阻率 1-视电阻
    m_vt_pos_file_data_cnt[idx_file] = pos_file_data_cnt;
    fprintf(file, "%-20d\n", 0);  //数据点数(预留20个字符的位置)
    fprintf(file, "1\n");//测量位置类型，0表示第一个电极(最小值)，1表示C1-P2中点
    fprintf(file, "%d\n", 1);//是否有激电数据：1-有，0-无
    //电压数据头
    fprintf(file, "Chargeability\n");
    fprintf(file, "mV/V\n");
    fprintf(file, "%g,%g\n",res->GetPrmMeas()->t2,
            res->GetPrmMeas()->l2);

    LimitItem_S appItem = m_vt_limit->at(LIMIT_TYPE_APP);
    LimitItem_S appcItem = m_vt_limit->at(LIMIT_TYPE_APPC);
    fprintf(file, "Error estimate for data present(Limit: AppErr=%.2f%, MErr=%.2f, App=%.2f~%.2f, M=%.2f~%.2f)\n",
            appItem.max_err_cp.value,
            appcItem.max_err_cp.value,
            appItem.min.value,appItem.max.value,
            appcItem.min.value,appcItem.max.value); //指示存在误差数据
    fprintf(file, "Type of error estimate (0=same unit as data)\n"); //误差类型的头
    fprintf(file, "0\n"); //0表示误差单位和测量值相同
}

void CTdipDataOut::PrintRes2dBody(int idx_file, CFormatTRS *res, CFormatTRS *trs)
{
    FILE *file = m_vt_files[idx_file];
    int pole_num = GetPoleNum();
    CResult::ChInfo_S info;
    float ph,ph_err,amp_avg,amp_std;
    float ax,bx,mx,nx,k;
    float ay,by,my,ny;
    float add_err_cp_mrad, add_err_app;
    float x_move;
    int idx_dev,idx_dev_ch;
    TdipPrmDev_S *prm_dev;

    ax = res->GetResultCal()->GetPosAx();
    bx = res->GetResultCal()->GetPosBx();
    ay = res->GetResultCal()->GetPosAy();
    by = res->GetResultCal()->GetPosBy();
    if(m_inv_info.add_system_err){
        add_err_cp_mrad = m_inv_info.system_err_cp;
        add_err_app = m_inv_info.system_err_app;
    }else{
        add_err_cp_mrad = 0;
        add_err_app = 0;
    }
    if(m_inv_info.is_xcoord_move){
        x_move = m_inv_info.xcoord_move_value;
    }else{
        x_move = 0;
    }
    for(int iCh=0; iCh<res->GetResultCal()->GetChNum(); iCh++){
        info = res->GetResultCal()->GetChInfo(iCh);
        //通道过滤
        if(!IsChValid(iCh, res->GetResultCal(), trs, true)){
            continue;
        }
        idx_dev = info.idx_dev;
        idx_dev_ch = info.idx_dev_ch;
        prm_dev = res->GetPrmDev(idx_dev);
        if(sipLayout == SIP_ALL_GRADIENT){
            mx = prm_dev->pos_m_x[idx_dev_ch];
            nx = prm_dev->pos_n_x[idx_dev_ch];
            my = prm_dev->pos_m_y[idx_dev_ch];
            ny = prm_dev->pos_n_y[idx_dev_ch];
        }else{
            mx = info.pos_m;
            my = ay + info.dist2AB;
            nx = mx + info.e_len;
            ny = ay + info.dist2AB;
        }

        fprintf(file, "%-3d", pole_num);
        fprintf(file, "%-14f", ax+x_move);
        fprintf(file, "%-14f", m_elev->GetElev(ax,0));
        if(pole_num == 4){
            fprintf(file, "%-14f", bx+x_move);
            fprintf(file, "%-14f", m_elev->GetElev(bx,0));
        }
        fprintf(file, "%-14f", mx+x_move);
        fprintf(file, "%-14f", m_elev->GetElev(mx,0));
        fprintf(file, "%-14f", nx+x_move);
        fprintf(file, "%-14f", m_elev->GetElev(nx,0));
        amp_avg = res->GetResultCal()->GetResItem(iCh,0,0).appr_value;
        amp_std = res->GetResultCal()->GetResItem(iCh,0,0).appr_err_value;
        amp_std = sqrt(pow(amp_avg*add_err_app, 2)+pow(amp_std,2)); //加系统误差
        if(m_inv_info.resistivity){
            fprintf(file, "%-14.4f", amp_avg);
            fprintf(file, "%-14.4f", amp_std);
        }else{  //视电阻需要用视电阻率转换回来
            if(pole_num == 3)
                //k = algo_cal_k_factor_pole3(ax, mx, nx, 0);
                k = algo_cal_k_factor_pole3(ax, ay, mx, my, nx, ny);
            else
                //k = algo_cal_k_factor(ax, bx, mx, nx, 0);
                k = algo_cal_k_factor(ax, ay, bx, by, mx, my, nx, ny);
            fprintf(file, "%-14.4E", amp_avg/k);
            fprintf(file, "%-14.4E", amp_std/k);
        }
        fprintf(file, "%-14.4f", res->GetResultCal()->GetResItem(iCh,0,0).appc_value);
        fprintf(file, "%-14.4f", res->GetResultCal()->GetResItem(iCh,0,0).appc_err_value);
        fprintf(file, "\n");
        m_vt_data_cnt[idx_file]++;
    }
}

void CTdipDataOut::OutputRes3dinv(CFormatTRS *trs)
{
    string file_name;
    FILE *file = NULL;
    char buff[50];
    if(m_first_out){
        m_first_out = false;
        sprintf(buff, "_Res3dinv.dat");
        file_name = m_base_file_name_out+string(buff);
        file = ex_fopen(file_name.data(), "wb");
        assert(file != NULL);
        m_vt_files.push_back(file);
        m_vt_file_names.push_back(file_name);
        m_vt_pos_file_data_cnt.push_back(0);
        m_vt_data_cnt.push_back(0);
        PrintRes3dHead(0, trs);
    }
    PrintRes3dBody(0, trs);
}

void CTdipDataOut::PrintRes3dHead(int idx_file, CFormatTRS *res)
{
    FILE *file = m_vt_files[idx_file];
    int pos_file_data_cnt = 0;

    pos_file_data_cnt += fprintf(file, "Mixed dipole-dipole and pole-dipole data\n");  //标题说明
    pos_file_data_cnt += fprintf(file, "%d\n",res->GetResultCal()->GetChNum()); //X轴方向有多少个电极
    pos_file_data_cnt += fprintf(file, "0\n");//Y轴方向有多少个电极
    int len = res->GetResultCal()->GetChInfo(0).e_len;
    pos_file_data_cnt += fprintf(file, "%d\n",len); //X轴方向的点距
    pos_file_data_cnt += fprintf(file, "0\n");//Y轴方向的点距
    pos_file_data_cnt += fprintf(file, "11\n"); //装置类型
    pos_file_data_cnt += fprintf(file, "0\n");
    pos_file_data_cnt += fprintf(file, "Type of data (0=apparent resistivity,1=resistance)\n"); //0-视电阻率 1-视电阻
    pos_file_data_cnt += fprintf(file, "%d\n", !m_inv_info.resistivity); //0-视电阻率 1-视电阻
    m_vt_pos_file_data_cnt[idx_file] = pos_file_data_cnt;
    fprintf(file, "%-20d\n", 0);  //数据点数(预留20个字符的位置)
    fprintf(file, "1\n");//测量位置类型，0表示第一个电极(最小值)，1表示C1-P2中点
    fprintf(file, "1\n");//是否有激电数据：1-有，0-无
    //电压数据头
    fprintf(file, "Chargeability\n");
    fprintf(file, "mV/V\n");
    fprintf(file, "%g,%g\n",res->GetPrmMeas()->t2,
            res->GetPrmMeas()->l2);

    LimitItem_S appItem = m_vt_limit->at(LIMIT_TYPE_APP);
    LimitItem_S appcItem = m_vt_limit->at(LIMIT_TYPE_APPC);

    fprintf(file, "Error estimate for data present(Limit: AppErr=%.2f%, MErr=%.2fmard, App=%.2f~%.2f, M=%.2f~%.2f)\n",
            appItem.max_err_cp.value,
            appcItem.max_err_cp.value,
            appItem.min.value,appItem.max.value,
            appcItem.min.value,appcItem.max.value); //指示存在误差数据
    fprintf(file, "Type of error estimate (0=same unit as data)\n"); //误差类型的头
    fprintf(file, "0\n"); //0表示误差单位和测量值相同
}

void CTdipDataOut::PrintRes3dBody(int idx_file, CFormatTRS *res)
{
    FILE *file = m_vt_files[idx_file];
    int pole_num = GetPoleNum();
    CResult::ChInfo_S info;
    float amp_avg,amp_std;
    float ax,ay,bx,by,mx,my,nx,ny,k;
    float add_err_cp_mrad, add_err_app;
    int idx_dev,idx_dev_ch;
    TdipPrmDev_S *prm_dev;
    float mxShifting,myShifting,nxShifting,nyShifting;
    float axShifting,ayShifting,bxShifting,byShifting;

    ax = res->GetResultCal()->GetPosAx();
    ay = res->GetResultCal()->GetPosAy();
    bx = res->GetResultCal()->GetPosBx();
    by = res->GetResultCal()->GetPosBy();
    if(m_inv_info.add_system_err){
        add_err_cp_mrad = m_inv_info.system_err_cp;
        add_err_app = m_inv_info.system_err_app;
    }else{
        add_err_cp_mrad = 0;
        add_err_app = 0;
    }
    for(int iCh=0; iCh<res->GetResultCal()->GetChNum(); iCh++){
        //通道过滤
        if(!IsChValid(iCh, res->GetResultCal(), res, true)){
            continue;
        }
        info = res->GetResultCal()->GetChInfo(iCh);
        GetShiftingValue(iCh, axShifting, ayShifting, bxShifting, byShifting,
                         mxShifting, myShifting, nxShifting, nyShifting);
        idx_dev = info.idx_dev;
        idx_dev_ch = info.idx_dev_ch;
        prm_dev = res->GetPrmDev(idx_dev);

        if(sipLayout == SIP_ALL_GRADIENT){
            mx = prm_dev->pos_m_x[idx_dev_ch];
            nx = prm_dev->pos_n_x[idx_dev_ch];
            my = prm_dev->pos_m_y[idx_dev_ch];
            ny = prm_dev->pos_n_y[idx_dev_ch];
        }else{
            mx = info.pos_m;
            my = ay + info.dist2AB;
            nx = mx + info.e_len;
            ny = ay + info.dist2AB;
        }
        fprintf(file, "%-3d", pole_num);
        fprintf(file, "%-14f", ax + axShifting);
        fprintf(file, "%-14f", ay + ayShifting);
        if(pole_num == 4){
            fprintf(file, "%-14f", bx + bxShifting);
            fprintf(file, "%-14f", by + byShifting);
        }
        fprintf(file, "%-14f", mx + mxShifting);
        fprintf(file, "%-14f", my + myShifting);
        fprintf(file, "%-14f", nx + nxShifting);
        fprintf(file, "%-14f", ny + nyShifting);

        amp_avg = res->GetResultCal()->GetResItem(iCh,0,0).appr_value;
        amp_std = res->GetResultCal()->GetResItem(iCh,0,0).appr_err_value;
        amp_std = sqrt(pow(amp_avg*add_err_app, 2)+pow(amp_std,2)); //加系统误差
        if(m_inv_info.resistivity){
            fprintf(file, "%-14.4f", amp_avg);
            fprintf(file, "%-14.4f", amp_std);
        }else{  //视电阻需要用视电阻率转换回来
            if(pole_num == 3)
                //k = algo_cal_k_factor_pole3(ax, mx, nx, 0);
                k = algo_cal_k_factor_pole3(ax, ay, mx, my, nx, ny);
            else
                //k = algo_cal_k_factor(ax, bx, mx, nx, 0);
                k = algo_cal_k_factor(ax, ay, bx, by, mx, my, nx, ny);
            fprintf(file, "%-14.4E", amp_avg/k);
            fprintf(file, "%-14.4E", amp_std/k);
        }
        fprintf(file, "%-14.4f", res->GetResultCal()->GetResItem(iCh,0,0).appc_value);
        fprintf(file, "%-14.4f", res->GetResultCal()->GetResItem(iCh,0,0).appc_err_value);

        fprintf(file, "\n");
        m_vt_data_cnt[idx_file]++;
    }
}

void CTdipDataOut::PrintRes3Topography()
{
    for(int i=0;i<m_vt_files.size();i++){
        FILE *file = m_vt_files[i];
        //高程坐标数据
        fprintf(file, "\n");
        fprintf(file, "Topography\n");
        fprintf(file, "1\n");//高程数据格式
        fprintf(file,"Topography in unstructured list\n");
        fprintf(file,"Number of topography data points\n");
        int size = m_elev->m_data.size();
        fprintf(file, "%d\n",size);//高程数据个数
        if(size>0){
            for(int i=0;i<size;i++){
                ElevInfo_S info = m_elev->m_data.at(i);
                fprintf(file, "%-6d",i+1);
                fprintf(file, "%-14f",info.x);
                fprintf(file, "%-14f",info.y);
                fprintf(file, "%-14f",info.elev);
                fprintf(file,"\n");
            }
        }
    }
}

void CTdipDataOut::ResetPassRateData()
{
    totalNum = 0;
    qualifiedNum = 0;
}

void CTdipDataOut::initMeasurePointInfo()
{
    m_measure_point.clear();
}

void CTdipDataOut::OutPassRateForPower(string file_name_srs,CFormatTRS *trs)
{
    string file_name;
    FILE *file = NULL;
    if(m_first_out){
        m_first_out = false;
        file_name = m_base_file_name_out+string("_passRate.txt");
        file = ex_fopen(file_name.data(), "wb");
        assert(file != NULL);
        m_vt_files.push_back(file);
        m_vt_file_names.push_back(file_name);
        m_vt_pos_file_data_cnt.push_back(-1);
        m_vt_data_cnt.push_back(0);

        fprintf(file, "%-30s", "file_name");
        fprintf(file, "%-14s", "qualified_num");
        fprintf(file, "%-14s", "total_num");
        fprintf(file, "%-14s", "pass_rate(%)");
        fprintf(file, "%-14s", "caled_num");
        fprintf(file, "%-14s", "acq_time");
        fprintf(file,"\n");
    }
    PrintPassRateForPower(0,file_name_srs,trs);
}

void CTdipDataOut::PrintPassRateForPower(int idx_file, string file_name_trs, CFormatTRS *trs)
{
    FILE *file = m_vt_files[idx_file];
    int num = trs->GetRes()->GetCaledNum();
    float sig_freq = trs->GetPrmMeas()->sig_freq;
    int nSec = num*(1/sig_freq)*2 + 35;
    string time = ex_time_sec_to_str(nSec);
    fprintf(file, "%-30s", file_name_trs.data());
    float pRate = GetPassRate(trs);
    fprintf(file, "%-14d", qualifiedNum);
    fprintf(file, "%-14d", totalNum);
    fprintf(file, "%-14.2f", pRate);
    fprintf(file, "%-14d", num);
    fprintf(file, "%-14s", time.data());
    fprintf(file,"\n");
}

void CTdipDataOut::OutPassRateForPoint()
{
    MeasurePointInfo_S info;
    FILE *file = m_vt_files[0];
    if(m_first_out){
        m_first_out = false;
        fprintf(file,"\n");
        fprintf(file, "%-14s", "pos_m(x)");
        if(sipLayout != SIP_POLE3_SOUNDING){
            fprintf(file, "%-14s", "pos_m(y)");
        }
        fprintf(file, "%-14s", "qualified_num");
        fprintf(file, "%-14s", "total_num");
        fprintf(file, "%-14s", "pass_rate(%)");
        fprintf(file,"\n");
    }
    for(int i=0;i<m_measure_point.size();i++){
        info = m_measure_point.at(i);
        fprintf(file, "%-14.2f", info.Mx);
        if(sipLayout != SIP_POLE3_SOUNDING){
            fprintf(file, "%-14.2f", info.My);
        }
        fprintf(file, "%-14d", info.qualified_num);
        fprintf(file, "%-14d", info.total_num);
        fprintf(file, "%-14.2f", info.pass_rate);
        fprintf(file,"\n");
    }
}

float CTdipDataOut::GetPassRate(CFormatTRS *trs)
{
    float pass_rate = 0;
    CTdipResultCal::ResItem_S res_item;
    CResult::ChInfo_S info;
    int i, ch_num, cnt_passed;
    CTdipResultCal *result_cal = trs->GetResultCal();
    ch_num = result_cal->GetChNum();
    cnt_passed = 0;
    int idx_dev,idx_dev_ch;
    TdipPrmDev_S *prm_dev;
    float mx,my;

    if(ch_num > 0){
        for(i=0; i<ch_num; i++){
            info = result_cal->GetChInfo(i);
            idx_dev = info.idx_dev;
            idx_dev_ch = info.idx_dev_ch;
            prm_dev = trs->GetPrmDev(idx_dev);
            res_item = result_cal->GetResItem(i, 0, 0);
            //通道过滤
            if(!IsChValid(i, result_cal, trs, false)){
                continue;
            }
            totalNum++;
            if(sipLayout == SIP_ALL_GRADIENT){
                mx = prm_dev->pos_m_x[idx_dev_ch];
                my = prm_dev->pos_m_y[idx_dev_ch];
            }else{
                mx = info.pos_m;
                my = 0;
            }
            //误差判断
            if(!IsWithinLimits(&res_item)){
                AddMeasurePoint(mx,my,0);
                continue;
            }
            cnt_passed++;
            AddMeasurePoint(mx,my,1);
        }
        qualifiedNum = cnt_passed;
        pass_rate = (float)cnt_passed/(float)totalNum;
    }
    return pass_rate*100;
}

void CTdipDataOut::AddMeasurePoint(float mx, float my, int qualified)
{
    MeasurePointInfo_S newPointInfo;
    MeasurePointInfo_S *oldPointInfo;
    float pass_rate;
    int i;
    bool isExist = false;

    newPointInfo.Mx = mx;
    newPointInfo.My = my;
    if(m_measure_point.size() < 1){//首次添加
        newPointInfo.qualified_num = qualified;
        newPointInfo.total_num = 1;
        pass_rate = (float)newPointInfo.qualified_num/(float)newPointInfo.total_num;
        newPointInfo.pass_rate = pass_rate*100;
        m_measure_point.push_back(newPointInfo);
    }else{//查找合并
        for(i=0;i<m_measure_point.size();i++){
            oldPointInfo = &m_measure_point.at(i);
            if(oldPointInfo->Mx == mx && oldPointInfo->My == my){
                isExist = true;
                oldPointInfo->qualified_num += qualified;
                oldPointInfo->total_num += 1;
                pass_rate = (float)oldPointInfo->qualified_num/(float)oldPointInfo->total_num;
                oldPointInfo->pass_rate = pass_rate*100;
                break;
            }
        }
        if(!isExist){
            newPointInfo.qualified_num = qualified;
            newPointInfo.total_num = 1;
            pass_rate = (float)newPointInfo.qualified_num/(float)newPointInfo.total_num;
            newPointInfo.pass_rate = pass_rate*100;
            m_measure_point.push_back(newPointInfo);
        }
    }
}

void CTdipDataOut::GetDataFromRes(CFormatTRS res_arr[],
                             const vector<ChPos_S> *vt_node,
                             int type,
                             vector<float> *vt_data)
{
    int iAcq;
    int idx_res, idx_ch;
    float value;

    vt_data->clear();
    for(iAcq=0; iAcq<vt_node->size(); iAcq++){
        idx_res = vt_node->at(iAcq).res_idx;
        idx_ch =  vt_node->at(iAcq).ch_idx_in_res;
//        assert(idx_res < arr_num);
        switch (type) {
        case 1:
            value = res_arr[idx_res].GetResultCal()->GetResItem(idx_ch, 0, 0).appr_value;
            break;
        case 2:
            value = res_arr[idx_res].GetResultCal()->GetResItem(idx_ch, 0, 0).appr_err_value;
            break;
        case 3:
            value = res_arr[idx_res].GetResultCal()->GetResItem(idx_ch, 0, 0).appc_value;
            break;
        case 4:
            value = res_arr[idx_res].GetResultCal()->GetResItem(idx_ch, 0, 0).appc_err_value;
            break;
        }

        vt_data->push_back(value);
    }
}

void CTdipDataOut::GetChMap(CFormatTRS res_arr[], int arr_num,
                            list<ChNode_S> *list_chs, bool needCheckPosB)
{
    int iArr, iCh;
    bool ch_prm_inited = false;
    float pos_m, dist2AB, e_len, Ax, Ay, Bx, By, freq;
    CTdipResultCal *res_tmp = NULL;
    ChNode_S ch_node;
    ChPos_S ch_pos;
    CResult::ChInfo_S ch_info;
    list<ChNode_S>::reverse_iterator iter;
    char strBuff[100];

    assert(arr_num > 0);
    list_chs->clear();
    Ax = res_arr[0].GetResultCal()->GetPosAx();
    Ay = res_arr[0].GetResultCal()->GetPosAy();
    Bx = res_arr[0].GetResultCal()->GetPosBx();
    By = res_arr[0].GetResultCal()->GetPosBy();
    //求取所有融合数据的m位置数组（按升序排列）
    for(iArr=0; iArr<arr_num; iArr++){
        res_tmp = res_arr[iArr].GetResultCal();
        assert(Ax == res_tmp->GetPosAx());
        assert(Ay == res_tmp->GetPosAy());
        if(needCheckPosB){
            assert(Bx == res_tmp->GetPosBx());
            assert(By == res_tmp->GetPosBy());
        }
        for(iCh=0; iCh<res_tmp->GetChNum(); iCh++){
            ch_info = res_tmp->GetChInfo(iCh);
            if(ch_prm_inited){
                assert(dist2AB == ch_info.dist2AB);
                assert(e_len == ch_info.e_len);
            }else{
                dist2AB = ch_info.dist2AB;
                e_len = ch_info.e_len;
            }
            sprintf(strBuff, "%s_%d", ch_info.dev_name.data(),
                    ch_info.idx_dev_ch+1);
            pos_m = ch_info.pos_m;
            ch_pos.res_idx = iArr;
            ch_pos.ch_idx_in_res = iCh;
            ch_pos.dev_ch = string(strBuff);
            ch_node.pos_m = pos_m;
            ch_node.vt_node.resize(1);
            ch_node.vt_node[0] = ch_pos;
            for(iter=list_chs->rbegin(); iter!=list_chs->rend(); iter++){ //倒序查找更快
                if(pos_m > iter->pos_m){  //重新插入新通道
                    list_chs->insert(iter.base(), ch_node);
                    break;
                }else if(pos_m == iter->pos_m){  //在老通道中追加信息
                    iter->vt_node.push_back(ch_pos);
                    break;
                }
            }//end iter
            if(iter == list_chs->rend()){//比所有值都小,在头部插入
                list_chs->push_front(ch_node);
            }
        }//end iCh
    }
}

bool CTdipDataOut::IsChValid(int idx_ch_res, CTdipResultCal *res, CFormatTRS *trs, bool is_check_limit)
{
    bool valid = true;
    float ax,bx,mx,nx;
    CResult::ChInfo_S info = res->GetChInfo(idx_ch_res);
    CTdipResultCal::ResItem_S item = res->GetResItem(idx_ch_res, 0, 0);
    //非电道
    if(info.ch_type != CH_TYPE_E)
        valid = false;
    //供电点在通道间
    ax = res->GetPosAx();
    bx = res->GetPosBx();
    mx = info.pos_m;
    nx = info.pos_m+info.e_len;
    if(ax>=mx && ax<=nx)
        valid = false;
    if(m_inv_info.layout != SIP_POLE3_SOUNDING){
        if(bx>=mx && bx<=nx)
            valid = false;
    }

    //检查范围和误差是否满足要求
    if(is_check_limit && !IsWithinLimits(&item)){
        valid = false;
    }

    //无计算次数
    if(trs->GetRes()->DoGetBuffCalNum() == 0)
        valid = false;

    return valid;
}

bool CTdipDataOut::IsWithinLimits(const CTdipResultCal::ResItem_S *item)
{
    assert(m_vt_limit != NULL);
    assert(m_vt_limit->size() == LIMIT_TYPE_CNT);
    //判断视电阻率及误差
    if(!IsWithinLimitOne(item->appr_value, item->appr_err_value, m_vt_limit->at(LIMIT_TYPE_APP))){
        return false;
    }
    //判断极化率及误差
    if(!IsWithinLimitOne(item->appc_value, item->appc_err_value, m_vt_limit->at(LIMIT_TYPE_APPC))){
        return false;
    }
    return true;
}

bool CTdipDataOut::IsWithinLimitOne(float value, float err, const LimitItem_S &limit)
{
    if(limit.min.valid){
        if(value< limit.min.value){
            return false;
        }
    }
    if(limit.max.valid){
        if(value > limit.max.value){
            return false;
        }
    }
    if(limit.max_err_cp.valid){
        if(err > limit.max_err_cp.value){
            return false;
        }
    }
    return true;
}

void CTdipDataOut::Open()
{
    m_first_out = true;
}

void CTdipDataOut::Close()
{
    //关闭所有文件
    for(int i=0; i<m_vt_files.size(); i++){
        ex_fclose(m_vt_files[i]);
    }
    m_vt_files.clear();
    //写入统计的数据点数
    for(int i=0; i<m_vt_file_names.size(); i++){
        if(m_vt_pos_file_data_cnt[i] >= 0){
            ReplaceDataCntInPrint(m_vt_file_names[i],
                                  m_vt_pos_file_data_cnt[i],
                                  m_vt_data_cnt[i]);
        }
    }
    m_vt_file_names.clear();
    m_vt_pos_file_data_cnt.clear();
    m_vt_data_cnt.clear();
}

void CTdipDataOut::ClearShifting()
{
    //清除相对坐标配置
    m_shifting->clear();
}

void CTdipDataOut::ReplaceDataCntInPrint(const string &file_name, int pos, int data_cnt)
{
    int ret = 0;
    FILE *file = ex_fopen(file_name.data(), "rb+");

    assert(file != NULL);
    if(pos < 0) //小于0不需要写入数据
        return;
    ret = ex_fseek_from_begin(file, pos);
    assert(ret == 0);
    ret = fprintf(file, "%d", data_cnt);
    assert(ret > 0);
    ex_fclose(file);
}

int CTdipDataOut::GetPoleNum()
{
    int pole_num = 0;

    if(m_inv_info.layout == SIP_POLE3_SOUNDING){
        pole_num = 3;
    }else{
        pole_num = 4;
    }
    return pole_num;
}

void CTdipDataOut::GetShiftingValue(int idx_ch, float &axShifting, float &ayShifting, float &bxShifting, float &byShifting,
                      float &mxShifting, float &myShifting, float &nxShifting, float &nyShifting)
{
    ShiftingInfo_S shifting_info;
    int ex_id;
    //重置当前通道所有点包括供电点的基础坐标偏移值
    axShifting = 0;
    ayShifting = 0;
    bxShifting = 0;
    byShifting = 0;
    mxShifting = 0;
    myShifting = 0;
    nxShifting = 0;
    nyShifting = 0;
    //查询当前相关坐标偏移值并赋值
    if(m_shifting_info.size()>0){
        for(int i=0;i<m_shifting_info.size();i++){
            shifting_info = m_shifting_info.at(i);
            ex_id = shifting_info.ex_id;
            //首先查找当前供电点是否需要偏移
            if(ex_id == 0){
                //电极号内部程序设置默认为0，表示有供电点偏移配置
//                    if(QObject::tr("%1").fromStdString(shifting_info.srs_name)
//                            == QObject::tr("%1").fromStdString(m_base_file_name_in)){
                if(QObject::tr("%1").fromStdString(m_base_file_name_in).contains( QObject::tr("%1").fromStdString(shifting_info.srs_name))){
                    //查找与当前供电点文件名一致的偏移配置
                    if(shifting_info.change_a){
                        axShifting = shifting_info.x;
                        ayShifting = shifting_info.y;
                    }
                    if(shifting_info.change_b){
                        bxShifting = shifting_info.x;
                        byShifting = shifting_info.y;
                    }
                }
            }else{//电极号配置非0，表示含有M,N电极偏移配置
                if(ex_id < 0){
                    //电极号配置小于0，表示当前所有电极都需要进行偏移
                    mxShifting += shifting_info.x;;
                    myShifting += shifting_info.y;
                    nxShifting += shifting_info.x;
                    nyShifting += shifting_info.y;
                }else{
                    //电极号大于0则表示需要根据设置的电极号进行偏移
                    if(ex_id == (idx_ch + 1)){//M极
                        mxShifting += shifting_info.x;
                        myShifting += shifting_info.y;
                    }
                    if(ex_id == (idx_ch + 2)){//N极
                        nxShifting += shifting_info.x;
                        nyShifting += shifting_info.y;
                    }
                }
            }
        }
    }
}
