#include "widget.h"
#include "ui_widget.h"

#include <QtNetwork>
#include <QStandardItem>
#include <QSignalMapper>
#include <QCloseEvent>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>

#include "global.h"
#include "settimedlg.h"
#include "logindlg.h"
#include "userandroommgrdlg.h"
#include "nwkmgrdlg.h"
#include "creatadminstratordlg.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent), \
    ui(new Ui::Widget), \
    m_pRoomSelected(NULL),
    locked(true),
    loged(false)
{
    ui->setupUi(this);

    initInputMethod();

    initMapParaIdx();

   // QPainter painter;

    initComboBox();
    initDial();
    initTemperatureNumber();
    initToggleButton();

    initDatabase();

    initNavibar();//数据库初始化以后才能初始化Navibar
    initThread();//数据库初始化以后才能初始化子线程

    initDateTime();
 // Login dlg(m_userList);
    // send the get all data of all devices command;
    QByteArray frame(4,0);
    frame[FRAME_FIELD_LEN] = 1;
    frame[FRAME_FIELD_ROOMIDX] = 0;
    frame[FRAME_FIELD_PARAIDX] = PARAIDX_INIT_COOR;
    m_pDataPipeThread->comWritePara(frame);
}

Widget::~Widget()
{
    killTimer(m_secTimerId);
    updateRoomDatabase();
    deinitThread();
    delete ui;
#ifdef _RELEASE
    delete g_im;
#endif
}

void Widget::initInputMethod()
{
#ifdef _RELEASE
    if (g_im == NULL)
    {
        g_im = new SyszuxIM;
    }
#endif
}

void Widget::initMapParaIdx()
{
    int key[] = {\
        PARAIDX_AIRTEMP_SV, \
        PARAIDX_FLOORTEMP_SV, \
        PARAIDX_AIRTEMP_PV, \
        PARAIDX_FLOORTEMP_PV, \
        PARAIDX_RELAY_STATE, \
        PARAIDX_RELAY_SETUP, \
        PARAIDX_SLEEP_SETUP, \
        PARAIDX_TIMER_ON, \
        PARAIDX_TIMER_OFF, \
        PARAIDX_POWER_SETUP, \
    };

    QString value[] = {\
        "PARAIDX_AIRTEMP_SV", \
        "PARAIDX_FLOORTEMP_SV", \
        "PARAIDX_AIRTEMP_PV", \
        "PARAIDX_FLOORTEMP_PV", \
        "PARAIDX_RELAY_STATE", \
        "PARAIDX_RELAY_SETUP", \
        "PARAIDX_SLEEP_SETUP", \
        "PARAIDX_TIMER_ON", \
        "PARAIDX_TIMER_OFF", \
        "PARAIDX_POWER_SETUP", \
    };

    int len = sizeof(key) / sizeof(key[0]);
    for (int i = 0; i != len; i++)
    {
        m_mapParaIdx.insert(key[i], value[i]);
    }
    //qDebug() << m_mapParaIdx;
}

void Widget::initDateTime()
{
    timerEvent(0);//call timerEvent to initialize time display
    m_secTimerId = startTimer(1000);
}

void Widget::initComboBox()
{
    const QString strMode[] = {tr("手动"), tr("智能"), tr("睡眠")};
    int len = sizeof(strMode) / sizeof(QString);
    for (int i = 0; i != len; i++)
    {
        ui->comboBoxMode->addItem(strMode[i]);
    }
}

void Widget::initTemperatureNumber()
{

}

void Widget::initToggleButton()
{
    ui->pushButtonPwr->setStyleSheet( \
                "QPushButton{border-image: url(:/togglebutton/off1.png);}"
                "QPushButton:checked{border-image: url(:/togglebutton/on1.png);}");

    ui->pushButtonTimerOnSwitch->setStyleSheet( \
                "QPushButton{border-image: url(:/togglebutton/off2.png);}"
                "QPushButton:checked{border-image: url(:/togglebutton/on3.png);}");

    ui->pushButtonTimerOffSwitch->setStyleSheet( \
                "QPushButton{border-image: url(:/togglebutton/off2.png);}"
                "QPushButton:checked{border-image: url(:/togglebutton/on3.png);}");

    ui->pushButtonSeting->setStyleSheet( \
                tr("QPushButton{border-image: url(:/navigationbar/设置.png);}"));

}

void Widget::initNavibar()
{
    m_pNavibar = new NavigationBarView;
    foreach (Room *pRoom, m_roomList)
    {
        m_pNavibar->AddItem(pRoom->getRoomName(), pRoom->getRoomIdx());
    }

    connect(m_pNavibar, SIGNAL(itemSelected(int)), \
            this, SLOT(slotNavibarSelected(int)));

    ui->layoutNavibar->addWidget(m_pNavibar);

    ui->pushButtonLeft->setStyleSheet( \
                "QPushButton{border-image: url(:/togglebutton/left.png);}"
                "QPushButton:pressed{border-image: url(:/togglebutton/left_pressed.png);}");

    ui->pushButtonRight->setStyleSheet( \
                "QPushButton{border-image: url(:/togglebutton/right.png);}"
                "QPushButton:pressed{border-image: url(:/togglebutton/right_pressed.png);}");

    connect(ui->pushButtonLeft, SIGNAL(clicked()), m_pNavibar, SLOT(SlotLeftMove()));
    connect(ui->pushButtonRight, SIGNAL(clicked()), m_pNavibar, SLOT(SlotRightMove()));

    //如果房间数据库非空，选择数据表中第一条记录作为当前选中房间
    if (m_roomList.size() != 0)
    {
        m_pRoomSelected = m_roomList[0];
        updateDisplay(m_pRoomSelected);
        m_pNavibar->SelectItem(m_pRoomSelected->getRoomName());
    }
}

