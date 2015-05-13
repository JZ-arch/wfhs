#include "logindlg.h"
#include "ui_login.h"
#include <QDebug>
//#include ""



Login::Login(QList<user *> &m_userlist,QWidget *parent) :
    QDialog(parent),\
    ui(new Ui::Login),
    m_puserList(m_userlist),
    selection(0),
    user_loged(NULL)
{
    ui->setupUi(this);

    //connect(ui->pushButtonOk, SIGNAL(clicked()), this, SLOT(accept()));
    connect(ui->pushButtonCancel, SIGNAL(clicked()), this, SLOT(reject()));
}

Login::~Login()
{
    delete ui;
}


void Login::on_pushButtonLogin_clicked()
{
    bool userexist = false;
    bool log = false;
    QString user_name = ui->lineEditUserName->text();
    QString user_code = ui->lineEditUserCode->text();
    QString title  = "登录失败";
    QString title1 = "用户名不存在";
   // QString a = "陶鹏";
   // qDebug() << user_name;
   //  qDebug() << user_code;
    foreach(user * puser, m_puserList)
    {
    //   qDebug() << puser->get_user_name();
       if(user_name == puser->get_user_name())//
       {

           userexist = true;
           if(puser->get_user_code() == user_code)
           {
               log = true;
               user_loged = puser;
               break;
           }

       }
    }
    if(!userexist)
    {
        QMessageBox::information(this,title,title1);
        ui->lineEditUserName->clear();
        ui->lineEditUserCode->clear();
    }
    else
    {
        if(log)
        {
            QDialog::accept();
        }
        else
        {
           QMessageBox::information(this,title,tr("密码不正确"));
           ui->lineEditUserCode->clear();
        }
    }
}

void Login::on_pushButtonCancel_clicked()
{
    QDialog::reject();
}

