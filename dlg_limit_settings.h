#ifndef DLG_LIMIT_SETTINGS_H
#define DLG_LIMIT_SETTINGS_H

#include <QWidget>
#include <QDialog>
#include "data_threshold.h"

namespace Ui {
class CdlgLimitSettings;
}

class CdlgLimitSettings : public QDialog
{
    Q_OBJECT

public:
    explicit CdlgLimitSettings(CDataThreshold *threshold, QWidget *parent = 0);
    ~CdlgLimitSettings();

    void UpdateData(bool bSaveOrShow);

private:
    void UpdateUiState();

private slots:
    void onBtnRestoreDefault();
    void onBtnOk();
    void onBtnCancel();
    void onChkApprErr();
    void onChkAppcErr();

private:
    Ui::CdlgLimitSettings *ui;

    CDataThreshold *m_threshold;
};

#endif // DLG_LIMIT_SETTINGS_H
