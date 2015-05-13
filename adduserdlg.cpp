#include "adduserdlg.h"
#include "ui_adduserdlg.h"
#include <QMessageBox>

adduserdlg::adduserdlg(QList<user *> &userList, QWidget *parent) :
    QDialog(parent),\
    ui(new Ui::adduserdlg),\
    m_puserList(userList),
    priority(0)
{
    ui->setupUi(this);
   // if the cancel button is clicked
    connect(ui->pushButton_Cancel, SIGNAL(clicked()), this, SLOT(reject()));
}

adduserdlg::~adduserdlg()
{
    delete ui;
}

void adduserdlg::on_pushButton_Ok_clicked()
{
    bool name_legal = true;// the user's name is legal or not
   // bool password_legal = false;// the password is legal or not
    QString user_name = ui->lineEdit_Name->text();
    QString user_code = ui->lineEdit_Code->text();
    QString user_ensure_code = ui->lineEdit_Ensure_Code->text();
    if(!user_name.isEmpty() && !user_code.isEmpty() && !user_ensure_code.isEmpty() && priority) // the line_edit can not be empty!
    {
        if(user_code == user_ensure_code)
        {
          //  password_legal = true;// the password is ok
           foreach(user * puser,m_puserList)
           {
               if(user_name == puser->get_user_name())
               {
                   name_legal = false;
                   QMessageBox::information(0,tr("错误！"),tr("用户已存在！"));
                   ui->lineEdit_Name->clear();
                   break;
               }

           }
           if(name_legal)
           {
               p_user = new user;
               p_user->set_user_name(user_name);
               p_user->set_user_code(user_code);
               p_user->set_user_priority(priority);
               p_user->set_user_id(m_puserList.length() + 1);// set the id of each user
               QMessageBox::information(0,tr("提醒"),tr("用户添加成功！"));
               QDialog::accept();
           }

        }
        else
        {
            QMessageBox::information(0,tr("错误！"),tr("两次密码不一致"));
            ui->lineEdit_Code->clear();
            ui->lineEdit_Ensure_Code->clear();
        }
    }
    else
    {
         QMessageBox::information(0,tr("错误！"),tr("信息不完整"));
    }

}

void adduserdlg::on_radioButtonLow_clicked()
{
    priority = 3;
}

void adduserdlg::on_radioButtonMid_clicked()
{
    priority = 2;
}

void adduserdlg::on_radioButtonHigh_clicked()
{
    priority = 1;
}
