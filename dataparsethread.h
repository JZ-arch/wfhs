#ifndef DataParseThread_H
#define DataParseThread_H

#include <QThread>
#include <QQueue>
#include <QSemaphore>

class DataParseThread : public QThread
{
    Q_OBJECT
public:
    explicit DataParseThread(QQueue<unsigned char> *pBuf, QSemaphore *pSemBufUsed, QSemaphore *pSemBufFree, QObject *parent = 0);
    ~DataParseThread();
    void stop();

protected:
    void run();

signals:
    void sigParaReady(QByteArray para);
    void sigAckReceived(QByteArray ack);

private:

    typedef enum
    {
        SOP_STATE = 0, \
        LEN_STATE, \
        ROOMIDX_STATE, \
        PARAIDX_STATE, \
        PARAVAL_STATE, \
        FCS_STATE
    }ProtocolField_t;
    ProtocolField_t m_parseState;

    bool m_stopFlag;
    QByteArray m_paraFrame;
    unsigned char m_xorCheck, m_paraLen, m_byteCnt, m_paraIdx;

    QQueue<unsigned char> *m_pBuf;
    QSemaphore *m_pSemBufUsed, *m_pSemBufFree;
};

#endif // COMDataParseThread_H
