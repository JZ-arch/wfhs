#ifndef LOGIN_H
#define LOGIN_H

#include <QDialog>
#include "room.h"
#include "user.h"
#include "widget.h"

namespace Ui {
class Login;
}

class Login : public QDialog
{
    Q_OBJECT

public:
    explicit Login(QList<user *> &m_userlist, QWidget * parent = 0);
    ~Login();
    int get_selection() const {return selection;}
    user* get_user_loged() const {return user_loged;}

private slots:


    void on_pushButtonLogin_clicked();

    void on_pushButtonCancel_clicked();


private:
    Ui::Login *ui;
    QList<user *> m_puserList;
    int selection;
    user *user_loged;
};

#endif // LOGIN_H