void Widget::initDatabase()
{
    QSqlDatabase db;
    if (QSqlDatabase::contains("qt_sql_default_connection"))
    {
       db = QSqlDatabase::database("qt_sql_default_connection");
    }
    else
    {
        db = QSqlDatabase::addDatabase("QSQLITE");
    }

    db.setDatabaseName("room.db");
    if (!db.open())
    {
        qDebug() << db.lastError();
        //initTestRoom();
    }
    else
    {
        QSqlQuery sql;
        sql.prepare("SELECT * FROM ROOM ORDER BY Room_Id");//
        if (!sql.exec())
        {
            qDebug() << sql.lastError();
            //initTestRoom();
        }
        else
        {
            while (sql.next())
            {
                Room* pRoom = new Room();
                pRoom->setRoomIdx(sql.value(COL_ROOM_ID).toInt());
                pRoom->setRoomName(sql.value(COL_ROOM_NAME).toString());
                pRoom->setRoomAirSv(sql.value(COL_ROOM_AIRSV).toInt());
                pRoom->setRoomFloorSv(sql.value(COL_ROOM_FLOORSV).toInt());
                pRoom->setRoomAirPv(sql.value(COL_ROOM_AIRPV).toFloat());
                pRoom->setRoomFloorPv(sql.value(COL_ROOM_FLOORPV).toFloat());
                pRoom->setRoomTimerOn(sql.value(COL_ROOM_TIMERON).toTime());
                pRoom->setRoomTimerOff(sql.value(COL_ROOM_TIMEROFF).toTime());
                pRoom->setRoomMode(sql.value(COL_ROOM_MODE).toInt());
                pRoom->setRoomPrio(sql.value(COL_ROOM_PRIORITY).toInt());
                //qDebug() << sql.value(COL_ROOM_TIMEROFF).toString();
                //qDebug() << sql.value(COL_ROOM_TIMEROFF).toTime();
                //qDebug() << pRoom->getRoomTimerOff();
                m_roomList.append(pRoom);
            }
        }

        sql.prepare("SELECT * FROM USER  ORDER BY User_Priority");// 读出一个根据User_Id 排序的结果
        if(sql.exec())
        {
          //  int id = 1;
            while(sql.next())
            {
               user * puser = new user();
               puser->set_user_name(sql.value(COL_USER_NAME).toString());
               puser->set_user_code(sql.value(COL_USER_CODE).toString());
               puser->set_user_priority(sql.value(COL_USER_PRIORITY).toInt());
              //  puser->set_user_id(id);
              // id++;
               m_userList.append(puser);//加入链表
               qDebug() << sql.value(COL_USER_NAME).toString();
               qDebug() << sql.value(COL_USER_CODE).toString();
               qDebug() << sql.value(COL_USER_PRIORITY).toInt();

             }

        }
        db.close();
    }
}

void Widget::updateRoomDatabase()
{
    QSqlDatabase db;
    if (QSqlDatabase::contains("qt_sql_default_connection"))
    {
       db = QSqlDatabase::database("qt_sql_default_connection");
    }
    else
    {
        db = QSqlDatabase::addDatabase("QSQLITE");
    }
    db.setDatabaseName("room.db");
    if (!db.open())
    {
        qDebug() << db.lastError();
    }
    else
    {
        QSqlQuery sql;
        foreach (Room *pRoom, m_roomList)
        {

            sql.prepare( \
                "UPDATE room "
                "SET Room_Name = ?, "
                "Room_AirSv = ?, Room_FloorSv = ?, "
                "Room_TimerOn = ?, Room_TimerOff = ?, "
                "Room_Mode = ?, Room_Priority = ?"
                "WHERE Room_Id = ?");
            sql.addBindValue(pRoom->getRoomName());
            sql.addBindValue(pRoom->getRoomAirSv());
            sql.addBindValue(pRoom->getRoomFloorSv());
            sql.addBindValue(pRoom->getRoomTimerOn());
            sql.addBindValue(pRoom->getRoomTimerOff());
            sql.addBindValue(pRoom->getRoomMode());
            sql.addBindValue(pRoom->getRoomPrio());
            sql.addBindValue(pRoom->getRoomIdx());
            if (!sql.exec())
            {
                qDebug() << sql.lastError();
            }
            else
            {

            }
        }
        db.close();
    }
}

bool Widget::deleteDatabase(int roomIdx)
{
    bool ret;
    QSqlDatabase db;
    if (QSqlDatabase::contains("qt_sql_default_connection"))
    {
       db = QSqlDatabase::database("qt_sql_default_connection");
    }
    else
    {
        db = QSqlDatabase::addDatabase("QSQLITE");
    }
    db.setDatabaseName("room.db");
    if (!db.open())
    {
        qDebug() << db.lastError();
        ret = false;
    }
    else
    {
        QSqlQuery sql;
        sql.prepare("DELETE FROM room WHERE Room_Id = ?");
        sql.addBindValue(roomIdx);
        if (!sql.exec())
        {
            qDebug() << sql.lastError();
            ret = false;
        }
        else
        {
            ret = true;
        }
        db.close();
    }
    return ret;
}

bool Widget::addDatabase(Room *pRoom)
{
    bool ret;
    QSqlDatabase db;
    if (QSqlDatabase::contains("qt_sql_default_connection"))
    {
       db = QSqlDatabase::database("qt_sql_default_connection");
    }
    else
    {
        db = QSqlDatabase::addDatabase("QSQLITE");
    }
    db.setDatabaseName("room.db");
    if (!db.open())
    {
        qDebug() << db.lastError();
        ret = false;
    }
    else
    {
        QSqlQuery sql;
        sql.prepare("INSERT INTO room (Room_Id, Room_Name, Room_Priority) VALUES (?, ?, ?)");
        sql.addBindValue(pRoom->getRoomIdx());
        sql.addBindValue(pRoom->getRoomName());
        sql.addBindValue(pRoom->getRoomPrio());
        if (!sql.exec())
        {
            qDebug() << sql.lastError();
            ret = false;
        }
        else
        {
            ret = true;
        }
        db.close();
    }
    return ret;
}

bool Widget::addDatabase(user* puser)
{
    bool ret;
    QSqlDatabase db;
    if (QSqlDatabase::contains("qt_sql_default_connection"))
    {
       db = QSqlDatabase::database("qt_sql_default_connection");
    }
    else
    {
        db = QSqlDatabase::addDatabase("QSQLITE");
    }
    db.setDatabaseName("room.db");
    if (!db.open())
    {
        qDebug() << db.lastError();
        ret = false;
    }
    else
    {
        QSqlQuery sql;
        sql.prepare("INSERT INTO user (User_Id, User_Name, User_Priority,User_Code) VALUES (?, ?, ?,?)");
        sql.addBindValue(puser->get_user_id());
        sql.addBindValue(puser->get_user_name());
        sql.addBindValue(puser->get_user_priority());
        sql.addBindValue(puser->get_user_code());
        if (!sql.exec())
        {
            qDebug() << sql.lastError();
            ret = false;
        }
        else
        {
            ret = true;
        }
        db.close();
    }
    return ret;
}

bool Widget::deleteDatabase(user* puser)
{
    bool ret;
    QSqlDatabase db;
    if (QSqlDatabase::contains("qt_sql_default_connection"))
    {
       db = QSqlDatabase::database("qt_sql_default_connection");
    }
    else
    {
        db = QSqlDatabase::addDatabase("QSQLITE");
    }
    db.setDatabaseName("room.db");
    if (!db.open())
    {
        qDebug() << db.lastError();
        ret = false;
    }
    else
    {
        QSqlQuery sql;
        sql.prepare("DELETE FROM user WHERE User_Name = ?");
        sql.addBindValue(puser->get_user_name());
        if (!sql.exec())
        {
            qDebug() << sql.lastError();
            ret = false;
        }
        else
        {
            ret = true;
        }
        db.close();
    }
    return ret;
}

