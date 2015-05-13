#ifndef CREATADMINSTRATORDLG_H
#define CREATADMINSTRATORDLG_H


#include <QDialog>
#include <user.h>

namespace Ui {
class CreatAdminstratorDlg;
}

class CreatAdminstratorDlg : public QDialog
{
    Q_OBJECT

public:
    explicit CreatAdminstratorDlg(QWidget *parent = 0);
    user * get_admin() const { return admin;}
    ~CreatAdminstratorDlg();

private slots:
    void on_pushButtonOk_clicked();

    void on_lineEdit_Name_editingFinished();

    void on_lineEdit_Code_editingFinished();

    void on_lineEdit_Ensure_Code_editingFinished();

private:
    Ui::CreatAdminstratorDlg *ui;
    user * admin;
};

#endif // CREATADMINSTRATORDLG_H
