#include "tcpclient.h"

#include <qhostaddress.h>

TcpClient::TcpClient(QObject *parent, int clientId) :
    QTcpSocket(parent), m_clientId(clientId)
{
    connect(this, SIGNAL(readyRead()), this, SLOT(readData()));
    connect(this, SIGNAL(disconnected()), this, SLOT(disConnect()));
}

void TcpClient::readData()
{
    QByteArray data = this->readAll();
    emit clientReadData(m_clientId, peerAddress().toString(), peerPort(), data);
}

void TcpClient::disConnect()
{
    emit clientDisConnect(m_clientId, peerAddress().toString(), peerPort());
}
