#ifndef ADDUSERDLG_H
#define ADDUSERDLG_H

#include <QDialog>
#include "user.h"


namespace Ui {
class adduserdlg;
}

class adduserdlg : public QDialog
{
    Q_OBJECT

public:
    explicit adduserdlg(QList<user *> &puserList, QWidget *parent = 0);
    ~adduserdlg();
    user * get_user_Added() const {return p_user;}

private slots:
    void on_pushButton_Ok_clicked();

    void on_radioButtonLow_clicked();

    void on_radioButtonMid_clicked();

    void on_radioButtonHigh_clicked();

private:
    Ui::adduserdlg *ui;
    QList<user *> m_puserList;
    user * p_user;
    int priority;

};

#endif // ADDUSERDLG_H
