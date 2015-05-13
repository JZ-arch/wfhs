#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QMap>

#include "room.h"
#include "user.h"

#include "datapipethread.h"
#include "dataparsethread.h"
#include "roomschedulethread.h"

#include "3rd-party/potentiometer/potentiometer.h"
#include "3rd-party/navigationbar/navigationbarview.h"

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

protected:
    void initMapParaIdx();
    void initTestRoom();

    void initComboBox();
    void initTemperatureNumber();
    void initDial(); 
    void initToggleButton();
    void initDateTime();
    void initNavibar();

    void initThread();
    void deinitThread();

    void updateDisplay(Room *pRoom);

    void initDatabase();
    bool addDatabase(Room *pRoom);
    bool deleteDatabase(int roomIdx);
    void updateRoomDatabase();
    void ReloadRoomDatabase();
    void ReloadUserDatabase();

    void initInputMethod();

    bool addDatabase(user* puser);
    bool deleteDatabase(user* puser);

    void setButtonsEnabled(bool isLoged,bool isLocked);

    QList<Room*> get_m_roomList() const {return m_roomList;}


private:
    Ui::Widget *ui;

    QList<Room*> m_roomList;
    QList<user*> m_userList;
    Room *m_pRoomSelected;

    DataPipeThread *m_pDataPipeThread;
    DataParseThread *m_pComDataParseThread, *m_pTcpDataParseThread;
    RoomScheduleThread *m_pRoomScheduleThread;

    NavigationBarView *m_pNavibar;
    PotentioMeter *m_pDialAir;
    PotentioMeter *m_pDialFloor;
    int m_dialVal;
    int m_dialMutex;

    int m_secTimerId;
    QMap<int, QString> m_mapParaIdx;
    bool locked;
    bool loged;
    user m_user_loged;
    int panid;

private slots:
    void slotDialPressed(int);

    void slotDialReleased(int);

    void slotDialInc(int);

    void slotDialDec(int);

    void slotNavibarSelected(int index);

    void slotParaProcess(QByteArray frame);

    void slotScheduleError(Room *pRoom, int error);

    void on_pushButtonPwr_clicked(bool checked);

    void on_pushButtonSeting_clicked();

    void on_pushButtonTimerOnSwitch_toggled(bool checked);

    void on_pushButtonTimerOffSwitch_toggled(bool checked);

    void on_comboBoxMode_activated(int index);

    void on_pushButtonSetTimerOn_clicked();

    void on_pushButtonSetTimerOff_clicked();

    void on_pushButtonsSetPanId_clicked();

    void on_pushButtonLogin_clicked();

protected:
    void timerEvent(QTimerEvent *e);

signals:
    void sigParaChanged(QByteArray frame);
    void sigRoomModeChanged(Room *pRoom, bool enabled);
};

#endif // WIDGET_H
