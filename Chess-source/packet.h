#ifndef PACKET_H
#define PACKET_H

#include <QObject>
#include <utils.h>
class Packet : public QObject
{
    Q_OBJECT
public:
    explicit Packet(QObject *parent,QTcpSocket *socket);
    void sendMsg(QByteArray data);
signals:
    void newPacket(QByteArray data);

private slots:
    void received();
private:
    QTcpSocket *socket;
    uint next_block_size=0;
};

#endif // PACKET_H
