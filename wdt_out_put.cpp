#include <assert.h>
#include <QMessageBox>
#include <QFileDialog>
#include "wdt_out_put.h"
#include "ui_wdt_out_put.h"
#include "ex_code.h"
#include "model_range_limit.h"
#include "ui_cursor.h"
#include "shared_data.h"

typedef struct acq_layout_s{ //单次供电采集装置信息
    float pos_a; //供电点A的坐标
    float pos_b; //供电点B的坐标
    float pos_m_seq0; //排列号为0的测点对应的m
    float distAb_res0; //结果文件第0索引通道与AB的距离
}AcqLayout_S;

//查找数组中相同采集装置的索引号，如果未查找到，返回-1
static int GetIndxOfAcq(const QVector<AcqLayout_S> *vt_acqs, const AcqLayout_S *acq, bool needCheckPosB)
{
    int idx_ret = -1;

    for(int i=0; i<vt_acqs->size(); i++){
        if(!EQ_FLOAT(vt_acqs->at(i).pos_a, acq->pos_a))
            continue;
        if(needCheckPosB && !EQ_FLOAT(vt_acqs->at(i).pos_b, acq->pos_b))
            continue;
        if(!EQ_FLOAT(vt_acqs->at(i).pos_m_seq0, acq->pos_m_seq0))
            continue;
        if(!EQ_FLOAT(vt_acqs->at(i).distAb_res0, acq->distAb_res0))
            continue;
        idx_ret = i;
        break;
    }
    return idx_ret;
}

CWdtOutPut::CWdtOutPut(QStringList *file_names_trs, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CWdtOutPut)
{
    ui->setupUi(this);
    m_file_names_trs = file_names_trs;
    InitLimitData();
    InitLimitTable();
    /**初始化按钮数组**/
    //导出数据类型-检查点,中梯扫面,二维,三维
    m_grpOutDataType = new QButtonGroup(this);
    m_grpOutDataType->addButton(ui->rdCheckErr_2);
    m_grpOutDataType->addButton(ui->rdMidGradient_2);
    m_grpOutDataType->addButton(ui->rd2DSounding_2);
    m_grpOutDataType->addButton(ui->rd3DSounding_2);
    //拟断面，反演
    m_grpSectOrInv = new QButtonGroup(this);
    m_grpSectOrInv->addButton(ui->rdOutSection_2);
    m_grpSectOrInv->addButton(ui->rdInvData_2);
    //极距组合
    m_grpPoleCmp = new QButtonGroup(this);
    m_grpPoleCmp->addButton(ui->rdPoleCmpYes_2);
    m_grpPoleCmp->addButton(ui->rdPoleCmpNo_2);
    //视极化率
    m_grpTdipPrm = new QButtonGroup(this);
    m_grpTdipPrm->addButton(ui->rdOutTdipAppc);
    //视电阻，视电阻率
    m_grpInvApp = new QButtonGroup(this);
    m_grpInvApp->addButton(ui->rdInvAppResistance_2);
    m_grpInvApp->addButton(ui->rdInvAppResistivity_2);
    //装置类型三极，四极
    m_grpType = new QButtonGroup(this);
    m_grpType->addButton(ui->rdType3_2);
    m_grpType->addButton(ui->rdType4_2);

    ui->rd2DSounding_2->setChecked(true);
    ui->rdOutSection_2->setChecked(true);
    ui->rdOutTdipAppc->setChecked(true);
    ui->rdPoleCmpNo_2->setChecked(true);
    ui->rdInvAppResistivity_2->setChecked(true);
    ui->chkAddSystemErr_2->setChecked(true);
    ui->chkXCoordMove_2->setChecked(false);
    ui->rdType4_2->setChecked(true);

    UpdateData(false);
    onUpdateUiShow();

    connect(m_grpOutDataType, SIGNAL(buttonClicked(int)), this, SLOT(onUpdateUiShow()));
    connect(m_grpSectOrInv, SIGNAL(buttonClicked(int)), this, SLOT(onUpdateUiShow()));
    connect(ui->chkAddSystemErr_2, SIGNAL(clicked()), this, SLOT(onUpdateUiShow()));
    connect(ui->chkXCoordMove_2, SIGNAL(clicked()), this, SLOT(onUpdateUiShow()));
    connect(ui->rdPoleCmpNo_2, SIGNAL(clicked()), this, SLOT(onUpdateUiShow()));
    connect(ui->rdPoleCmpYes_2, SIGNAL(clicked()), this, SLOT(onUpdateUiShow()));
    connect(ui->edtMaxAppErr,SIGNAL(textChanged(QString)),this, SLOT(onUpdateMaxErrValue()));
    connect(ui->edtMaxAppcErr,SIGNAL(textChanged(QString)),this, SLOT(onUpdateMaxErrValue()));
}

