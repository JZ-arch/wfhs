#include "user.h"

user::user(QObject *parent) : QObject(parent)
{
   user_name = "";
   user_code = "";
   user_priority = -1;
   user_id = 0;
}

/*user::user(user  &copy)
{
    user_name = copy.get_user_name();
    user_code = copy.get_user_code();
    user_id = copy.get_user_id();
    user_priority = copy.get_user_priority();

}*/

user::~user()
{

}


void user::set_user_name(QString  name)
{
   this->user_name =  name;
}


void user::set_user_code(QString code)
{
  this->user_code =  code;
}

void user::set_user_priority(int priority)
{
  this->user_priority =  priority;
}

 void user:: set_user_id(int id)
 {
     this->user_id =  id;
 }
