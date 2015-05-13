#ifndef DATAPIPETHREAD_H
#define DATAPIPETHREAD_H

#include <QThread>
#include <QTimer>

#include "global.h"
#include "para.h"
#include "tcpserver.h"
#include "3rd-party/serialport/qextserialbase.h"

#if defined Q_OS_WIN32
    #include "3rd-party/serialport/win_qextserialport.h"
#elif defined Q_OS_LINUX
    #include "3rd-party/serialport/posix_qextserialport.h"
#endif

class DataPipeThread : public QThread
{
    Q_OBJECT
public:
    explicit DataPipeThread(QObject *parent = 0);
    ~DataPipeThread();
    void initTcpServer();
    void initCom();
    void packagePara(QByteArray para, QByteArray &frame);
    void stop();

public slots:
    void tcpClientConnect(int clientId, QString ip, int port);
    void tcpClientDisconnect(int clientId, QString ip, int port);
    void tcpClientReadData(int clientId, QString ip, int port, QByteArray data);
    void tcpWritePara(QByteArray para);

    void comReadPolling();
    void comWritePara(QByteArray para);

private:
    TcpServer *m_pTcpServer;

#ifdef Q_OS_WIN32
    Win_QextSerialPort *m_pCom;
#elif defined Q_OS_LINUX
    Posix_QextSerialPort *m_pCom;
#endif

    QTimer *m_pComPollingTimer;
};

#endif // DATAPIPETHREAD_H
