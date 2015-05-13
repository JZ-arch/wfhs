#include "dataparsethread.h"
#include "global.h"
#include <QDebug>

DataParseThread::DataParseThread(QQueue<unsigned char> *pBuf, QSemaphore *pSemBufUsed, QSemaphore *pSemBufFree, QObject *parent) :
    QThread(parent), \
    m_parseState(SOP_STATE), \
    m_stopFlag(false), \
    m_pBuf(pBuf), m_pSemBufUsed(pSemBufUsed), m_pSemBufFree(pSemBufFree)
{
    start();
}

DataParseThread::~DataParseThread()
{
}

void DataParseThread::stop()
{
    m_stopFlag = true;
    if (m_pSemBufUsed->available() == 0)
    {
        m_pBuf->enqueue(0xff);
        m_pSemBufUsed->release();
    }
    quit();
}

void DataParseThread::run()
{
    unsigned char tmp;
    while (1)
    {
        m_pSemBufUsed->acquire();
        tmp = m_pBuf->dequeue();
        m_pSemBufFree->release();

        if (m_stopFlag)
        {
            break;
        }

        //qDebug() << "DataParseThread::run()" << m_parseState;
        if (m_parseState == SOP_STATE)
        {
            if (tmp == (unsigned char)0xfe)
            {
                m_xorCheck = 0;
                m_byteCnt = 0;
                m_parseState = LEN_STATE;
            }
        }
        else
        {
            if (m_parseState != FCS_STATE)
            {
                m_xorCheck ^= tmp;
                m_paraFrame[m_byteCnt++] = tmp;
                if (m_parseState == LEN_STATE)
                {
                    m_paraLen = tmp;
                    m_parseState = ROOMIDX_STATE;
                }
                else if (m_parseState == ROOMIDX_STATE)
                {
                    m_parseState = PARAIDX_STATE;
                }
                else if (m_parseState == PARAIDX_STATE)
                {
                    m_paraIdx = tmp;
                    m_parseState = PARAVAL_STATE;
                }
                else if (m_parseState == PARAVAL_STATE && m_paraLen)
                {
                    m_paraLen--;
                    if (m_paraLen == 0)
                    {
                        m_parseState = FCS_STATE;
                    }
                }
            }
            else
            {
                if (m_xorCheck == tmp)
                {
                    if (m_paraIdx == PARAIDX_ACK)
                    {
                        //qDebug() << "sigAckReceived" << m_paraFrame.toHex();
                        emit sigAckReceived(m_paraFrame);
                    }
                    else
                    {
                        emit sigParaReady(m_paraFrame);
                    }
                }
                else
                {
                    qDebug() << "Frame xor check error!";
                }
                m_parseState = SOP_STATE;
            }
        }
    }
    //qDebug() << "DataParseThread::run() is returned!";
}
