#ifndef TcpClient_H
#define TcpClient_H

#include <QTcpSocket>

class TcpClient : public QTcpSocket
{
    Q_OBJECT
public:
    explicit TcpClient(QObject *parent = 0, int clientId = 0);
    int getClientID() const { return m_clientId; }

private:
    int m_clientId;

signals:
    void clientReadData(int clientId, QString IP, int Port, QByteArray data);
    void clientDisConnect(int clientId, QString IP, int Port);

private slots:
    void readData();
    void disConnect();

};

#endif // TcpClient_H
