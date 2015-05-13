#include "creatadminstratordlg.h"
#include "ui_creatadminstratordlg.h"
#include <QMessageBox>

CreatAdminstratorDlg::CreatAdminstratorDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CreatAdminstratorDlg)
{
    ui->setupUi(this);
}

CreatAdminstratorDlg::~CreatAdminstratorDlg()
{
    delete ui;
}

void CreatAdminstratorDlg::on_pushButtonOk_clicked()
{
   // QString name = ui->lineEdit_Name;
    QString code = ui->lineEdit_Code->text();
    QString ensure_code = ui->lineEdit_Ensure_Code->text();
    if(code == ensure_code )
    {
        admin = new user;
        admin->set_user_name(ui->lineEdit_Name->text());
        admin->set_user_code(code);
        admin->set_user_priority(1);
        accept();
    }
    else
    {
        QMessageBox::information(0,tr("提醒"),tr("两次密码不一致！"));
        ui->lineEdit_Code->clear();
        ui->lineEdit_Ensure_Code->clear();
    }

}

void CreatAdminstratorDlg::on_lineEdit_Name_editingFinished()
{
    QString name = ui->lineEdit_Name->text();
    QString code = ui->lineEdit_Code->text();
    QString ensure_code = ui->lineEdit_Ensure_Code->text();
    if(!name.isEmpty() && !code.isEmpty() && !ensure_code.isEmpty())
    {
        ui->pushButtonOk->setEnabled(true);
    }
    else
    {
        ui->pushButtonOk->setEnabled(false);
    }
}

void CreatAdminstratorDlg::on_lineEdit_Code_editingFinished()
{
    QString name = ui->lineEdit_Name->text();
    QString code = ui->lineEdit_Code->text();
    QString ensure_code = ui->lineEdit_Ensure_Code->text();
    if(!name.isEmpty() && !code.isEmpty() && !ensure_code.isEmpty())
    {
        ui->pushButtonOk->setEnabled(true);
    }
    else
    {
        ui->pushButtonOk->setEnabled(false);
    }
}

void CreatAdminstratorDlg::on_lineEdit_Ensure_Code_editingFinished()
{
    QString name = ui->lineEdit_Name->text();
    QString code = ui->lineEdit_Code->text();
    QString ensure_code = ui->lineEdit_Ensure_Code->text();
    if(!name.isEmpty() && !code.isEmpty() && !ensure_code.isEmpty())
    {
        ui->pushButtonOk->setEnabled(true);
    }
    else
    {
        ui->pushButtonOk->setEnabled(false);
    }
}