void Widget::updateDisplay(Room *pRoom)
{
    if (pRoom == NULL)
    {
        //清空温度显示
        ui->labelAirSv->setText("0");
        ui->labelFloorSv->setText("0");
        ui->labelAirPv->setText("0");
        ui->labelFloorPv->setText("0");

        //关闭定时开关时间显示
        ui->pushButtonTimerOnSwitch->setChecked(false);
        ui->pushButtonTimerOffSwitch->setChecked(false);

        //取消工作模式显示
        ui->comboBoxMode->setCurrentIndex(-1);
    }
    else
    {

        //刷新温度显示
        QString str;
        str.sprintf("%d",pRoom->getRoomAirSv());
        ui->labelAirSv->setText(str);


        str.sprintf("%d",pRoom->getRoomFloorSv());
        ui->labelFloorSv->setText(str);

        if(pRoom->getRoomAirPv() <= 100)
        {
            str.sprintf("%.1f",pRoom->getRoomAirPv());
            ui->labelAirPv->setText(str);
        }
        else
        {
           ui->labelAirPv->setText("Err!");
        }

       // qDebug() << pRoom->getRoomFloorPv() << "aaa";
        if(pRoom->getRoomFloorPv() <= 100)
        {
            str.sprintf("%.1f",pRoom->getRoomFloorPv());
           // qDebug() << str;
            ui->labelFloorPv->setText(str);
        }
        else
        {
            ui->labelFloorPv->setText("Err!");
        }

        //刷新定时开时间显示
        QTime time = pRoom->getRoomTimerOn();
        ui->pushButtonSetTimerOn->setText(time.toString("hh:mm"));
        ui->pushButtonTimerOnSwitch->setChecked(time.second());

        //刷新定时关时间显示
        time = pRoom->getRoomTimerOff();
        ui->pushButtonSetTimerOff->setText(time.toString("hh:mm"));
        ui->pushButtonTimerOffSwitch->setChecked(time.second());

        //刷新工作模式显示
        ui->comboBoxMode->setCurrentIndex(pRoom->getRoomMode());

        //刷新电源开关显示
        ui->pushButtonPwr->setChecked(pRoom->getRoomPwr());
    }
}

void Widget::initThread()
{
    m_pDataPipeThread = new DataPipeThread();
    connect(this, SIGNAL(sigParaChanged(QByteArray)), \
            m_pDataPipeThread, SLOT(tcpWritePara(QByteArray)), \
            Qt::QueuedConnection);
    connect(this, SIGNAL(sigParaChanged(QByteArray)), \
            m_pDataPipeThread, SLOT(comWritePara(QByteArray)), \
            Qt::QueuedConnection);

    m_pComDataParseThread = new DataParseThread(&g_comBuf, &g_comBufUsed, &g_comBufFree);
    connect(m_pComDataParseThread, SIGNAL(sigParaReady(QByteArray)), \
            this, SLOT(slotParaProcess(QByteArray)), \
            Qt::QueuedConnection);

    m_pTcpDataParseThread = new DataParseThread(&g_tcpBuf, &g_tcpBufUsed, &g_tcpBufFree);
    connect(m_pTcpDataParseThread, SIGNAL(sigParaReady(QByteArray)), \
            this, SLOT(slotParaProcess(QByteArray)), \
            Qt::QueuedConnection);

    m_pRoomScheduleThread = new RoomScheduleThread();
    connect(m_pRoomScheduleThread, SIGNAL(sigSendCmd(QByteArray)), \
            m_pDataPipeThread, SLOT(comWritePara(QByteArray)), \
            Qt::QueuedConnection);
    connect(m_pComDataParseThread, SIGNAL(sigAckReceived(QByteArray)), \
            m_pRoomScheduleThread, SLOT(slotAckReceived(QByteArray)), \
            Qt::QueuedConnection);
    connect(this, SIGNAL(sigRoomModeChanged(Room*, bool)), \
            m_pRoomScheduleThread, SLOT(slotRoomModeChanged(Room*, bool)));
    connect(m_pRoomScheduleThread, SIGNAL(sigScheduleError(Room*, int)), \
            this, SLOT(slotScheduleError(Room*, int)));

    /*foreach (Room *pRoom, m_roomList)
    {
        if (pRoom->getRoomMode() == ROOM_MODE_SCHEDULE)
        {
            bool start = pRoom == m_roomList.last();
            m_pRoomScheduleThread->externStartTest(pRoom, start);
        }
    }*/
}

void Widget::deinitThread()
{
    //数据管道线程
    disconnect(this, SIGNAL(sigParaChanged(QByteArray)), \
            m_pDataPipeThread, SLOT(tcpWritePara(QByteArray)));
    disconnect(this, SIGNAL(sigParaChanged(QByteArray)), \
            m_pDataPipeThread, SLOT(comWritePara(QByteArray)));
    m_pDataPipeThread->stop();
    m_pDataPipeThread->wait();
    m_pDataPipeThread->deleteLater();

    //串口数据解析线程
    disconnect(m_pComDataParseThread, SIGNAL(sigParaReady(QByteArray)), \
            this, SLOT(slotParaProcess(QByteArray)));
    m_pComDataParseThread->stop();
    m_pComDataParseThread->wait();
    m_pComDataParseThread->deleteLater();

    //TCP数据解析线程
    disconnect(m_pTcpDataParseThread, SIGNAL(sigParaReady(QByteArray)), \
            this, SLOT(slotParaProcess(QByteArray)));
    m_pTcpDataParseThread->stop();
    m_pTcpDataParseThread->wait();
    m_pTcpDataParseThread->deleteLater();
}

void Widget::initTestRoom()
{
    //for test
    Room* pRoom = new Room();
    pRoom->setRoomIdx(0);
    pRoom->setRoomName(tr("主卧"));
    pRoom->setRoomAirSv(20);
    pRoom->setRoomFloorSv(30);
    pRoom->setRoomAirPv(19.5);
    pRoom->setRoomFloorPv(31.5);
    pRoom->setRoomTimerOff(16, 33);
    pRoom->setRoomTimerOn(16, 33);

    m_roomList.append(pRoom);
    m_pRoomSelected = pRoom;

    updateDisplay(pRoom);
}

