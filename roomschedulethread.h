#ifndef ROOMSCHEDULETHREAD_H
#define ROOMSCHEDULETHREAD_H

#include <QThread>
#include <QList>
#include <QTimer>
#include <QVector>
#include "room.h"

class RoomScheduleThread : public QThread
{
    Q_OBJECT
public:
    explicit RoomScheduleThread(QObject *parent = 0);
    void externStartTest(Room *pRoom, bool start);

protected:
    Room* findNextTokenHolder();
    bool updateScheduleList();
    void sendCmdModeSetup(int roomIdx, int roomMode);
    void sendCmdToken(int roomIdx);
    int calcScheduleScore(Room *pRoom);

signals:
    void sigSendCmd(QByteArray frame);
    void sigScheduleError(Room *pRoom, int error);

public slots:
    void slotRoomModeChanged(Room *pRoom, bool enabled);
    void slotAckReceived(QByteArray frame);
    void slotTokenTimeout();
    void slotInitTimeout();
    void slotDeinitTimeout();

private:
    typedef enum
    {
        IDLE_STATE = 0, \
        TOKEN_STATE, \
        ACK_STATE, \
        UPDATE_STATE
    }scheduleState_t;
    scheduleState_t m_schState;
    Room *m_pTokenHolder;
    QVector<bool> m_waitingFlag;
    QList<Room*> m_scheduleList;
    QList<Room*> m_addList, m_addReqList;
    QList<Room*> m_delList, m_delReqList;
    QTimer m_tokenTimeout, m_initTimeout, m_deinitTimeout;
    bool m_flagTimeout;
};

#endif // ROOMSCHEDULETHREAD_H