CWdtOutPut::~CWdtOutPut()
{
    delete ui;
}

/**
 * @brief 导出数据
 */
void CWdtOutPut::on_btnOutData_2_clicked()
{
    onDataOut();
}

/**
 * @brief 导出合格率
 */
void CWdtOutPut::on_btnOutPassRate_2_clicked()
{
    onPassRateOut();
}

/**
 * @brief 设置导出参数
 */
void CWdtOutPut::ResetPoleCmp()
{
    m_first_layout = GetTrsLayout();
    bool isAllGradient = m_first_layout==SIP_ALL_GRADIENT;
    ui->rdPoleCmpNo_2->setChecked(isAllGradient);
    //ui->rdPoleCmpYes_2->setEnabled(!isAllGradient);
    ui->rdPoleCmpNo_2->setEnabled(!isAllGradient);
    ui->lbCmpShiftPer_2->setEnabled(!isAllGradient);
    ui->edtShiftPercent_2->setEnabled(!isAllGradient);
}

/**
 * @brief 清除数据
 */
void CWdtOutPut::ClearData()
{
    ui->edtInputFileShifting_2->setText("");
    m_shifting.clear();
}

/**
 * @brief 初始化默认误差设置
 */
void CWdtOutPut::InitLimitData()
{
    //初始化限制参数
    for(int i=0; i<LIMIT_TYPE_CNT; i++){
        m_vt_limit.push_back(DataLimitGetInit());
    }
    DataLimitSetItem(&m_vt_limit[LIMIT_TYPE_APP].min, 0);
    DataLimitSetItem(&m_vt_limit[LIMIT_TYPE_APP].max, 1E6);
    DataLimitSetItem(&m_vt_limit[LIMIT_TYPE_APP].max_err_cp, 1);
    DataLimitSetItem(&m_vt_limit[LIMIT_TYPE_APPC].min, 0);
    DataLimitSetItem(&m_vt_limit[LIMIT_TYPE_APPC].max, 0.5);
    DataLimitSetItem(&m_vt_limit[LIMIT_TYPE_APPC].max_err_cp, 0.5);
    SharedData::instance()->SetLimitInfo(&m_vt_limit);
}

/**
 * @brief 初始化误差表格
 */
