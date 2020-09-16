#include "packet.h"

Packet::Packet(QObject *parent,QTcpSocket *socket) : QObject(parent),socket(socket)
{
    connect(socket,SIGNAL(readyRead()),this,SLOT(received()));
}
void Packet::received(){
    QDataStream clientReadStream(socket);
        while(true) {
            if (!next_block_size) {
                if (socket->bytesAvailable() < sizeof(uint)) { // are size data available
                    break;
                }
                clientReadStream >> next_block_size;
            }
            if (socket->bytesAvailable() < next_block_size) {
                break;
            }
            char* raw;

            clientReadStream.readBytes(raw,next_block_size);
            QByteArray data(raw,next_block_size);
            emit newPacket(data);
            next_block_size=0;
        }
}
void Packet::sendMsg(QByteArray data){
    QByteArray rec;
    QDataStream sendStream(&rec,QIODevice::ReadWrite);
    sendStream << uint(data.size()) << data;

    socket->write(rec);
}