void Widget::initDial()
{
    m_dialVal = 0;
    m_dialMutex = -1;

    m_pDialAir = new PotentioMeter();
    ui->layoutDialAirSv->addWidget(m_pDialAir);

    m_pDialFloor = new PotentioMeter();
    ui->layoutDialFloorSv->addWidget(m_pDialFloor);

    //数据增加槽函数
    QSignalMapper *pSignalMapper_valueInc = new QSignalMapper;

    connect(m_pDialAir, SIGNAL(valueInc()), \
            pSignalMapper_valueInc, SLOT(map()));
    connect(m_pDialFloor, SIGNAL(valueInc()), \
            pSignalMapper_valueInc, SLOT(map()));

    pSignalMapper_valueInc->setMapping(m_pDialAir, PARAIDX_AIRTEMP_SV);
    pSignalMapper_valueInc->setMapping(m_pDialFloor, PARAIDX_FLOORTEMP_SV);

    connect(pSignalMapper_valueInc, SIGNAL(mapped(int)), \
            this, SLOT(slotDialInc(int)));

    //数据减少槽函数
    QSignalMapper *pSignalMapper_valueDec = new QSignalMapper;

    connect(m_pDialAir, SIGNAL(valueDec()), \
            pSignalMapper_valueDec, SLOT(map()));
    connect(m_pDialFloor, SIGNAL(valueDec()), \
            pSignalMapper_valueDec, SLOT(map()));

    pSignalMapper_valueDec->setMapping(m_pDialAir, PARAIDX_AIRTEMP_SV);
    pSignalMapper_valueDec->setMapping(m_pDialFloor, PARAIDX_FLOORTEMP_SV);

    connect(pSignalMapper_valueDec, SIGNAL(mapped(int)), \
            this, SLOT(slotDialDec(int)));

    //控件点击槽函数
    QSignalMapper* pSignalMapper_sliderPressed = new QSignalMapper;

    connect(m_pDialAir, SIGNAL(sliderPressed()), \
            pSignalMapper_sliderPressed, SLOT(map()));
    connect(m_pDialFloor, SIGNAL(sliderPressed()), \
            pSignalMapper_sliderPressed, SLOT(map()));

    pSignalMapper_sliderPressed->setMapping(m_pDialAir, PARAIDX_AIRTEMP_SV);
    pSignalMapper_sliderPressed->setMapping(m_pDialFloor, PARAIDX_FLOORTEMP_SV);

    connect(pSignalMapper_sliderPressed, SIGNAL(mapped(int)), \
            this, SLOT(slotDialPressed(int)));

    //控件释放槽函数
    QSignalMapper* pSignalMapper_sliderReleased = new QSignalMapper;

    connect(m_pDialAir, SIGNAL(sliderReleased()), \
            pSignalMapper_sliderReleased, SLOT(map()));
    connect(m_pDialFloor, SIGNAL(sliderReleased()), \
            pSignalMapper_sliderReleased, SLOT(map()));

    pSignalMapper_sliderReleased->setMapping(m_pDialAir, PARAIDX_AIRTEMP_SV);
    pSignalMapper_sliderReleased->setMapping(m_pDialFloor, PARAIDX_FLOORTEMP_SV);

    connect(pSignalMapper_sliderReleased, SIGNAL(mapped(int)), \
            this, SLOT(slotDialReleased(int)));
}

void Widget::timerEvent(QTimerEvent *e)
{
    Q_UNUSED(e);
    //更新界面时间显示
    QDateTime curDateTime = QDateTime::currentDateTime();
    ui->labelDateTime->setText(curDateTime.toString(Qt::LocalDate));

    //温控器定时开关功能轮询
    QByteArray frame(4, 0);
    frame[FRAME_FIELD_LEN] = 1;
    frame[FRAME_FIELD_PARAIDX] = PARAIDX_POWER_SETUP;

    QTime curTime = QTime::currentTime();
    curTime = curTime.addMSecs(-curTime.msec());
    //curTime.setHMS(curTime.hour(), curTime.minute(), curTime.second());
    //设置当前时间的毫秒为0，上述两种方法均可以
    //因为currentTime方法获取的毫秒不一定为0，和定时开关时间比较会失败

    foreach (Room *pRoom, m_roomList)
    {
        //qDebug() << curTime << pRoom->getRoomTimerOff() << (bool)(pRoom->getRoomTimerOff() == curTime);
        if (pRoom->getRoomTimerOff() == curTime && pRoom->getRoomTimerOff().second() != 0)
        {
            frame[FRAME_FIELD_ROOMIDX] = pRoom->getRoomIdx();
            frame[FRAME_FIELD_PARAVAL] = 0;
            emit sigParaChanged(frame);
        }

        //qDebug() << curTime << pRoom->getRoomTimerOn() << (bool)(pRoom->getRoomTimerOn() == curTime);
        if (pRoom->getRoomTimerOn() == curTime && pRoom->getRoomTimerOn().second() != 0)
        {
            frame[FRAME_FIELD_ROOMIDX] = pRoom->getRoomIdx();
            frame[FRAME_FIELD_PARAVAL] = 1;
            emit sigParaChanged(frame);
        }
    }
    if(loged)
    {

    }
}

//slots
void Widget::slotNavibarSelected(int index)
{
    m_pRoomSelected = m_roomList[index];
    updateDisplay(m_pRoomSelected);
    //qDebug() << "slotNavibarSelected" << m_pRoomSelected->getRoomName();
}

void Widget::slotDialInc(int paraIdx)
{
    //qDebug() << paraIdx << m_dialVal;
    if (paraIdx == m_dialMutex)
    {
        if (paraIdx == PARAIDX_AIRTEMP_SV)
        {
            if (m_dialVal < 50)
            {
                QString str;
                // QTextStream(&str) << 1.2f;
                m_dialVal++;
                str = str.sprintf("%d", m_dialVal);
                ui->labelAirSv->setText(str);//display(m_dialVal);
            }
        }
        else if (paraIdx == PARAIDX_FLOORTEMP_SV)
        {
            if (m_dialVal < 70)
            {
                QString str;
                m_dialVal++;
                str = str.sprintf("%d", m_dialVal);
                ui->labelFloorSv->setText(str);
            }
        }
    }
}

void Widget::slotDialDec(int paraIdx)
{
    if (paraIdx == m_dialMutex)
    {
        if (paraIdx == PARAIDX_AIRTEMP_SV)
        {
            if (m_dialVal > 10)
            {
                QString str;
                m_dialVal--;
                str = str.sprintf("%d", m_dialVal);

                ui->labelAirSv->setText(str);
            }
        }
        else if (paraIdx == PARAIDX_FLOORTEMP_SV)
        {
            if (m_dialVal > 20)
            {
                QString str;
                m_dialVal--;
                str = str.sprintf("%d", m_dialVal);

                ui->labelFloorSv->setText(str);
            }
        }
    }
 }

