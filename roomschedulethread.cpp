#include "roomschedulethread.h"
#include "global.h"
#include <QDebug>

RoomScheduleThread::RoomScheduleThread(QObject *parent) :
    QThread(parent), m_schState(IDLE_STATE), m_flagTimeout(false)
{
    start();
    m_tokenTimeout.setInterval(10000);
    m_initTimeout.setInterval(3000);
    m_deinitTimeout.setInterval(13000);
    connect(&m_tokenTimeout, SIGNAL(timeout()), this, SLOT(slotTokenTimeout()));
    connect(&m_initTimeout, SIGNAL(timeout()), this, SLOT(slotInitTimeout()));
    connect(&m_deinitTimeout, SIGNAL(timeout()), this, SLOT(slotDeinitTimeout()));
}

void RoomScheduleThread::slotRoomModeChanged(Room *pRoom, bool enabled)
{
    if (enabled)
    {
        m_addReqList.append(pRoom);
        sendCmdModeSetup(pRoom->getRoomIdx(), ROOM_MODE_SCHEDULE);
        m_initTimeout.start();
    }
    else
    {
        //要删除的温控器是否在添加请求列表中并且不是第一个
        //因为添加请求列表的第一个已经发送了切换为调度命令不能直接删除
        int idx = m_addReqList.indexOf(pRoom);
        if (idx > 0)
        {
            m_addReqList.removeAt(idx);
        }
        else
        {
            m_delReqList.append(pRoom);
            sendCmdModeSetup(pRoom->getRoomIdx(), ROOM_MODE_MANUAL);
            m_deinitTimeout.start();
        }
    }
}

void RoomScheduleThread::slotInitTimeout()
{
    QString strMsg = QString("No ack for init schedule of Room#%1")\
            .arg(m_addReqList.first()->getRoomIdx());
    qDebug() << "RoomScheduleThread::slotInitTimeout" << strMsg;

    emit sigScheduleError(m_addReqList.first(), SCHERR_INIT_NOACK);

    m_initTimeout.stop();
    m_addReqList.removeFirst();
    if (!m_addReqList.isEmpty())
    {
        sendCmdModeSetup(m_addReqList.first()->getRoomIdx(), true);
        m_initTimeout.start();
    }
}

void RoomScheduleThread::slotDeinitTimeout()
{
    QString strMsg = QString("No ack for deinit schedule of Room#%1")\
            .arg(m_delReqList.first()->getRoomIdx());
    qDebug() << "RoomScheduleThread::slotDeinitTimeout" << strMsg;

    emit sigScheduleError(m_delReqList.first(), SCHERR_DEINIT_NOACK);

    m_deinitTimeout.stop();
    m_delReqList.removeFirst();
    if (!m_delReqList.isEmpty())
    {
        sendCmdModeSetup(m_delReqList.first()->getRoomIdx(), false);
        m_deinitTimeout.start();
    }
}

void RoomScheduleThread::slotTokenTimeout()
{
    QString strMsg = QString("No ack for send token of Room#%1")\
            .arg(m_pTokenHolder->getRoomIdx());
    qDebug() << "RoomScheduleThread::slotTokenTimeout" << strMsg;

    m_tokenTimeout.stop();
    //更新后如调度列表为空，返回true，调度器进入空闲状态；否则继续
    if (!updateScheduleList())
    {
        m_pTokenHolder = findNextTokenHolder();
        sendCmdToken(m_pTokenHolder->getRoomIdx());
        m_tokenTimeout.start();
    }
}

void RoomScheduleThread::slotAckReceived(QByteArray frame)
{
    //qDebug() << "RoomScheduleThread::slotAckReceived" << frame.toHex();
    unsigned char paraVal = frame[FRAME_FIELD_PARAVAL];
    if (paraVal == PARAIDX_MODE_SETUP)
    {
        if ((unsigned char)frame[FRAME_FIELD_PARAVAL + 1] == ROOM_MODE_SCHEDULE)
        {
            if (!m_addReqList.isEmpty() && (unsigned char)frame[FRAME_FIELD_ROOMIDX] == m_addReqList.first()->getRoomIdx())
            {           
                if (m_scheduleList.isEmpty())
                {
                    m_scheduleList.append(m_addReqList.first());
                    m_waitingFlag.append(false);

                    m_pTokenHolder = m_addReqList.first();
                    sendCmdToken(m_pTokenHolder->getRoomIdx());
                    m_tokenTimeout.start();
                }
                else
                {
                    m_addList.append(m_addReqList.first());
                }
                m_addReqList.removeFirst();

                if (!m_addReqList.isEmpty())
                {
                    sendCmdModeSetup(m_addReqList.first()->getRoomIdx(), true);
                    m_initTimeout.start();
                }
                else
                {
                    m_initTimeout.stop();
                }
            }
        }
        else if ((unsigned char)frame[FRAME_FIELD_PARAVAL + 1] == ROOM_MODE_MANUAL)
        {
            if (!m_delReqList.isEmpty() && (unsigned char)frame[FRAME_FIELD_ROOMIDX] == m_delReqList.first()->getRoomIdx())
            {
                m_delList.append(m_delReqList.first());
                m_delReqList.removeFirst();
                if (!m_delReqList.isEmpty())
                {
                    sendCmdModeSetup(m_delReqList.first()->getRoomIdx(), false);
                    m_deinitTimeout.start();
                }
                else
                {
                    m_deinitTimeout.stop();
                }
            }
        }
    }
    else if (paraVal == PARAIDX_SEND_TOKEN)
    {
        if ((unsigned char)frame[FRAME_FIELD_ROOMIDX] == m_pTokenHolder->getRoomIdx())
        {
            m_tokenTimeout.stop();
            //更新后如调度列表为空，返回true，调度器进入空闲状态；否则继续发送令牌
            if (!updateScheduleList())
            {
                m_pTokenHolder = findNextTokenHolder();
                sendCmdToken(m_pTokenHolder->getRoomIdx());
                m_tokenTimeout.start();
            }
        }
    }
}