void CWdtOutPut::InitLimitTable()
{
    CModelRangeLimit *model = NULL;

    //设置数据表模型及控件
    model = new CModelRangeLimit(&m_vt_limit, this);
    ui->tbLimitRange->setModel(model);
    ui->tbLimitRange->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tbLimitRange->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void CWdtOutPut::onUpdateMaxErrValue(){
    m_vt_limit.at(LIMIT_TYPE_APP).max_err_cp.value = ui->edtMaxAppErr->text().toFloat();
    m_vt_limit.at(LIMIT_TYPE_APPC).max_err_cp.value = ui->edtMaxAppcErr->text().toFloat();
    SharedData::instance()->SetLimitInfo(&m_vt_limit);
}

/**
 * @brief 更新UI
 */
void CWdtOutPut::onUpdateUiShow()
{
    bool is_plane;//是否为扫面数据
    bool is_sect; //是否为二维测深数据
    bool is_inv; //是否为反演数据
    bool is_add_system_err; //是否加入系统误差
    bool is_chk_err; //是否为检查点误差
    bool is_xcoord_move; //x坐标是否移动
    bool is_cmp_pole; //极距是否组合
    bool is_sect3;//是否为三维测深数据

    is_plane = ui->rdMidGradient_2->isChecked();
    is_chk_err = ui->rdCheckErr_2->isChecked();
    is_sect = ui->rd2DSounding_2->isChecked();
    is_sect3 = ui->rd3DSounding_2->isChecked();
    is_inv = !ui->rdOutSection_2->isChecked();
    is_add_system_err = ui->chkAddSystemErr_2->isChecked();
    is_xcoord_move = ui->chkXCoordMove_2->isChecked();
    is_cmp_pole = ui->rdPoleCmpYes_2->isChecked();

    /*一维扫面UI*/
    ui->btnImportFileCoord_2->setVisible(is_plane || is_chk_err);
    ui->edtInputFileCoord_2->setVisible(is_plane || is_chk_err);
    /*二维测深UI*/
    ui->rdOutSection_2->setVisible(is_sect || is_sect3);
    ui->rdInvData_2->setVisible(is_sect || is_sect3);

    ui->rdOutTdipAppc->setVisible( (is_sect || is_sect3) && is_inv);
    ui->rdInvAppResistance_2->setVisible( (is_sect || is_sect3) && is_inv);
    ui->rdInvAppResistivity_2->setVisible( (is_sect || is_sect3) && is_inv);

    ui->rdPoleCmpYes_2->setVisible(is_sect && is_inv);
    ui->rdPoleCmpNo_2->setVisible(is_sect && is_inv);
    ui->edtShiftPercent_2->setVisible(is_sect && is_inv);
    ui->lbCmpShiftPer_2->setVisible(is_sect && is_inv);


    ui->lbMinPoleLen_2->setVisible( (is_sect || is_sect3) && is_inv);
    ui->edtMinDist_2->setVisible( (is_sect || is_sect3) && is_inv);
    ui->chkAddSystemErr_2->setVisible( (is_sect || is_sect3) && is_inv);
    ui->edtSystemErrAppPer_2->setVisible( (is_sect || is_sect3) && is_inv);
    ui->edtSystemErrCp_2->setVisible( (is_sect || is_sect3) && is_inv);
    ui->lbSystemErrApp_2->setVisible( (is_sect || is_sect3) && is_inv);
    ui->lbSystemErrCp_2->setVisible( (is_sect || is_sect3) && is_inv);
    ui->chkAddSystemErr_2->setVisible( (is_sect || is_sect3) && is_inv);

    ui->chkXCoordMove_2->setVisible( is_sect && is_inv);
    ui->edtXCoordMoveValue_2->setVisible( is_sect && is_inv);
    ui->btnImportFileElev_2->setVisible( (is_sect || is_sect3) && is_inv);
    ui->edtInputFileElev_2->setVisible( (is_sect || is_sect3) && is_inv);
    ui->lb2DSouding3_2->setVisible((is_sect || is_sect3));
    ui->lbInvH1_2->setVisible( (is_sect || is_sect3) && is_inv);
    ui->lbInvH2_2->setVisible( (is_sect || is_sect3) && is_inv);

    ui->lbInvV2_2->setVisible( (is_sect || is_sect3) && is_inv);
    ui->lbInvV3_2->setVisible( (is_sect || is_sect3) && is_inv);
    ui->btnImportFileShifting_2->setVisible(is_sect3);
    ui->edtInputFileShifting_2->setVisible(is_sect3);
    ui->rdType3_2->setVisible(is_sect3 && !is_inv);
    ui->rdType4_2->setVisible(is_sect3 && !is_inv);

    ui->edtSystemErrAppPer_2->setEnabled(is_add_system_err);
    ui->edtSystemErrCp_2->setEnabled(is_add_system_err);
    ui->edtXCoordMoveValue_2->setEnabled(is_xcoord_move);
    ui->edtShiftPercent_2->setEnabled(is_cmp_pole);
}

/**
 * @brief 获取当前选择TRS装置方法
 * @return
 */
SipLayout_T CWdtOutPut::GetTrsLayout() const
{
    SipLayout_T layout_type = SIP_POLE4_SOUNDING;
    CFormatTRS trs;

    if(m_file_names_trs->size() == 0)
        return layout_type;
    if(!trs.ReadFile(ex_u2g(m_file_names_trs->at(0).toStdString()).data()))
        assert(0);
    layout_type = trs.GetPrmMeas()->layout_type;
    return layout_type;
}

void CWdtOutPut::onDataOut()
{
    CTdipDataOut DataOut;
    CTdipDataOut::InvInfo_S inv_info;
    QString strOutFile;
    QVector<QStringList> vt_file_names;
    float cmp_shift_ratio; //组合移动距离与极距的比值
    QStringList *file_names;
    vector<string>  vt_files; //不带路径名和后缀的文件名
    QString base_name;
    CFormatTRS *format_trs_arr = NULL;
//    CTdipResultCal *res_arr = NULL;
    CFormatTRS res;
    int power_num = 0;
    int max_ch_num_cps, ch_num_cps, ch_num_res, iPower, cal_num;
    string strTRS, strTxt;
    QString strTmp;
    bool need_compose_ch = IsNeedCompseCh();
    int fileCheckResut;
    bool isOutput3dinv = false;

    //导入文件校验
    fileCheckResut = ClassifyFileName(m_file_names_trs, &vt_file_names);
    if(fileCheckResut > 0){
        QMessageBox::warning(this,tr("文件警告"),tr("第%1个SRS数据文件装置错误!").arg(fileCheckResut));
        return;
    }
    if(vt_file_names.size() <=0){
        QMessageBox::warning(this,tr("文件警告"),tr("没有找到符合要求的SRS数据文件!"));
        return;
    }
    UpdateData(true);
    if(m_strPathOut.isEmpty()){
        m_strPathOut = GetFirstTrsDir();
    }
    m_strPathOut = QFileDialog::getExistingDirectory(this, tr("保存目录选择"), m_strPathOut);
    if(m_strPathOut.isEmpty())
        return;
    CUiCursor::Wait();
    strOutFile = m_strPathOut+"/";
    strOutFile += ui->edtOutFileName_2->text();

    inv_info.inv_thickness = ui->edtMinDist_2->text().toFloat();
    inv_info.resistivity = ui->rdInvAppResistivity_2->isChecked();
    inv_info.layout = GetSipLayout();
    inv_info.add_system_err = ui->chkAddSystemErr_2->isChecked();
    inv_info.system_err_app = ui->edtSystemErrAppPer_2->text().toFloat()/100;
    inv_info.system_err_cp = ui->edtSystemErrCp_2->text().toFloat()/1000;
    inv_info.is_xcoord_move = ui->chkXCoordMove_2->isChecked();
    if(ui->rd3DSounding_2->isChecked()){
        inv_info.xcoord_move_value = 0;
    }else{
        inv_info.xcoord_move_value = ui->edtXCoordMoveValue_2->text().toFloat();
    }
    cmp_shift_ratio = 0;
    DataOut.SetCoord(&m_site_coord);
    DataOut.SetElevation(&m_elev);
    DataOut.SetLimitInfo(&m_vt_limit);
    DataOut.SetShiftingInfo(&m_shifting);
    DataOut.SetOutFile(ex_u2g(strOutFile.toStdString()));
    DataOut.SetInvInfo(inv_info);
    DataOut.SetType(inv_info.layout == SIP_POLE3_SOUNDING);
    DataOut.SetSipLayout(m_first_layout);
    DataOut.Open();
    for(int i=0; i<vt_file_names.size(); i++){//不同供电点遍历
        file_names = &vt_file_names[i];
        base_name = GetBaseNameFromFullFileName(file_names->at(0));
        DataOut.SetInputFileBaseName(base_name.toStdString());
        DataOut.SetShiftingXY();//三维数据导出设置当前供电点的XY相对坐标
        //按单供电点的供电次数申请缓冲区
        if(power_num != file_names->size()){
            if(format_trs_arr!=NULL)delete[] format_trs_arr;
            power_num = file_names->size();
            format_trs_arr = new CFormatTRS[power_num];
//            res_arr = new CTdipResultCal[power_num];
        }
        //读取单供电点所有供电次数的采集数据
        max_ch_num_cps = 1;
        vt_files.clear();
        for(iPower=0; iPower<power_num; iPower++){
            strTmp = file_names->at(iPower);
            strTRS = ex_u2g(strTmp.toStdString());
            strTmp = GetBaseNameFromFullFileName(strTmp);
            vt_files.push_back(ex_u2g(strTmp.toStdString()));
            if(!format_trs_arr[iPower].ReadFile(strTRS.data())){
                DataOut.Close();
                CUiCursor::Restore();
                QMessageBox::warning(this, tr("提示"), tr("读取文件%1失败!").arg(m_file_names_trs->at(i)));
                goto end;
            }
//            &res_arr[iPower] = format_trs_arr[iPower].GetResultCal();
            if(need_compose_ch){
                cmp_shift_ratio = ui->edtShiftPercent_2->text().toFloat()/100.0;
                ch_num_res = format_trs_arr[iPower].GetResultCal()->GetChNum();
                if(ch_num_res > max_ch_num_cps)
                    max_ch_num_cps = ch_num_res;
            }
        }
        //计算各种通道组合的数据并导出到文件中(首版暂不支持极距组合)
        for(ch_num_cps=1; ch_num_cps<=max_ch_num_cps; ch_num_cps++){ //多通道组合遍历
            //将数据输出导文件中
            if(ui->rdCheckErr_2->isChecked()){
                DataOut.OutputErr(format_trs_arr, &vt_files, power_num);
            }else{
                bool needCheckPosB = format_trs_arr[0].GetPrmMeas()->layout_type != SIP_POLE3_SOUNDING;
                MergeRes(power_num, format_trs_arr, &res, needCheckPosB); //多次供电数据及检查点合并(选择误差小的)
                if(ui->rdMidGradient_2->isChecked()){
                    DataOut.OutputMidGradient(&res, vt_files[0], &format_trs_arr[0]);
                }else if(ui->rd2DSounding_2->isChecked()){
                    if(ui->rdOutSection_2->isChecked()){
                        DataOut.OutputSect(&res, &format_trs_arr[0]);
                    }else{
                        DataOut.OutputRes2dinv(&res, &format_trs_arr[0]);
                    };
                }else if(ui->rd3DSounding_2->isChecked()){
                    if(ui->rdOutSection_2->isChecked()){
                        DataOut.OutputSect(&format_trs_arr[0], &format_trs_arr[0]);
                    }else if(ui->rdInvData_2->isChecked()){
                        isOutput3dinv = true;
                        DataOut.OutputRes3dinv(&format_trs_arr[0]);
                    }
                }
            }
        }//end ch_num_cps
    }//end vt_file_names
    if(isOutput3dinv){//三维反演数据导出末尾高程数据
        DataOut.PrintRes3Topography();
    }
    DataOut.Close();
    CUiCursor::Restore();
    QMessageBox::information(this, tr("提示"), tr("数据转换完成"));
end:
    if(format_trs_arr!=NULL)delete[] format_trs_arr;
//    if(res_arr!=NULL)delete[] res_arr;
}

void CWdtOutPut::onPassRateOut()
{
    CTdipDataOut DataOut;
    QString strOutFile;
    string strTRS;
    QString file_name;
    CFormatTRS format_trs;
    CTdipDataOut::InvInfo_S inv_info;
    int count=0;//成功文件数
    int errorId = -1;//错误的文件ID
    //导出文件判断
    if(m_file_names_trs->size() < 1){
        QMessageBox::warning(this,tr("文件警告"),tr("没有找到符合要求的TRS数据文件!"));
        return;
    }
    strOutFile = ui->edtOutFileName_2->text();
    UpdateData(true);
    if(strOutFile.isEmpty()){
        QMessageBox::warning(this,tr("文件警告"),tr("导出的文件名为空!"));
        return;
    }
    if(m_strPassRatePathOut.isEmpty()){
        m_strPassRatePathOut = GetFirstTrsDir();
    }
    m_strPassRatePathOut = QFileDialog::getExistingDirectory(this, tr("保存目录选择"), m_strPassRatePathOut);
    if(m_strPassRatePathOut.isEmpty())
        return;
    CUiCursor::Wait();
    strOutFile = m_strPassRatePathOut+"/";
    strOutFile += ui->edtOutFileName_2->text();

    inv_info.inv_thickness = ui->edtMinDist_2->text().toFloat();
    inv_info.resistivity = 1;
    inv_info.layout = m_first_layout;
    inv_info.system_err_app = ui->edtSystemErrAppPer_2->text().toFloat();
    inv_info.system_err_cp = ui->edtSystemErrCp_2->text().toFloat();
    DataOut.SetOutFile(ex_u2g(strOutFile.toStdString()));
    DataOut.SetElevation(&m_elev);
    DataOut.SetSipLayout(m_first_layout);
    DataOut.SetInvInfo(inv_info);
    DataOut.SetLimitInfo(&m_vt_limit);
    DataOut.initMeasurePointInfo();
    DataOut.Open();
    //循环读取导入的所有供电点文件
    for(int i=0; i<m_file_names_trs->size(); i++){
        DataOut.ResetPassRateData();
        file_name = m_file_names_trs->at(i);
        strTRS = ex_u2g(file_name.toStdString());
        if(!format_trs.ReadFile(strTRS.data())){
            continue;
        } //读取数据失败
        if(format_trs.GetResultCal()->GetChNum()<=0) //无有效通道
            continue;
        if(format_trs.GetPrmMeas()->layout_type != m_first_layout){//文件装置异常
            errorId = i+1;
            break;
        }
        count++;
        //写入当前供电点的合格率
        file_name = GetFileNameFromFullFileName(file_name);
        DataOut.OutPassRateForPower(ex_u2g(file_name.toStdString()),&format_trs);
    }
    DataOut.Open();
    DataOut.OutPassRateForPoint();
    DataOut.Close();
    CUiCursor::Restore();
    if(errorId != -1){
        QMessageBox::warning(this,tr("文件警告"),tr("第%1个TRS数据文件装置错误!").arg(errorId));
        return;
    }
    if(count != 0){
       QMessageBox::information(this, tr("提示"), tr("数据导出完成"));
    }else{
       QMessageBox::information(this,tr("提示"),tr("导出完成，无有效数据!"));
    }
}

void CWdtOutPut::UpdateData(bool bSaveOrShow)
{
    float *app_max_err = &m_vt_limit[LIMIT_TYPE_APP].max_err_cp.value;
    float *appc_max_err = &m_vt_limit[LIMIT_TYPE_APPC].max_err_cp.value;
    if(bSaveOrShow){
        *app_max_err = ui->edtMaxAppErr->text().toFloat();
        *appc_max_err = ui->edtMaxAppcErr->text().toFloat();
    }else{
        ui->edtMaxAppErr->setText(tr("%1").arg((*app_max_err)));
        ui->edtMaxAppcErr->setText(tr("%1").arg((*appc_max_err)));
    }
}

bool CWdtOutPut::IsNeedCompseCh()
{
    bool need_compse_ch = false;

    if(ui->rd2DSounding_2->isChecked()){ //二维测深装置
        if(ui->rdInvData_2->isChecked()){ //Res2dinv反演
            if(ui->rdPoleCmpYes_2->isChecked()){
                need_compse_ch = true;
            }
        }
    }
    return need_compse_ch;
}

int CWdtOutPut::ClassifyFileName(const QStringList *list_files,
                                  QVector<QStringList> *vt_files)
{
    QVector<AcqLayout_S> vt_acq_layout;
    AcqLayout_S acq_layout;
    QString file_name;
    string strTRS;
    CFormatTRS format_trs;
    CResult::ChInfo_S ch_info;
    int idx;
    int result = 0;

    vt_files->clear();
    for(int i=0; i<list_files->size(); i++){
        file_name = list_files->at(i);
        strTRS = ex_u2g(file_name.toStdString());
        if(!format_trs.ReadFile(strTRS.data())){
            continue;
        } //读取数据失败

        if(format_trs.GetResultCal()->GetChNum() <= 0) //无有效通道
            continue;

        if(format_trs.GetPrmMeas()->layout_type != m_first_layout){//文件装置异常
            result = i+1;
            vt_files->clear();
            break;
        }
        acq_layout.pos_a = format_trs.GetPrmMeas()->Ax;
        acq_layout.pos_b = format_trs.GetPrmMeas()->Bx;
        ch_info = format_trs.GetResultCal()->GetChInfo(0);
        acq_layout.distAb_res0 = ch_info.dist2AB;
        acq_layout.pos_m_seq0 = ch_info.pos_m-ch_info.idx_seq*ch_info.e_len;
        bool needCheckPosB = format_trs.GetPrmMeas()->layout_type != SIP_POLE3_SOUNDING;
        if(format_trs.GetPrmMeas()->layout_type != SIP_ALL_GRADIENT && !ui->rd3DSounding_2->isChecked()){//非任意装置
            idx = GetIndxOfAcq(&vt_acq_layout, &acq_layout, needCheckPosB);
            if(idx < 0){ //无同类装置
                vt_acq_layout.append(acq_layout);
                vt_files->append(QStringList(file_name));
            }else{ //有同类装置
                (*vt_files)[idx].append(file_name);
            }
        }else{//任意装置的同类装置判断待定，目前暂时都做非同类装置处理
            vt_files->append(QStringList(file_name));
        }
    }
    return result;
}

void CWdtOutPut::MergeRes(int arr_num, CFormatTRS res_arr[], CFormatTRS *res_dest, bool needCheckPosB)
{
    int iArr, iCh, idx;
    QList<float> vt_pos_m; //从小到大的m坐标(针对单条接收测线，多接收测线还需要考虑AB距)
    float pos_m, dist2AB, e_len, Ax, Ay, Bx, By;
    CFormatTRS *res_tmp = NULL;
    CResult::ChInfo_S ch_info;
    CTdipResultCal::ResItem_S res_item, res_item_tmp;

    assert(arr_num > 0);
    Ax = res_arr[0].GetResultCal()->GetPosAx();
    Ay = res_arr[0].GetResultCal()->GetPosAy();
    Bx = res_arr[0].GetResultCal()->GetPosBx();
    By = res_arr[0].GetResultCal()->GetPosBy();
    //求取所有融合数据的m位置数组（按升序排列）
    for(iArr=0; iArr<arr_num; iArr++){
        res_tmp = &res_arr[iArr];
        assert(Ax == res_tmp->GetResultCal()->GetPosAx());
        assert(Ay == res_tmp->GetResultCal()->GetPosAy());
        if(needCheckPosB){
            assert(Bx == res_tmp->GetResultCal()->GetPosBx());
            assert(By == res_tmp->GetResultCal()->GetPosBy());
        }
        for(iCh=0; iCh<res_tmp->GetResultCal()->GetChNum(); iCh++){
            dist2AB = res_tmp->GetResultCal()->GetChInfo(iCh).dist2AB;
            e_len = res_tmp->GetResultCal()->GetChInfo(iCh).e_len;
            pos_m = res_tmp->GetResultCal()->GetChInfo(iCh).pos_m;
            for(idx=vt_pos_m.size()-1; idx>=0; idx--){ //倒序查找更快
                if(pos_m > vt_pos_m[idx]){
                    vt_pos_m.insert(idx+1, pos_m);
                    break;
                }else if(pos_m == vt_pos_m[idx]){
                    break;
                }
            }
            if(idx < 0){//比所有值都小
                vt_pos_m.insert(0, pos_m);
            }
        }
    }
    //初始化输出结果的参数
//    assert(vt_pos_m.size() > 0);
    res_dest->GetResultCal()->ResetBuff(vt_pos_m.size(), 1, 1);
    res_dest->GetResultCal()->SetPosAx(Ax);
    res_dest->GetResultCal()->SetPosAy(Ay);
    res_dest->GetResultCal()->SetPosBx(Bx);
    res_dest->GetResultCal()->SetPosBy(By);
    res_dest->SetPrmMeas(res_arr[0].GetPrmMeas());
    //填充结果
    for(iArr=0; iArr<arr_num; iArr++){
        res_tmp = &res_arr[iArr];
        for(iCh=0; iCh<res_tmp->GetResultCal()->GetChNum(); iCh++){
            ch_info = res_tmp->GetResultCal()->GetChInfo(iCh);
            idx = vt_pos_m.indexOf(ch_info.pos_m);
            res_dest->GetResultCal()->SetChInfo(idx, ch_info);
            res_item = res_dest->GetResultCal()->GetResItem(idx, 0, 0);
            res_item_tmp = res_tmp->GetResultCal()->GetResItem(iCh, 0, 0);
            MergeResItem(res_item_tmp, res_item);
            res_dest->GetResultCal()->SetResItem(idx, 0, 0, res_item);
        }
    }
}

void CWdtOutPut::MergeResItem(CTdipResultCal::ResItem_S &item_src,
                              CTdipResultCal::ResItem_S &item_dest)
{
    if(item_dest.u1 == 0 && item_dest.u2 == 0
            && item_dest.u3 == 0
            && item_dest.u4 == 0){
        item_dest = item_src;
    }else{
        if(item_src.appr_err_value <= item_dest.appr_err_value){
            item_dest = item_src;
        }
    }
//    if(item_dest.items[type].cnt<2){//目标数据采集小于2次时直接使用源数据
//        item_dest = item_src;
//    }else{ //目标数据采集次数至少两次
//        if(item_src.items[type].cnt >= 2){ //源数据采集次数至少两次
//            err_src = item_src.items[type].err_abs/item_src.items[type].avg;
//            err_dest = item_dest.items[type].err_abs/item_dest.items[type].avg;
//            if(err_src < err_dest){
//                item_dest = item_src;
//            }
//        }
//    }
//    //***少于2次数据归为0次
//    if(item_dest.items[type].cnt<2){
//        item_dest = CResult::GetEmptyItem();
//    }
}

QString CWdtOutPut::GetFirstTrsDir() const
{
    QString strRet;

    if(m_file_names_trs->size() > 0){
        strRet = GetPathFromFullFileName(m_file_names_trs->at(0));
    }
    return strRet;
}

SipLayout_T CWdtOutPut::GetSipLayout()
{
    SipLayout_T layout_type = SIP_MID_GRADIENT;

    if(ui->rdMidGradient_2->isChecked()){ //一维中梯扫面
        layout_type = SIP_MID_GRADIENT;
    }else if(ui->rd2DSounding_2->isChecked()){ //二维测深
        layout_type = GetTrsLayout();
        if(layout_type == SIP_MID_GRADIENT){  //中梯装置的数据可以作为四极测深的数据来处理
            layout_type = SIP_POLE4_SOUNDING;
        }
    }else if(ui->rdCheckErr_2->isChecked()){ //检查点误差
        layout_type = GetTrsLayout();
    }else if(ui->rd3DSounding_2->isChecked()){
        if(ui->rdOutSection_2->isChecked()){
            if(ui->rdType4_2->isChecked()){
                layout_type = SIP_POLE4_SOUNDING;
            }else{
                layout_type = SIP_POLE3_SOUNDING;
            }
        }else{
            layout_type = m_first_layout;
        }
    }else{
        assert(0);
    }
    return layout_type;
}

void CWdtOutPut::on_btnImportFileCoord_2_clicked()
{
    static QString strPath;
    QString strFileName = QFileDialog::getOpenFileName(this,
                                                       tr("打开坐标文件"),
                                                       strPath,
                                                       tr("txt Files (*.txt)"));
    if(strFileName.isEmpty())
        return;
    strPath = GetPathFromFullFileName(strFileName);
    string file_name = ex_u2g(strFileName.toStdString());
    if(!m_site_coord.ReadFile(file_name)){
        QMessageBox::warning(this, tr("提示"), tr("导入坐标失败！"));
        ui->edtInputFileCoord_2->setText(tr(""));
    }else{
        ui->edtInputFileCoord_2->setText(strFileName);
    }
}

void CWdtOutPut::on_btnImportFileElev_2_clicked()
{
    static QString strPath;
    QString strFileName = QFileDialog::getOpenFileName(this,
                                                       tr("打开高程文件"),
                                                       strPath,
                                                       tr("txt Files (*.txt)"));
    if(strFileName.isEmpty())
        return;
    strPath = GetPathFromFullFileName(strFileName);
    string file_name = ex_u2g(strFileName.toStdString());
    if(!m_elev.ReadFile(file_name)){
        QMessageBox::warning(this, tr("提示"), tr("导入高程失败！"));
        ui->edtInputFileElev_2->setText(tr(""));
    }else{
        ui->edtInputFileElev_2->setText(strFileName);
    }
}

void CWdtOutPut::on_btnImportFileShifting_2_clicked()
{
    static QString strPath;
    QString strFileName = QFileDialog::getOpenFileName(this,
                                                       tr("打开坐标文件"),
                                                       strPath,
                                                       tr("txt Files (*.csv)"));
    if(strFileName.isEmpty())
        return;
    strPath = GetPathFromFullFileName(strFileName);
    string file_name = ex_u2g(strFileName.toStdString());
    if(!m_shifting.ReadFile(file_name)){
        QMessageBox::warning(this, tr("提示"), tr("导入相对坐标失败！"));
        ui->edtInputFileShifting_2->setText(tr(""));
    }else{
        ui->edtInputFileShifting_2->setText(strFileName);
    }
}
