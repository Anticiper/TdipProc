#ifndef SHARED_DATA_H
#define SHARED_DATA_H

#include <QObject>
#include "tdip_data_out.h"
#include "tdip_def.h"

class SharedData : public QObject {
    Q_OBJECT
public:
    static SharedData* instance();

    void SetLimitInfo(vector<LimitItem_S> *vt_limit);
    vector<LimitItem_S>* GetLimitInfo(){return m_vt_limit;}
private:
    SharedData(QObject* parent = nullptr);

    vector<LimitItem_S> *m_vt_limit;
};

#endif // SHARED_DATA_H