void Widget::slotDialPressed(int paraIdx)
{
    /*
     * using m_dialMutex as mutex variable
     * only when a complete procedure(pressed->inc/dec->released) is finshed,
     * m_dialMutex will be assigned a new value.
     * */
    /*
     * dialPressed
     * using paraIdx to auto-lock m_dialMutex
     * do not refresh m_dialVal if a different dial is pressed
     * */
    /* dialInc/dialDec
     * only response if paraIdx == m_dialVal
     * */
    /* dialRelease
     * reset m_dialVal to -1, allow to response a new dialPressed
     * */
    if (m_pRoomSelected != NULL && m_dialMutex == -1)
    {
        m_dialMutex = paraIdx;
        if (paraIdx == PARAIDX_AIRTEMP_SV)
            m_dialVal = m_pRoomSelected->getRoomAirSv();
        else if (paraIdx == PARAIDX_FLOORTEMP_SV)
            m_dialVal = m_pRoomSelected->getRoomFloorSv();
    }
    //qDebug() << m_dialVal;
}

void Widget::slotDialReleased(int paraIdx)
{
    if (m_pRoomSelected != NULL)
    {
        if (paraIdx == PARAIDX_AIRTEMP_SV)
            m_pRoomSelected->setRoomAirSv(m_dialVal);
        else if (paraIdx == PARAIDX_FLOORTEMP_SV)
            m_pRoomSelected->setRoomFloorSv(m_dialVal);

        QByteArray frame(4, 0);
        frame[FRAME_FIELD_LEN] = 1;
        frame[FRAME_FIELD_ROOMIDX] = m_pRoomSelected->getRoomIdx();
        frame[FRAME_FIELD_PARAIDX] = paraIdx;
        frame[FRAME_FIELD_PARAVAL] = m_dialVal;
        //qDebug() << "dialReleased" << frame.toHex();
        emit sigParaChanged(frame);

        m_dialMutex = -1;
    }
}

void Widget::slotParaProcess(QByteArray frame)
{
    uint paraVal  = 0;
    unsigned char roomIdx = (byte)(frame[FRAME_FIELD_ROOMIDX]);
    unsigned char paraIdx = (byte)(frame[FRAME_FIELD_PARAIDX]);

    if(paraIdx == PARAIDX_AIRTEMP_SV ||  paraIdx == PARAIDX_FLOORTEMP_SV )
    {
        qDebug() << "tiaoxiaopeng" << paraVal;
    }
    foreach (Room *pRoom, m_roomList)
    {
        if (pRoom->getRoomIdx() == roomIdx)
        {
            switch (frame[FRAME_FIELD_LEN])
            {
            case 1:
                paraVal = (byte)frame[FRAME_FIELD_PARAVAL];
                break;
            case 2:
                paraVal = (byte)(frame[FRAME_FIELD_PARAVAL]);
                paraVal <<= 8;
                // for debugging
               /* if(paraIdx == 3 || paraIdx == 4)
                {
                    qDebug() << roomIdx  << ":" << paraIdx << ":" <<"paraValHIGH:" << paraVal;
                }*/
                paraVal |= (byte)(frame[FRAME_FIELD_PARAVAL + 1]);
                // for debugging
               /* if(paraIdx == 3 || paraIdx == 4)
                {
                    qDebug() << roomIdx  << ":" << paraIdx << ":" <<"paraValLow:" <<  (byte)(frame[FRAME_FIELD_PARAVAL + 1]);//paraVal;
                }*/

                break;
            default:;
            }

            // for debugging
            /*if(paraIdx == 3 || paraIdx == 4)
            {
                qDebug() << roomIdx  << ":" << paraIdx << ":" <<"paraValfloat:" << paraVal;
            }*/
            switch (paraIdx)
            {
            case PARAIDX_AIRTEMP_SV:
                pRoom->setRoomAirSv(paraVal);
               //
                break;
            case PARAIDX_FLOORTEMP_SV:
                pRoom->setRoomFloorSv(paraVal);
               // qDebug() << "taoxiaopeng" << paraVal;
                break;
            case PARAIDX_AIRTEMP_PV:
                pRoom->setRoomAirPv(paraVal / 10.0f);
                break;
            case PARAIDX_FLOORTEMP_PV:
                pRoom->setRoomFloorPv(paraVal / 10.0f);
                break;
            case PARAIDX_SLEEP_SETUP:
                break;
            case PARAIDX_MODE_SETUP:
                pRoom->setRoomMode(paraVal);
                break;
            case PARAIDX_TIMER_ON:
                pRoom->setRoomTimerOnEnabled(frame[FRAME_FIELD_PARAVAL]);
                if (frame[FRAME_FIELD_PARAVAL])
                {
                    QTime time(frame[FRAME_FIELD_PARAVAL + 1], frame[FRAME_FIELD_PARAVAL + 2], 1);
                    if (time.isValid())
                    {
                        pRoom->setRoomTimerOn(time);
                    }
                }
                break;
            case PARAIDX_TIMER_OFF:
                pRoom->setRoomTimerOffEnabled(frame[FRAME_FIELD_PARAVAL]);
                if (frame[FRAME_FIELD_PARAVAL])
                {
                    QTime time(frame[FRAME_FIELD_PARAVAL + 1], frame[FRAME_FIELD_PARAVAL + 2], 1);
                    if (time.isValid())
                    {
                        pRoom->setRoomTimerOff(time);
                    }
                }
                break;
            case PARAIDX_POWER_SETUP:
                pRoom->setRoomPwr(paraVal);
                break;
            }
            break;
        }
    }

    //刷新当前显示 如果不是当前房间的则不显示

    if(paraIdx == PARAIDX_INIT_COOR)// 协调器发来的网络ID号
    {
       paraVal = (byte)(frame[FRAME_FIELD_PARAVAL]);
       ui->pushButtonsSetPanId->setText(QString::number((byte)(paraVal)));
    }

    if (m_pRoomSelected != NULL && m_pRoomSelected->getRoomIdx() == roomIdx)
    {
        QString str;
        switch(paraIdx)
        {
        case PARAIDX_AIRTEMP_SV:
            if(paraVal <= 100)
            {
                str.sprintf("%d",paraVal);
                ui->labelAirSv->setText(str);
            }
            else
            {
                str = "Err!";
                ui->labelAirSv->setText(str);
            }
            break;
        case PARAIDX_FLOORTEMP_SV:
            if(paraVal <= 100)
            {
                str.sprintf("%d",paraVal);
                ui->labelFloorSv->setText(str);
            }
            else
            {
                str = "Err!";
                ui->labelFloorSv->setText(str);
            }
            break;
        case PARAIDX_AIRTEMP_PV:
            if(paraVal <= 1000)
            {
                str.sprintf("%.1f",paraVal / 10.0f);
                ui->labelAirPv->setText(str);
            }
            else
            {
                str = "Err!";
                ui->labelAirPv->setText(str);
            }
            break;
        case PARAIDX_FLOORTEMP_PV:
            if(paraVal <= 1000)
            {
                str.sprintf("%.1f",paraVal / 10.0f);
                ui->labelFloorPv->setText(str);
            }
            else
            {
                str = "Err!";
                ui->labelFloorPv->setText(str);
            }
            break;
        case PARAIDX_SLEEP_SETUP:
            break;
        case PARAIDX_MODE_SETUP:
             ui->comboBoxMode->setCurrentIndex(paraVal);
             break;
        case PARAIDX_TIMER_ON:
            paraVal = frame[FRAME_FIELD_PARAVAL];
            ui->pushButtonTimerOnSwitch->setChecked(paraVal);
            if (paraVal)
            {
                QTime time(frame[FRAME_FIELD_PARAVAL + 1], frame[FRAME_FIELD_PARAVAL + 2]);
                if (time.isValid())
                {
                    ui->pushButtonSetTimerOn->setText(time.toString("hh:mm"));
                    // ui->timeEditTimerOn->setTime(time);
                }
            }
            break;
        case PARAIDX_TIMER_OFF:
            paraVal = frame[FRAME_FIELD_PARAVAL];
            ui->pushButtonTimerOffSwitch->setChecked(paraVal);
            if (paraVal)
            {
                QTime time(frame[FRAME_FIELD_PARAVAL + 1], frame[FRAME_FIELD_PARAVAL + 2]);
                if (time.isValid())
                {
                   // ui->timeEditTimerOff->setTime(time);

                }
            }
            break;
        case PARAIDX_POWER_SETUP:
            ui->pushButtonPwr->setChecked(paraVal);
            break;
        }
    }

    /*if (m_mapParaIdx.find(paraIdx) != m_mapParaIdx.end())
    {
        qDebug() << "Widget::slotParaProcess" \
                 << QString("Room#%1").arg(roomIdx) \
                 << m_mapParaIdx.find(paraIdx).value() \
                 << frame.toHex();
    }
    else
    {
        qDebug() << "CMD1 error unkonwn paraIdx!";
    }*/
}