Room* RoomScheduleThread::findNextTokenHolder()
{
    int curPriority, score, scoreMax;
    int len = m_scheduleList.length();

    int pos = m_waitingFlag.indexOf(false);//等待中的最高优先级温控器索引
    if (pos == -1)
    {
        //如果等待标志向量中均为true，表示所有温控器都已经完成一次调度
        //复位等待标志并为pos赋值为0，重新开始新一轮调度
        m_waitingFlag.fill(false);
        pos = 0;
    }

    curPriority = m_scheduleList[pos]->getRoomPrio();
    scoreMax = calcScheduleScore(m_scheduleList[pos]);
    for (int i = pos + 1; i < len && !m_waitingFlag[i] && curPriority == m_scheduleList[i]->getRoomPrio(); i++)
    {
        score = calcScheduleScore(m_scheduleList[i]);
        if (scoreMax < score)
        {
            scoreMax = score;
            pos = i;
        }
    }
    m_waitingFlag[pos] = true;

    QString strMsg = QString("Room#%1 Score%2 Prio%3")\
            .arg(m_scheduleList[pos]->getRoomIdx())\
            .arg(scoreMax)\
            .arg(m_scheduleList[pos]->getRoomPrio());
    qDebug() << "RoomScheduleThread::findNextTokenHolder" << strMsg;

    return m_scheduleList[pos];
}

int RoomScheduleThread::calcScheduleScore(Room *pRoom)
{
    int score;
    float errFloorTemp = pRoom->getRoomFloorSv() - pRoom->getRoomFloorPv();
    if (errFloorTemp > 0)
    {
        score = 5000 + (pRoom->getRoomAirSv() - pRoom->getRoomAirPv()) * 100;
    }
    else
    {
        score = -5000 + errFloorTemp * 100;
    }
    return score;
}

bool RoomScheduleThread::updateScheduleList()
{
    //删除调度列表中的相应温控器
    if (!m_delList.isEmpty())
    {
        int pos;
        do
        {
            pos = m_scheduleList.indexOf(m_delList.first());
            if (pos != -1)
            {
                m_scheduleList.removeAt(pos);
                m_waitingFlag.remove(pos);
            }
            else
            {
                QString strMsg = QString("No Room#%1 in schedule list!")\
                        .arg(m_delList.first()->getRoomIdx());
                qDebug() << strMsg;
            }
            m_delList.removeFirst();
        }while (!m_delList.isEmpty());
    }

    //按照优先级升序插入新的温控器至调度列表
    if (!m_addList.isEmpty())
    {
        int roomPrio, len, pos;
        do
        {
            roomPrio = m_addList.first()->getRoomPrio();
            len = m_scheduleList.length();

            for (pos = 0; pos != len; pos++)
            {
                if (roomPrio > m_scheduleList[pos]->getRoomPrio())
                {
                    break;
                }
            }
            if (pos == len)
            {
                pos = 0;
            }
            m_waitingFlag.insert(pos, false);
            m_scheduleList.insert(pos, m_addList.first());
            m_addList.removeFirst();
        }while (!m_addList.isEmpty());
    }

    return m_scheduleList.isEmpty();
}

void RoomScheduleThread::sendCmdModeSetup(int roomIdx, int roomMode)
{
    QByteArray frame(4, 0);
    frame[FRAME_FIELD_LEN] = 1;
    frame[FRAME_FIELD_ROOMIDX] = roomIdx;
    frame[FRAME_FIELD_PARAIDX] = PARAIDX_MODE_SETUP;
    frame[FRAME_FIELD_PARAVAL] = roomMode;
    emit sigSendCmd(frame);
}

void RoomScheduleThread::sendCmdToken(int roomIdx)
{
    QByteArray frame(4, 0);
    frame[FRAME_FIELD_LEN] = 1;
    frame[FRAME_FIELD_ROOMIDX] = roomIdx;
    frame[FRAME_FIELD_PARAIDX] = PARAIDX_SEND_TOKEN;
    frame[FRAME_FIELD_PARAVAL] = 'T';
    emit sigSendCmd(frame);
}

void RoomScheduleThread::externStartTest(Room *pRoom, bool start)
{
    m_scheduleList.append(pRoom);
    m_waitingFlag.append(false);
    if (start)
    {
        m_pTokenHolder = findNextTokenHolder();
        sendCmdToken(m_pTokenHolder->getRoomIdx());
        m_tokenTimeout.start();
    }
}
