#ifndef USERANDROOMMGRDLG_H
#define USERANDROOMMGRDLG_H

#include <QDialog>
#include "user.h"
#include "room.h"
#include "comboboxdelegate.h"
#include "readonlydelegate.h"
#include <QStandardItemModel>
#include "global.h"

namespace Ui {
class UserAndRoomMgrDlg;
}

class UserAndRoomMgrDlg : public QDialog
{
    Q_OBJECT

public:
    explicit UserAndRoomMgrDlg(QList<Room *> &roomList, QList<user *> &userList,  user * &user_loged, QWidget *parent = 0);
    ~UserAndRoomMgrDlg();
    QList<Room*> getRoomAdd() const { return m_roomAdd; }
    QList<Room*> getRoomDelete() const { return m_roomDelete; }
    QList<Room*> getRoomUpdate() const { return m_roomUpdate; }
    QList<user*> get_useradded() const {return m_userAdd;}
    QList<user*> get_userdeleted() const {return m_userDel;}

protected:
    void addItemToModel(Room *pRoom, int row);
    void addItemToModel(user *puser, int row);
    void updateUserModel();


private slots:
    void on_pushButtonRoom_clicked();

    void on_pushButtonUser_clicked();

    void on_pushButtonAdd_clicked();

    void on_pushButtonDel_clicked();

    void on_pushButtonOk_clicked();

    void on_pushButtonCancel_clicked();

    void on_treeView_doubleClicked(const QModelIndex &index);

    void slotRoomDataChanged(QModelIndex,QModelIndex);

private:

    // for common use
    Ui::UserAndRoomMgrDlg *ui;
    const user *m_user_loged;//  for the user loged;
    int SelectedRow;// record the row selected now
    bool room_list_selected;// ture if the room list is select,false the user to be

    // for room list
    typedef enum
    {
        TREEVIEW_COL_ROOMIDX = 0, \
        TREEVIEW_COL_ROOMNAME, \
        TREEVIEW_COL_ROOMPRIO
    }treeViewRoomCol_t;

    ComboBoxDelegate* m_pRoomPriorityDelegate;
    ReadOnlyDelegate* m_pRoomIdDelegate;
    QStandardItemModel *m_pRoomModel;
    QList<Room*> m_roomList, m_roomAdd, m_roomDelete, m_roomUpdate;


    //for user list
    typedef enum
    {
        LIST_COL_USER_ID = 0,\
        LIST_COL_USER_NAME,\
        LIST_COL_USER_PRIORITY
    }treeViewUserCol_t;
    ReadOnlyDelegate* m_pUserNameDelegate;
    ReadOnlyDelegate* m_pUserIdDelegate;
    ReadOnlyDelegate* m_pUserPriorityDelegate;
    QStandardItemModel *m_pUserModel;
    QList<user *> m_userList,m_userAdd,m_userDel;
   // user * m_user_loged;


};

#endif // USERANDROOMMGRDLG_H
