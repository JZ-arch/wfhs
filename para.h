#ifndef PARA_H
#define PARA_H

#include <QObject>

class Para : public QObject
{
    Q_OBJECT

public:
    explicit Para(int roomIdx, int paraIdx, int paraVal, QObject *parent = 0) \
        : QObject(parent), m_roomIdx(roomIdx), m_paraIdx(paraIdx), m_paraVal(paraVal)
    {

    }

    Para() : QObject(0), m_roomIdx(0), m_paraIdx(0), m_paraVal(0)
    {

    }

    Para(const Para & para) : QObject(0)
    {
        m_roomIdx = para.getRoomIdx();
        m_paraIdx = para.getParaIdx();
        m_paraVal = para.getParaVal();
    }

    int getRoomIdx() const {return m_roomIdx;}
    int getParaIdx() const {return m_paraIdx;}
    int getParaVal() const {return m_paraVal;}

    void setRoomIdx(int roomIdx) {m_roomIdx = roomIdx;}
    void setParaIdx(int paraIdx) {m_paraIdx = paraIdx;}
    void setParaVal(int paraVal) {m_paraVal = paraVal;}

private:
    int m_roomIdx, m_paraIdx, m_paraVal;
};

#endif // PARA_H
