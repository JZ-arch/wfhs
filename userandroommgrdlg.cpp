#include "userandroommgrdlg.h"
#include "ui_userandroommgrdlg.h"
#include "addroomdlg.h"
#include "adduserdlg.h"
#include <QMessageBox>

UserAndRoomMgrDlg::UserAndRoomMgrDlg(QList<Room *> &roomList, QList<user *> &userList, user *&user_loged, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UserAndRoomMgrDlg),\
    m_roomList(roomList),\
    m_userList(userList),
    m_user_loged(user_loged),
    SelectedRow(-1),
    room_list_selected(true)

{
    // 设置好treeView
    ui->setupUi(this);
    ui->treeView->setFocusPolicy(Qt::NoFocus);//去除item选中时的虚线边框
    ui->treeView->setSelectionBehavior(QAbstractItemView::SelectRows);//行选中

    // 设置好代理的指针
    m_pRoomPriorityDelegate=  new ComboBoxDelegate(this);
    m_pRoomIdDelegate  =  new ReadOnlyDelegate(this);

    m_pUserNameDelegate = new ReadOnlyDelegate(this);
    m_pUserIdDelegate = new ReadOnlyDelegate(this);
    m_pUserPriorityDelegate = new ReadOnlyDelegate(this);

    //初始化treeView的房间列表标题行
    m_pRoomModel = new QStandardItemModel(0, 3);//row参数无关紧要，setItem时会自动增加
    m_pRoomModel->setHeaderData(0, Qt::Horizontal, tr("编号"));
    m_pRoomModel->setHeaderData(1, Qt::Horizontal, tr("名称"));
    m_pRoomModel->setHeaderData(2, Qt::Horizontal, tr("优先级"));

    //初始化treeView的用户列表标题行
    m_pUserModel = new QStandardItemModel(0, 3);//row参数无关紧要，setItem时会自动增加
    m_pUserModel->setHeaderData(0, Qt::Horizontal, tr("序号"));
    m_pUserModel->setHeaderData(1, Qt::Horizontal, tr("用户名"));
    m_pUserModel->setHeaderData(2, Qt::Horizontal, tr("权限"));


    //  m_pRoomModel->setHeaderData(3, Qt::Horizontal, tr("测试"));
    ui->treeView->setModel(m_pRoomModel);
    // ui->treeViewRoom->setMo

    //添加item到m_pRoomModel的记录行
    int row = 0;
    foreach (Room *pRoom, m_roomList)
    {
       addItemToModel(pRoom, row++);
    }

    //添加item到m_pUserModel的记录行
    row = 0;
    foreach (user *pUser, m_userList)
    {
       addItemToModel(pUser, row++);
    }


    //房间优先级列ComboBox委托
    ui->treeView->setItemDelegateForColumn(\
                TREEVIEW_COL_ROOMPRIO,m_pRoomPriorityDelegate);
    //房间编号列只读委托
    ui->treeView->setItemDelegateForColumn(\
                TREEVIEW_COL_ROOMIDX,m_pRoomIdDelegate );

    //列宽自动适应内容
    for (int i = 0; i < 3; i++)
    {
        ui->treeView->resizeColumnToContents(i);
    }

    connect(m_pRoomModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)), \
        this, SLOT(slotRoomDataChanged(QModelIndex,QModelIndex)));
   // connect(m_pUserModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)),
      //  this, SLOT(slotRoomPrioChanged(QModelIndex,QModelIndex)));
   if(m_user_loged->get_user_priority() == 1)
   {
      ui->pushButtonAdd->setEnabled(true);
      //ui->pushButtonDel->setEnabled(true);
      ui->pushButtonOk->setEnabled(true);
   }

}


UserAndRoomMgrDlg::~UserAndRoomMgrDlg()
{
    delete m_pRoomIdDelegate;
    delete m_pRoomPriorityDelegate;
    delete m_pUserNameDelegate;
    delete m_pUserIdDelegate;
    delete m_pUserPriorityDelegate;

    delete m_pUserModel;
    delete m_pRoomModel;

    delete ui;
}

void UserAndRoomMgrDlg::addItemToModel(Room *pRoom, int row)
{
    Q_ASSERT(pRoom != NULL);

    QStandardItem *pItem = new QStandardItem(QString::number(pRoom->getRoomIdx()));
    m_pRoomModel->setItem(row, TREEVIEW_COL_ROOMIDX, pItem);

    pItem = new QStandardItem(pRoom->getRoomName());
    m_pRoomModel->setItem(row, TREEVIEW_COL_ROOMNAME, pItem);

    pItem = new QStandardItem(pRoom->getRoomPrioStr());
    m_pRoomModel->setItem(row, TREEVIEW_COL_ROOMPRIO, pItem);
}


