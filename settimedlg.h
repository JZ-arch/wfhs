#ifndef SETTIMEDLG_H
#define SETTIMEDLG_H

#include <QDialog>
#include "3rd-party/digit/digit.h"

namespace Ui {
class SetTimeDlg;
}

class SetTimeDlg : public QDialog
{
    Q_OBJECT

public:
    explicit SetTimeDlg(int hour, int minute, QWidget *parent = 0);
    ~SetTimeDlg();
    int getHour() const {return m_hour;}
    int getMinute() const {return m_minute;}

private slots:
    void on_pushButtonHourDec_clicked();
    void on_pushButtonHourInc_clicked();
    void on_pushButtonMinuteDec_clicked();
    void on_pushButtonMinuteInc_clicked();
    //void on_pushButtonOk_clicked();
    //void on_pushButtonCancel_clicked();





private:
    Ui::SetTimeDlg *ui;
    Digit *m_pDigitHour, *m_pDigitMinute;
    int m_hour, m_minute;
};

#endif // SETTIMEDLG_H