void Widget::on_pushButtonPwr_clicked(bool checked)
{
    if (m_pRoomSelected)
    {
        //qDebug() << "on_pushButtonPwr_toggled " << checked;
        QByteArray frame(4, 0);
        frame[FRAME_FIELD_LEN] = 1;
        frame[FRAME_FIELD_ROOMIDX] = m_pRoomSelected->getRoomIdx();
        frame[FRAME_FIELD_PARAIDX] = PARAIDX_POWER_SETUP;
        frame[FRAME_FIELD_PARAVAL] = checked;
        //qDebug() << "dialReleased" << frame.toHex();
        emit sigParaChanged(frame);
    }
}

void Widget::on_pushButtonTimerOnSwitch_toggled(bool checked)
{
    if (m_pRoomSelected)
    {
        m_pRoomSelected->setRoomTimerOnEnabled(checked);
        ui->pushButtonSetTimerOn->setText(m_pRoomSelected->getRoomTimerOn().toString("hh:mm"));
    }
}

void Widget::on_pushButtonTimerOffSwitch_toggled(bool checked)
{
    if (m_pRoomSelected)
    {

        m_pRoomSelected->setRoomTimerOffEnabled(checked);
       // ui->timeEditTimerOff->setTime(m_pRoomSelected->getRoomTimerOff());
        ui->pushButtonSetTimerOff->setText(m_pRoomSelected->getRoomTimerOff().toString("hh:mm"));
    }

}

void Widget::on_comboBoxMode_activated(int index)
{
    /*const QString strMode[] = {tr("手动"), tr("智能"), tr("睡眠")};
    qDebug() << strMode[index];*/
    if (m_pRoomSelected != NULL)
    {
        QByteArray frame(4,0);
        frame[FRAME_FIELD_LEN] = 1;
        frame[FRAME_FIELD_ROOMIDX] = m_pRoomSelected->getRoomIdx();
        frame[FRAME_FIELD_PARAIDX] = PARAIDX_MODE_SETUP;
        m_pRoomSelected->setRoomMode(index);
        switch (index)
        {
        case ROOM_MODE_MANUAL:  
           // emit sigRoomModeChanged(m_pRoomSelected, false);
            frame[FRAME_FIELD_PARAVAL] = 0x00;
            emit sigParaChanged(frame);
            break;
        case ROOM_MODE_SCHEDULE:
           // emit sigRoomModeChanged(m_pRoomSelected, true);
            frame[FRAME_FIELD_PARAVAL] = 0x01;
            emit sigParaChanged(frame);
            break;
        case ROOM_MODE_SLEEP:
            break;
        }
    }
}

void Widget::slotScheduleError(Room *pRoom, int error)
{
    Q_UNUSED(pRoom);

    switch (error)
    {
    case SCHERR_INIT_NOACK:
        m_pRoomSelected->setRoomMode(ROOM_MODE_MANUAL);
        ui->comboBoxMode->setCurrentIndex(ROOM_MODE_MANUAL);
        break;
    case SCHERR_DEINIT_NOACK:
        m_pRoomSelected->setRoomMode(ROOM_MODE_SCHEDULE);
        ui->comboBoxMode->setCurrentIndex(ROOM_MODE_SCHEDULE);
        break;
    case SCHERR_TOKEN_NOACK:
        break;
    }
    QMessageBox::warning(NULL, tr("提示"), tr("通讯故障，切换模式失败！"));
}

void Widget::on_pushButtonSetTimerOn_clicked()
{

    if (m_pRoomSelected)
    {
        QTime timerOn = m_pRoomSelected->getRoomTimerOn();
        SetTimeDlg dlg(timerOn.hour(), timerOn.minute());
        if (dlg.exec() == QDialog::Accepted)
        {
             //之前逻辑有问题 && 改成 ||
            if (m_pRoomSelected->getRoomTimerOff().hour() != dlg.getHour() \
                   || m_pRoomSelected->getRoomTimerOff().minute() != dlg.getMinute())
            {
                timerOn.setHMS(dlg.getHour(), dlg.getMinute(), timerOn.second());

                m_pRoomSelected->setRoomTimerOn(timerOn);
                ui->pushButtonSetTimerOn->setText(timerOn.toString("hh:mm"));
                // ui->timeEditTimerOn->setTime(timerOn);
            }
            else
            {
                QMessageBox::warning(NULL, tr("错误"), tr("定时开机时间与关机时间相同，请重新设置！"));
            }
        }
    }

}

