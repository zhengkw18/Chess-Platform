#include "chessclient.h"

ChessClient::ChessClient(QWidget *parent,QTcpSocket *socket,ChessBoard *board) : QObject(parent),socket(socket),board(board),parent(parent)
{
    packet=new Packet(this,socket);
    connect(packet,&Packet::newPacket,this,&ChessClient::READ);
    connect(board,&ChessBoard::move,this,&ChessClient::move);
}
void ChessClient::READ(QByteArray data){
    QDataStream in(data);
    int type;
    in>>type;
    if(type==0){
        Situation s;
        in>>s;
        situation=s;
        board->setSituation(situation);
    }else if(type==1){
        int t1,t2;
        in>>t1>>t2;
        emit sig_changetime(t1,t2);
    }else if(type==2){
        int x;
        in>>x;
        emit sig_end(x);
    }else if(type==3){
        bool w;
        in>>w;
        white=w;
        emit sig_switchside(w);
        board->isnowwhite=w;
    }else if(type==4){
        UpDialog *up=new UpDialog(parent);
        up->setWindowModality(Qt::WindowModal);
        int id=up->getSetting();
        QByteArray data;
        QDataStream out(&data,QIODevice::ReadWrite);
        out<<1<<id;
        packet->sendMsg(data);
    }else if(type==5){
        isplaying=true;
        emit sig_begin();
        board->setStatus(1);
        board->setSide(false);
    }
}
void ChessClient::move(Pos pos1,Pos pos2){
    QByteArray data;
    QDataStream out(&data,QIODevice::ReadWrite);
    out<<0<<pos1<<pos2;
    packet->sendMsg(data);
}
void ChessClient::Reset(){
    isplaying=false;
    white=false;
    situation=utils::getDefaultSituation();
    board->setSituation(situation);
    board->setStatus(0);
}
void ChessClient::submit(){
    QByteArray data;
    QDataStream out(&data,QIODevice::ReadWrite);
    out<<3;
    packet->sendMsg(data);
}
void ChessClient::save(){
    if(isplaying)utils::save(parent,situation,white);
}
