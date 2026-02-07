#include "shared_data.h"

SharedData* SharedData::instance() {
    static SharedData instance;
    return &instance;
}

SharedData::SharedData(QObject* parent) : QObject(parent) {}

void SharedData::SetLimitInfo(vector<LimitItem_S> *vt_limit)
{
    m_vt_limit = vt_limit;
}