void Widget::on_pushButtonSetTimerOff_clicked()
{

    if (m_pRoomSelected)
    {
        QTime timerOff = m_pRoomSelected->getRoomTimerOff();
        SetTimeDlg dlg(timerOff.hour(), timerOff.minute());
        if (dlg.exec() == QDialog::Accepted)
        {
            //之前逻辑有问题
            if (m_pRoomSelected->getRoomTimerOn().hour() != dlg.getHour() \
                   || m_pRoomSelected->getRoomTimerOn().minute() != dlg.getMinute())
            {
                timerOff.setHMS(dlg.getHour(), dlg.getMinute(), timerOff.second());
                m_pRoomSelected->setRoomTimerOff(timerOff);
                ui->pushButtonSetTimerOff->setText(timerOff.toString("hh:mm"));

                // ui->timeEditTimerOff->setTime(timerOff);
            }
            else
            {
                QMessageBox::warning(NULL, tr("错误"), tr("定时开机时间与关机时间相同，请重新设置！"));
            }
        }
    }
}

// PanId设置槽函数
void Widget::on_pushButtonsSetPanId_clicked()
{
   NwkMgrDlg dlg;
   dlg.setWindowModality(Qt::WindowModal);
   ui->pushButtonsSetPanId->setEnabled(false);
   if(dlg.exec() == QDialog::Accepted)
   {
       QString str(tr("请确认修改网络号为：%1").arg(QString::number(dlg.get_panid())));
       int ret = QMessageBox::information(0,tr("提醒"),str,QMessageBox::Yes | QMessageBox::No);
       if(ret == QMessageBox::Yes)
       {
           QByteArray frame(4,0);
           frame[FRAME_FIELD_LEN] = 1;
           frame[FRAME_FIELD_ROOMIDX] = m_pRoomSelected->getRoomIdx();
           frame[FRAME_FIELD_PARAIDX] = PARAIDX_PANID_SETUP;
           frame[FRAME_FIELD_PARAVAL] = dlg.get_panid();
           emit sigParaChanged(frame);
           ui->pushButtonsSetPanId->setText(QString::number(dlg.get_panid()));
       }
       ui->pushButtonsSetPanId->setEnabled(true);
   }
   else
   {
       ui->pushButtonsSetPanId->setEnabled(true);
   }
}

void Widget::setButtonsEnabled(bool isLoged,bool isLocked)
{
     if(isLoged && !isLocked)
     {
        ui->layoutNavibar->setEnabled(true);
        ui->pushButtonLeft->setEnabled(true);
        ui->pushButtonPwr->setEnabled(true);
        ui->pushButtonRight->setEnabled(true);
        ui->pushButtonSetTimerOff->setEnabled(true);
        ui->pushButtonSetTimerOn->setEnabled(true);
        ui->pushButtonsSetPanId->setEnabled(true);
        ui->pushButtonTimerOffSwitch->setEnabled(true);
        ui->pushButtonTimerOnSwitch->setEnabled(true);
        ui->layoutDialAirSv->setEnabled(true);
        ui->layoutDialFloorSv->setEnabled(true);
        ui->comboBoxMode->setEnabled(true);
     }
     else if(isLoged && isLocked)
     {
         ui->layoutNavibar->setEnabled(true);
         ui->pushButtonLeft->setEnabled(true);
         ui->pushButtonPwr->setEnabled(false);
         ui->pushButtonRight->setEnabled(true);
         ui->pushButtonSetTimerOff->setEnabled(false);
         ui->pushButtonSetTimerOn->setEnabled(false);
         ui->pushButtonsSetPanId->setEnabled(false);
         ui->pushButtonTimerOffSwitch->setEnabled(false);
         ui->pushButtonTimerOnSwitch->setEnabled(false);
         ui->layoutDialAirSv->setEnabled(false);
         ui->layoutDialFloorSv->setEnabled(false);
         ui->comboBoxMode->setEnabled(false);
     }
     else
     {
         ui->layoutNavibar->setEnabled(false);
         ui->pushButtonLeft->setEnabled(false);
         ui->pushButtonPwr->setEnabled(false);
         ui->pushButtonRight->setEnabled(false);
         ui->pushButtonSetTimerOff->setEnabled(false);
         ui->pushButtonSetTimerOn->setEnabled(false);
         ui->pushButtonsSetPanId->setEnabled(false);
         ui->pushButtonTimerOffSwitch->setEnabled(false);
         ui->pushButtonTimerOnSwitch->setEnabled(false);
         ui->layoutDialAirSv->setEnabled(false);
         ui->layoutDialFloorSv->setEnabled(false);
         ui->comboBoxMode->setEnabled(false);
     }

}

void Widget::on_pushButtonLogin_clicked()
{
    // if the user list isn't empty,than we can login
    if(!m_userList.isEmpty())
    {
        if(loged == false)// 没有登录时弹出窗口提醒登录
        {
            Login  login_dlg(m_userList);
            // 设置成非模式，可以在其他窗口干事情
            login_dlg.setWindowModality(Qt::WindowModal);
            //必须把登录按钮屏蔽，要不然如果可以按，会弹出好多个重复窗口
            ui->pushButtonLogin->setEnabled(FALSE);
            if(login_dlg.exec() == QDialog::Accepted)
            {
                user *temp = login_dlg.get_user_loged();
                loged = true;
                if(temp->get_user_priority() == 3)
                {
                    locked = true;
                }
                else
                {
                    locked = false;
                }
                setButtonsEnabled(loged,locked);
                //然后还要记得释放
                ui->pushButtonLogin->setEnabled(true);
                ui->pushButtonLogin->setText(tr("退出"));
                m_user_loged.set_user_name(temp->get_user_name());
                m_user_loged.set_user_code(temp->get_user_code());
                m_user_loged.set_user_priority(temp->get_user_priority());
               // m_user_loged.set_user_id(temp->get_user_id());
            }
            else
            {
                loged = false;
                setButtonsEnabled(loged,locked);
                ui->pushButtonLogin->setEnabled(true);
            }
        }
        else// 登录时弹出窗口提询问是否退出点登录
        {
           int ret = QMessageBox::information(0,tr("提醒"),tr("确认退出？"),QMessageBox::Yes | QMessageBox::No);
           if(ret == QMessageBox::Yes)
       {
           loged = false;
           ui->pushButtonLogin->setText(tr("登录"));
           setButtonsEnabled(loged,locked);

       }
        }
    }
    else// we need to creat the adminstrator
    {
        CreatAdminstratorDlg dlg;
        if(dlg.exec() == QDialog::Accepted)
        {
           user * puser = dlg.get_admin();
           m_userList.append(puser);
           QMessageBox::information(0,tr("提醒"),tr("添加管理员成功，请登录！"));
           addDatabase(puser);
        }

    }
}

