#ifndef GLOBAL_H
#define GLOBAL_H

#include <QQueue>
#include <QSemaphore>
#include <QMutex>

#ifdef _RELEASE
#include "3rd-party/SYSZUXpinyin/syszuxim.h"
extern QWSInputMethod *g_im;
#endif

extern QQueue<unsigned char> g_comBuf;
extern QSemaphore g_comBufUsed, g_comBufFree;

extern QQueue<unsigned char> g_tcpBuf;
extern QSemaphore g_tcpBufUsed, g_tcpBufFree;

typedef enum
{
    PARAIDX_AIRTEMP_SV = 1, \
    PARAIDX_FLOORTEMP_SV, \
    PARAIDX_AIRTEMP_PV, \
    PARAIDX_FLOORTEMP_PV, \
    PARAIDX_RELAY_STATE, \
    PARAIDX_RELAY_SETUP, \
    PARAIDX_SLEEP_SETUP, \
    PARAIDX_TIMER_ON, \
    PARAIDX_TIMER_OFF, \
    PARAIDX_DEVICEID_CHANGE,\
    PARAIDX_PRIORITY_SETUP, \
    PARAIDX_MODE_SETUP, \
    PARAIDX_SEND_TOKEN,
    PARAIDX_TAKEN_TOKEN,\
    PARAIDX_PANID_SETUP = 16,\
    PARAIDX_INIT_COOR,\
    PARAIDX_ACK = 0xfe, \
    PARAIDX_POWER_SETUP = 0xff
}paraIdx_t;

typedef enum
{
    FRAME_FIELD_LEN = 0, \
    FRAME_FIELD_ROOMIDX, \
    FRAME_FIELD_PARAIDX, \
    FRAME_FIELD_PARAVAL
}frameField_t;

typedef enum
{
    COL_ROOM_ID = 0, \
    COL_ROOM_NAME, \
    COL_ROOM_AIRSV, \
    COL_ROOM_AIRPV, \
    COL_ROOM_FLOORSV, \
    COL_ROOM_FLOORPV, \
    COL_ROOM_TIMERON, \
    COL_ROOM_TIMEROFF, \
    COL_ROOM_MODE, \
    COL_ROOM_PRIORITY, \
    COL_ROOM_SLEEPON, \
    COL_ROOM_SLEEPOFF, \
    COL_ROOM_SLEEPSV
}roomDbColName_t;

typedef enum
{
    ROOM_MODE_MANUAL = 0, \
    ROOM_MODE_SCHEDULE, \
    ROOM_MODE_SLEEP
}roomMode_t;

typedef enum
{
    ROOM_PRIO_HIGH = 1, \
    ROOM_PRIO_MID, \
    ROOM_PRIO_LOW, \
}roomPriority_t;

typedef enum
{
    SCHERR_INIT_NOACK = 0, \
    SCHERR_DEINIT_NOACK, \
    SCHERR_TOKEN_NOACK
}scheduleError_t;

typedef enum
{
    COL_USER_NAME= 0,\
    COL_USER_CODE,\
    COL_USER_PRIORITY,\
    COL_USER_ID\
}UserDbColName_t;
#endif // GLOBAL_H
