#ifndef USER_H
#define USER_H

#include <QObject>
//#include <QString>

class user : public QObject
{
    Q_OBJECT
public:
    explicit user(QObject *parent = 0);
    //user(user  &copy);
    ~user();

    QString get_user_name() const {return user_name;}
    QString get_user_code() const {return user_code;}
    int get_user_priority() const {return user_priority;}
    int get_user_id() const {return user_id;}

    void set_user_name(QString);
    void set_user_code(QString);
    void set_user_priority(int);
    void set_user_id(int);

private:
    QString user_name;
    QString user_code;
    int user_priority;
    int user_id;

signals:

public slots:
};

#endif // USER_H
