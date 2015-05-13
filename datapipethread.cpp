#include "datapipethread.h"

#include <QtNetwork>
#include <QDebug>

DataPipeThread::DataPipeThread(QObject *parent) :
    QThread(parent)
{
    initTcpServer();
    initCom();
    start();
}

DataPipeThread::~DataPipeThread()
{

}

void DataPipeThread::stop()
{
    m_pComPollingTimer->stop();
    m_pCom->close();

    m_pTcpServer->close();
    m_pTcpServer->closeAllClient();

    quit();
}

void DataPipeThread::initCom()
{
    //serial port initialization
#if defined Q_OS_WIN32
    m_pCom = new Win_QextSerialPort("COM4", QextSerialBase::Polling);
#elif defined Q_OS_LINUX
    m_pCom = new Posix_QextSerialPort("/dev/yc2440_serial1", QextSerialBase::Polling);
#endif

    m_pCom->open(QIODevice::ReadWrite);
    m_pCom->setBaudRate(BAUD115200);
    m_pCom->setDataBits(DATA_8);
    m_pCom->setParity(PAR_NONE);
    m_pCom->setStopBits(STOP_1);
    m_pCom->setFlowControl(FLOW_OFF);
    m_pCom->setTimeout(500);

    //polling timer initialization
    m_pComPollingTimer = new QTimer(this);
    m_pComPollingTimer->setInterval(500);
    connect(m_pComPollingTimer, SIGNAL(timeout()), this, SLOT(comReadPolling()));
    m_pComPollingTimer->start();
}

void DataPipeThread::comReadPolling()
{
    int len = m_pCom->bytesAvailable();

    // wait for at most 10ms for len bytes to become available.
    if (len > 0 && g_comBufFree.tryAcquire(len, 10))
    {
        QByteArray data = m_pCom->read(len);
        for (int i = 0; i < len; i++)
        {
            g_comBuf.enqueue(data[i]);
        }
        g_comBufUsed.release(len);

        if (m_pTcpServer->getClientNum() != 0)
        {
            m_pTcpServer->sendDataBroadcast(data);
        }

        qDebug() << "comReadPolling " << data.toHex() << endl;
    }
}

void DataPipeThread::initTcpServer()
{
    //tcp server initialization
    m_pTcpServer = new TcpServer();
#ifdef _RELEASE
    m_pTcpServer->listen(QHostAddress("192.168.0.102"), 8080);
#else
    m_pTcpServer->listen(QHostAddress::LocalHost, 8080);
#endif

    connect(m_pTcpServer, SIGNAL(clientConnect(int, QString, int)), \
            this, SLOT(tcpClientConnect(int, QString, int)));

    connect(m_pTcpServer, SIGNAL(clientDisconnect(int, QString, int)), \
            this, SLOT(tcpClientDisconnect(int, QString, int)));

    connect(m_pTcpServer, SIGNAL(clientReadData(int, QString, int, QByteArray)), \
            this, SLOT(tcpClientReadData(int, QString, int, QByteArray)));
}

void DataPipeThread::tcpClientConnect(int clientId, QString ip, int port)
{
    QString str = QString("Client#%1 %2:%3 is connected.")
            .arg(clientId).arg(ip).arg(port);
    qDebug() << str;
}

void DataPipeThread::tcpClientDisconnect(int clientId, QString ip, int port)
{
    QString str = QString("Client#%1 %2:%3 is disconnected.")
            .arg(clientId).arg(ip).arg(port);
    qDebug() << str;
}

void DataPipeThread::tcpClientReadData(int clientId, QString ip, int port, QByteArray data)
{
    QString str = QString("Client#%1 %2:%3")
            .arg(clientId).arg(ip).arg(port);
    qDebug() << str << data.toHex();

    // passthrough from tcp to com
    if (m_pCom != NULL && m_pCom->isOpen())
    {
        m_pCom->write(data);
    }

    // save to buffer waiting for analyze
    int len = data.length();
    for (int i = 0; i < len; i++)
    {
        g_tcpBufFree.acquire();
        g_tcpBuf.enqueue(data[i]);
        g_tcpBufUsed.release();
    }
}

void DataPipeThread::tcpWritePara(QByteArray para)
{
    QByteArray frame(para.length() + 2, 0);
    packagePara(para, frame);

    if (m_pTcpServer->getClientNum() != 0)
    {
        m_pTcpServer->sendDataBroadcast(frame);
        qDebug() << "DataPipeThread::tcpWritePara" << frame.toHex();
    }


}

void DataPipeThread::comWritePara(QByteArray para)
{
    QByteArray frame(para.length() + 2, 0);

    packagePara(para, frame);

    if (m_pCom != NULL && m_pCom->isOpen())
    {
        m_pCom->write(frame);
        qDebug() << "DataPipeThread::comWritePara" << frame.toHex();
    }


}

void DataPipeThread::packagePara(QByteArray para, QByteArray &frame)
{
    //SOF
    frame[0] = 0xfe;

    char xorCheck = 0;
    int len = para.length();
    for (int i = 0; i < len; i++)
    {
        xorCheck ^= para[i];
        frame[i + 1] = para[i];
    }
    frame[len + 1] = xorCheck;
}
