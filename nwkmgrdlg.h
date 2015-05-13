#ifndef NWKMGRDLG_H
#define NWKMGRDLG_H

#include <QDialog>
#include "3rd-party/digit/digit.h"

namespace Ui {
class NwkMgrDlg;
}

class NwkMgrDlg : public QDialog
{
    Q_OBJECT

public:
    explicit NwkMgrDlg(QWidget *parent = 0);
    int get_panid() const {return (panidhighbit * 100 + panidlowbit);}
    ~NwkMgrDlg();

private slots:
    void on_pushButtonHighAdd_clicked();

    void on_pushButtonLowAdd_clicked();

    void on_pushButtonHighDec_clicked();

    void on_pushButtonLowDec_clicked();

    void on_pushButtonOk_clicked();

    void on_pushButtonCancel_clicked();

private:
    Ui::NwkMgrDlg *ui;
    int panidhighbit;
    int panidlowbit;
    Digit * m_DigitpanidHigt,* m_DigitpanidLow;
};

#endif // NWKMGRDLG_H