void UserAndRoomMgrDlg::addItemToModel(user * puser,int row)
{
    QStandardItem *pitem = new QStandardItem(QString::number(row + 1));
    m_pUserModel->setItem(row,LIST_COL_USER_ID,pitem);

    pitem = new QStandardItem(puser->get_user_name());
    m_pUserModel->setItem(row,LIST_COL_USER_NAME,pitem);

    QString PrioritytoStr;
    switch(puser->get_user_priority())
    {
        case 1:
            PrioritytoStr = tr("高");
            break;
        case 2:
            PrioritytoStr = tr("中");
            break;
        case 3:
            PrioritytoStr = tr("低");
            break;
    }

    pitem = new QStandardItem(PrioritytoStr);
    m_pUserModel->setItem(row,LIST_COL_USER_PRIORITY,pitem);
}


void UserAndRoomMgrDlg::on_pushButtonRoom_clicked()
{
    room_list_selected = true;
    ui->treeView->setModel(m_pRoomModel);
    //优先级列ComboBox委托
    ui->treeView->setItemDelegateForColumn(\
                TREEVIEW_COL_ROOMPRIO,m_pRoomPriorityDelegate);
    //房间编号列只读委托
    ui->treeView->setItemDelegateForColumn(\
                TREEVIEW_COL_ROOMIDX, m_pRoomIdDelegate);
    //房间名不委托
    ui->treeView->setItemDelegateForColumn(\
                TREEVIEW_COL_ROOMNAME,0);
    for (int i = 0; i < 3; i++)
    {
        ui->treeView->resizeColumnToContents(i);
    }
    ui->pushButtonDel->setEnabled(false);
    if(m_user_loged->get_user_priority() == 2)
    {
        ui->pushButtonAdd->setEnabled(true);
        ui->pushButtonOk->setEnabled(true);
    }
}


void UserAndRoomMgrDlg::on_pushButtonUser_clicked()
{
    room_list_selected = false;
    ui->treeView->setModel(m_pUserModel);
    //用户id只读委托
    ui->treeView->setItemDelegateForColumn(\
                LIST_COL_USER_ID, m_pUserIdDelegate);
    //用户名编号列只读委托
    ui->treeView->setItemDelegateForColumn(\
                LIST_COL_USER_NAME, m_pUserNameDelegate);
    //用户权限只读委托
    ui->treeView->setItemDelegateForColumn(\
                LIST_COL_USER_PRIORITY, m_pUserPriorityDelegate);
    for (int i = 0; i < 3; i++)
    {
        ui->treeView->resizeColumnToContents(i);
    }
    ui->pushButtonDel->setEnabled(false);
    if(m_user_loged->get_user_priority() == 2)
    {
        ui->pushButtonAdd->setEnabled(false);
        ui->pushButtonOk->setEnabled(false);
    }
}

void UserAndRoomMgrDlg::on_pushButtonAdd_clicked()
{
    if(room_list_selected)
    {
        AddRoomDlg dlg;
        dlg.setWindowModality(Qt::WindowModal);

        //必须把房间设置按钮屏蔽，要不然如果可以按，可以弹出好多个重复窗口
         ui->pushButtonAdd->setEnabled(false);
        if (dlg.exec() == QDialog::Accepted)
        {
            Room *pRoom = dlg.getNewRoom();

            bool isRepeated = false;
            int len = m_pRoomModel->rowCount();
            for (int i = 0; i != len; i++)
            {
                if (pRoom->getRoomIdx() == m_pRoomModel->item(i)->text().toInt())
                {
                    isRepeated = true;
                    break;
                }
            }

            if (!isRepeated)//房间编号无重复，添加房间
            {
                m_roomAdd.append(pRoom);
                m_roomList.append(pRoom);
                addItemToModel(pRoom, m_pRoomModel->rowCount());
                m_pRoomModel->sort(TREEVIEW_COL_ROOMIDX);// 显示的时候排个序(降序)

                QMessageBox::information(NULL, tr("提示"), \
                         QString(tr("房间：#%1 %2 添加成功！")\
                                 .arg(pRoom->getRoomIdx())\
                                 .arg(pRoom->getRoomName())));
            }
            else
            {
                QMessageBox::warning(NULL, tr("提示"), tr("房间编号重复，添加失败！"));
            }
            // 必须释放
             ui->pushButtonAdd->setEnabled(true);
        }
        else
        {     // 必须释放
             ui->pushButtonAdd->setEnabled(true);
        }
    }
    else
    {
        adduserdlg dlg(m_userList);
       //  ui->pushButtonAdd->setEnabled(false);
        if(dlg.exec() == QDialog::Accepted)
        {
           user * puser = dlg.get_user_Added();
           m_userList.append(puser);// for detect the user name overlaped
           m_userAdd.append(puser);
           addItemToModel(dlg.get_user_Added(),m_pUserModel->rowCount());
       //    ui->pushButtonAdd->setEnabled(true);
        }
      //  else
     //   {
       //    ui->pushButtonAdd->setEnabled(true);
      //  }
    }
}