void Widget::ReloadRoomDatabase()
{
    QSqlDatabase db;
    if (QSqlDatabase::contains("qt_sql_default_connection"))
    {
       db = QSqlDatabase::database("qt_sql_default_connection");
    }
    else
    {
        db = QSqlDatabase::addDatabase("QSQLITE");
    }

    db.setDatabaseName("room.db");
    if (!db.open())
    {
        qDebug() << db.lastError();
        //initTestRoom();
    }
    else
    {
        QSqlQuery sql;
        sql.prepare("SELECT * FROM ROOM ORDER BY Room_Id");//
        if (!sql.exec())
        {
            qDebug() << sql.lastError();
            //initTestRoom();
        }
        else
        {
            while (sql.next())
            {
                Room* pRoom = new Room();
                pRoom->setRoomIdx(sql.value(COL_ROOM_ID).toInt());
                pRoom->setRoomName(sql.value(COL_ROOM_NAME).toString());
                pRoom->setRoomAirSv(sql.value(COL_ROOM_AIRSV).toInt());
                pRoom->setRoomFloorSv(sql.value(COL_ROOM_FLOORSV).toInt());
                pRoom->setRoomAirPv(sql.value(COL_ROOM_AIRPV).toFloat());
                pRoom->setRoomFloorPv(sql.value(COL_ROOM_FLOORPV).toFloat());
                pRoom->setRoomTimerOn(sql.value(COL_ROOM_TIMERON).toTime());
                pRoom->setRoomTimerOff(sql.value(COL_ROOM_TIMEROFF).toTime());
                pRoom->setRoomMode(sql.value(COL_ROOM_MODE).toInt());
                pRoom->setRoomPrio(sql.value(COL_ROOM_PRIORITY).toInt());
                //qDebug() << sql.value(COL_ROOM_TIMEROFF).toString();
                //qDebug() << sql.value(COL_ROOM_TIMEROFF).toTime();
                //qDebug() << pRoom->getRoomTimerOff();
                m_roomList.append(pRoom);
            }
        }

        db.close();
    }


}

void Widget::ReloadUserDatabase()
{
    QSqlDatabase db;
    if (QSqlDatabase::contains("qt_sql_default_connection"))
    {
       db = QSqlDatabase::database("qt_sql_default_connection");
    }
    else
    {
        db = QSqlDatabase::addDatabase("QSQLITE");
    }

    db.setDatabaseName("room.db");
    if (!db.open())
    {
        qDebug() << db.lastError();
        //initTestRoom();
    }
    else
    {
        QSqlQuery sql;
        sql.prepare("SELECT * FROM USER  ORDER BY User_Priority");// 读出一个根据User_Id 排序的结果
        if(sql.exec())
        {
          //  int id = 1;
            while(sql.next())
            {
               user * puser = new user();
               puser->set_user_name(sql.value(COL_USER_NAME).toString());
               puser->set_user_code(sql.value(COL_USER_CODE).toString());
               puser->set_user_priority(sql.value(COL_USER_PRIORITY).toInt());
              //  puser->set_user_id(id);
              // id++;
               m_userList.append(puser);//加入链表
               qDebug() << sql.value(COL_USER_NAME).toString();
               qDebug() << sql.value(COL_USER_CODE).toString();
               qDebug() << sql.value(COL_USER_PRIORITY).toInt();

             }

        }
        db.close();
    }
}

void Widget::on_pushButtonSeting_clicked()
{
    if(loged)
    {
       user * pUser = &m_user_loged;
       UserAndRoomMgrDlg dlg(m_roomList,m_userList,pUser);
       // 设置成非模式，可以在其他窗口干事情
       dlg.setWindowModality(Qt::WindowModal);
       // 屏蔽设置按钮
       ui->pushButtonSeting->setEnabled(FALSE);
       if(dlg.exec()== QDialog::Accepted)
       {
           if (!(dlg.getRoomAdd().isEmpty()))
           {
               foreach (Room *pRoom, dlg.getRoomAdd())
               {
                   addDatabase(pRoom);
                   //m_pNavibar->AddItem(pRoom->getRoomName(), pRoom->getRoomIdx());
                   qDebug() << pRoom->getRoomName();
                   m_roomList.append(pRoom);
               }
           }

           if (!(dlg.getRoomDelete().isEmpty()))
           {
               foreach (Room *pRoom, dlg.getRoomDelete())
               {
                   deleteDatabase(pRoom->getRoomIdx());
                  // m_pNavibar->RemoveItem(pRoom->getRoomIdx());
                   m_roomList.removeOne(pRoom);
                   delete(pRoom);//一定要销毁对象，否则内存泄漏！！！bug修复！！！
                   if (m_pRoomSelected == pRoom)
                   {
                       m_pRoomSelected = NULL;
                   }
               }
           }

           if (!(dlg.getRoomUpdate().isEmpty()))
           {
               foreach (Room *pRoom, dlg.getRoomUpdate())
               {
                   m_pNavibar->UpdateItem(pRoom->getRoomIdx(), pRoom->getRoomName());
               }
           }

           if (m_pRoomSelected == NULL)
           {
               updateDisplay(NULL);
           }

           if(!(dlg.getRoomAdd().isEmpty()) || !(dlg.getRoomDelete().isEmpty()))
               // 解决不按房间序号添加房间时导航栏不按房间序号显示房间的问题
               //解决删除房间时导航栏失效的问题
           {
               updateRoomDatabase();//先把数据更新到数据库中
               foreach(Room * pRoom,m_roomList)// 清空房间列表
               {
                   m_roomList.removeOne(pRoom);
                   delete pRoom;
               }
               ReloadRoomDatabase(); // 重新载入数据库数据
               delete m_pNavibar;//删除旧的导航栏
               initNavibar();//新建导航栏
           }





          if(!dlg.get_useradded().isEmpty())
           {
             foreach(user *puser,dlg.get_useradded())
             {
                addDatabase(puser);
                m_userList.append(puser);
             }
           }

           if(!dlg.get_userdeleted().isEmpty())
           {
               foreach(user *puser,dlg.get_userdeleted())
               {
                  deleteDatabase(puser);
                  m_userList.removeOne(puser);
                  delete(puser);//销毁对象，避免内存泄漏
               }
           }

           QMessageBox::information(0,tr("提醒"),tr("设置成功！"));
           ui->pushButtonSeting->setEnabled(true);
       }
       else
       {
           ui->pushButtonSeting->setEnabled(true);
       }

   }
    else
    {
        QMessageBox::information(0,tr("提醒"),tr("请先登录！"));

    }
}

