#include "room.h"
#include "global.h"

Room::Room(QObject *parent) :
    QObject(parent)
{
    m_roomPwr = false;
    m_roomIdx = 0;
    m_roomName = "";
    m_roomAirPv = 0;
    m_roomAirSv = 0;
    m_roomFloorPv = 0;
    m_roomFloorSv = 0;
    m_roomTimerOff = QTime::fromString("00:00","hh':'mm");
    m_roomTimerOn  = QTime::fromString("00:00","hh':'mm");
}

QString Room::getRoomPrioStr()
{
    QString strPrio;
    switch (m_roomPrio)
    {
    case ROOM_PRIO_HIGH:
        strPrio = tr("高");
        break;
    case ROOM_PRIO_MID:
        strPrio = tr("中");
        break;
    case ROOM_PRIO_LOW:
        strPrio = tr("低");
        break;
    }
    return strPrio;
}

void Room::setRoomMode(int roomMode)
{
    m_roomMode = roomMode;
}

void Room::setRoomPrio(int roomPrio)
{
    m_roomPrio = roomPrio;
}

void Room::setRoomIdx(int roomIdx)
{
    m_roomIdx = roomIdx;
}
void Room::setRoomName(QString roomName)
{
    m_roomName = roomName;
}
void Room::setRoomName(int roomType)
{
    const QString roomName[3] = {tr("客厅"), tr("卧室"), tr("餐厅")};
    if(roomType > 2)
        roomType = 2;
    m_roomName = roomName[roomType];
}
void Room::setRoomAirSv(int roomAirSv)
{
    m_roomAirSv = roomAirSv;
}

void Room::setRoomAirPv(float roomAirPv)
{
    m_roomAirPv = roomAirPv;
}

void Room::setRoomFloorSv(int roomFloorSv)
{
    m_roomFloorSv = roomFloorSv;
}

void Room::setRoomFloorPv(float roomFloorPv)
{
    m_roomFloorPv = roomFloorPv;
}

QTime Room::getRoomTimerOn()
{
    QTime time;
    m_timerOnLock.lockForRead();
    time = m_roomTimerOn;
    m_timerOnLock.unlock();
    return time;
}

QTime Room::getRoomTimerOff()
{
    QTime time;
    m_timerOffLock.lockForRead();
    time = m_roomTimerOff;
    m_timerOffLock.unlock();
    return time;
}

void Room::setRoomTimerOff(int hour, int minute)
{
    m_timerOffLock.lockForWrite();
    m_roomTimerOff.setHMS(hour, minute, m_roomTimerOff.second());
    m_timerOffLock.unlock();
}

void Room::setRoomTimerOff(QTime time)
{
    m_timerOffLock.lockForWrite();
    m_roomTimerOff = time;
    m_timerOffLock.unlock();
}

void Room::setRoomTimerOn(int hour, int minute)
{
    m_timerOnLock.lockForWrite();
    m_roomTimerOn.setHMS(hour, minute, m_roomTimerOn.second());
    m_timerOnLock.unlock();
}

void Room::setRoomTimerOn(QTime time)
{
    m_timerOnLock.lockForWrite();
    m_roomTimerOn = time;
    m_timerOnLock.unlock();
}

void Room::setRoomTimerOnEnabled(bool enabled)
{
    //定时分辨到分钟，秒位作为使能位，秒为1使能，为0禁止。
    m_timerOnLock.lockForWrite();
    m_roomTimerOn.setHMS(m_roomTimerOn.hour(), m_roomTimerOn.minute(), enabled ? 1 : 0);
    m_timerOnLock.unlock();
}

void Room::setRoomTimerOffEnabled(bool enabled)
{
    //定时分辨到分钟，秒位作为定时使能位，秒为1使能，为0禁止。
    m_timerOnLock.lockForWrite();
    m_roomTimerOff.setHMS(m_roomTimerOff.hour(), m_roomTimerOff.minute(), enabled ? 1 : 0);
    m_timerOnLock.unlock();
}

void Room::setRoomPwr(bool roomPwr)
{
    m_roomPwr = roomPwr;
}