void UserAndRoomMgrDlg::on_pushButtonDel_clicked()
{
   if(room_list_selected)
   {
       QString roomName = m_pRoomModel->index(\
                  SelectedRow, TREEVIEW_COL_ROOMNAME).data().toString();

       int ret = QMessageBox::information(NULL, tr("提示"), \
                  QString(tr("是否删除房间：%1？")).arg(roomName), QMessageBox::Yes | QMessageBox::No);
       if (ret == QMessageBox::Yes)
       {

           int roomIdx = m_pRoomModel->index(\
                       SelectedRow, TREEVIEW_COL_ROOMIDX).data().toInt();

           foreach (Room *pRoom, m_roomList)
           {
               if (pRoom->getRoomIdx() == roomIdx)
               {
                   m_roomDelete.append(pRoom);
                  // m_roomAdd.removeOne(pRoom);
                   m_roomList.removeOne(pRoom);
                   m_pRoomModel->removeRow(SelectedRow);
                   break;
               }
           }

       }
   }
   else
   {
       QString user_name = m_pUserModel->index(SelectedRow,LIST_COL_USER_NAME).data().toString();
      // int id = m_pUserModel->index(SelectedRow,LIST_COL_USER_ID).data().toInt();
      int ret = QMessageBox::information(0,tr("提醒!"), QString(tr("是否删除用户:%1？")).arg(user_name),QMessageBox::Yes | QMessageBox::No);
      if(ret == QMessageBox::Yes)//&&  user_name !=m_user_loged->get_user_name())
      {
          foreach(user * puser, m_userList)
          {
              if(user_name == puser->get_user_name())// delete the user except the user administrator
               {
                  if(puser->get_user_priority() != 1)
                  {
                      m_userDel.append(puser);// the user to be delete
                      m_userList.removeOne(puser);
                      m_pUserModel->removeRow(SelectedRow);
                      updateUserModel();
                      break;
                  }
                 /* else if(id != 1)
                  {
                      m_userDel.append(puser);// the user to be delete
                      m_userList.removeOne(puser);
                      m_pUserModel->removeRow(SelectedRow);

                  }*/
                  else
                  {
                      QMessageBox::information(0,tr("提醒!"),tr("不可以删除管理员"));
                      break;
                  }
               }
          }

      }
      /*//else
      //{
     //     if(ret == QMessageBox::Yes)
     //     {
     //         QMessageBox::information(0,tr("提醒!"),tr("不可以删除本人"));
      //    }

    //  }*/
   }
   SelectedRow = -1;
   ui->treeView->clearSelection();
   ui->pushButtonDel->setEnabled(false);
}

void UserAndRoomMgrDlg::on_pushButtonOk_clicked()
{
   if(!m_userAdd.isEmpty() || !m_userDel.isEmpty() || !m_roomAdd.isEmpty() || !m_roomDelete.isEmpty() || !m_roomUpdate.isEmpty())
   {
       accept();
   }
   else
   {
       reject();
   }
}

void UserAndRoomMgrDlg::on_pushButtonCancel_clicked()
{
    reject();
}

void UserAndRoomMgrDlg::on_treeView_doubleClicked(const QModelIndex &index)
{
    SelectedRow = index.row();
    if((m_user_loged->get_user_priority() == 1) || ((m_user_loged->get_user_priority() == 2) && (room_list_selected)))
    {
        ui->pushButtonDel->setEnabled(true);
    }
}

void UserAndRoomMgrDlg::slotRoomDataChanged(QModelIndex topLeft,QModelIndex bottomRight)
{
    Q_UNUSED(bottomRight);

    int col = topLeft.column();
    int roomIdx = m_pRoomModel->index(topLeft.row(), TREEVIEW_COL_ROOMIDX).data().toInt();

    if (col == TREEVIEW_COL_ROOMNAME)
    {
        QString strRoomName = m_pRoomModel->index(topLeft.row(), TREEVIEW_COL_ROOMNAME).data().toString();
        foreach (Room *pRoom, m_roomList)
        {
            if (roomIdx == pRoom->getRoomIdx())
            {
                 pRoom->setRoomName(strRoomName);
                 m_roomUpdate.append(pRoom);
                 break;
            }
        }
    }
    else if (col == TREEVIEW_COL_ROOMPRIO)
    {
        int roomPrio;
        QString strRoomPrio = m_pRoomModel->index(topLeft.row(), TREEVIEW_COL_ROOMPRIO).data().toString();
        if (strRoomPrio == tr("高"))
        {
            roomPrio = ROOM_PRIO_HIGH;
        }
        else if (strRoomPrio == tr("中"))
        {
            roomPrio = ROOM_PRIO_MID;
        }
        else //if (strRoomPrio == tr("低"))
        {
            roomPrio = ROOM_PRIO_LOW;
        }

        foreach (Room *pRoom, m_roomList)
        {
            if (roomIdx == pRoom->getRoomIdx())
            {
                 pRoom->setRoomPrio(roomPrio);
                 break;
            }
        }
    }
}

void UserAndRoomMgrDlg::updateUserModel()
{
   int rowcount =  m_pUserModel->rowCount();
   int i = 0;
   for( i= 0; i < rowcount; i++)
   {
       QModelIndex index = m_pUserModel->index(i,LIST_COL_USER_ID);
       m_pUserModel->setData(index,QString::number(i + 1));
   }
}
