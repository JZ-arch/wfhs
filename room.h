#ifndef ROOM_H
#define ROOM_H

#include <QObject>
#include <QTime>
#include <QReadWriteLock>

class Room : public QObject
{
    Q_OBJECT

public:
    explicit Room(QObject *parent = 0);

    bool getRoomPwr() const { return m_roomPwr; }

    int getRoomMode() const { return m_roomMode; }

    int getRoomPrio() const { return m_roomPrio; }
    QString getRoomPrioStr();

    int getRoomIdx() const { return m_roomIdx; }
    QString getRoomName() const { return m_roomName; }

    int getRoomAirSv() const { return m_roomAirSv; }
    float getRoomAirPv() const { return m_roomAirPv; }

    int getRoomFloorSv() const { return m_roomFloorSv; }
    float getRoomFloorPv() const { return m_roomFloorPv;}

    QTime getRoomTimerOn();
    QTime getRoomTimerOff();

    void setRoomPwr(bool);
    void setRoomMode(int);
    void setRoomPrio(int);
    void setRoomIdx(int);

    void setRoomName(QString);
    void setRoomName(int);

    void setRoomAirSv(int);
    void setRoomAirPv(float);

    void setRoomFloorSv(int);
    void setRoomFloorPv(float);

    void setRoomTimerOff(int, int);
    void setRoomTimerOn(int, int);

    void setRoomTimerOff(QTime);
    void setRoomTimerOn(QTime);

    void setRoomTimerOnEnabled(bool);
    void setRoomTimerOffEnabled(bool);

private:
    bool m_roomPwr;
    int m_roomIdx, m_roomPrio, m_roomMode;
    QString m_roomName;
    int m_roomAirSv, m_roomFloorSv;
    float m_roomAirPv, m_roomFloorPv;
    QTime m_roomTimerOff, m_roomTimerOn;
    QReadWriteLock m_timerOnLock, m_timerOffLock;
};

#endif // ROOM_H
