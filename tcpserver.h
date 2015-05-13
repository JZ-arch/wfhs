#ifndef TcpServer_H
#define TcpServer_H

#include <QTcpServer>

#include "tcpclient.h"

class TcpServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit TcpServer(QObject *parent = 0);
    ~TcpServer();

    void sendDataP2p(int clientID, QByteArray data);
    void sendDataBroadcast(QByteArray data);

    void closeAllClient();

    int getClientNum() const { return m_clientNum; }

private:
    QList<TcpClient*> m_clientList;
    int m_clientNum;

protected:
    void incomingConnection(int handle);

signals:
    void clientReadData(int clientID, QString ip, int port, QByteArray data);
    void clientConnect(int clientID, QString ip, int port);
    void clientDisconnect(int clientID, QString ip, int port);

private slots:
    void disconnect(int clientID, QString ip, int port);
};

#endif // TcpServer_H
