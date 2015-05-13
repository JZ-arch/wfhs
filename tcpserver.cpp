#include "tcpserver.h"

TcpServer::TcpServer(QObject *parent) :
    QTcpServer(parent), m_clientNum(0)
{
}

TcpServer::~TcpServer()
{
    for(int i = 0; i < m_clientNum; i++)
    {
        m_clientList[i]->deleteLater();
    }
    m_clientList.empty();
}

void TcpServer::incomingConnection(int handle)
{
    TcpClient *pClient = new TcpClient(this, handle);
    pClient->setSocketDescriptor(handle);
    connect(pClient, SIGNAL(clientReadData(int, QString, int, QByteArray)), \
            this, SIGNAL(clientReadData(int, QString, int, QByteArray)));
    connect(pClient, SIGNAL(clientDisConnect(int, QString, int)), \
            this, SLOT(disconnect(int, QString, int)));
    emit clientConnect(handle, pClient->peerAddress().toString(), pClient->peerPort());
    m_clientList.append(pClient);
    m_clientNum++;
}

void TcpServer::disconnect(int clientID, QString ip, int port)
{
    for(int i = 0; i < m_clientNum; i++)
    {
        if(m_clientList[i]->getClientID() == clientID)
        {
            m_clientList.removeAt(i);
            i--;
            m_clientNum--;
            emit clientDisconnect(clientID, ip, port);
            break;
        }
    }
}

void TcpServer::sendDataP2p(int clientID, QByteArray data)
{
    for(int i = 0; i < m_clientNum; i++)
    {
        if(m_clientList[i]->getClientID() == clientID)
        {
            m_clientList[i]->write(data);
            break;
        }
    }
}
void TcpServer::sendDataBroadcast(QByteArray data)
{
    for(int i = 0; i < m_clientNum; i++)
    {
        m_clientList[i]->write(data);
    }
}

void TcpServer::closeAllClient()
{
    for(int i = 0; i < m_clientNum; i++)
    {
        m_clientList[i]->close();
        //m_clientList[i]->deleteLater();
    }
    //m_clientList.empty();
    //m_clientNum = 0;
}
