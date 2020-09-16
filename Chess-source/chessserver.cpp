#include "chessserver.h"

ChessServer::ChessServer(QWidget *parent,QTcpSocket *socket,ChessBoard *board) : QObject(parent),socket(socket),board(board),parent(parent)
{
    packet=new Packet(this,socket);
    connect(packet,&Packet::newPacket,this,&ChessServer::READ);
    isplaying=false;
    connect(board,&ChessBoard::move,this,&ChessServer::move);
    timer=new QTimer(this);
    connect(timer,&QTimer::timeout,this,&ChessServer::timeout);
    subtimer=new QTimer(this);
    subtimer->setSingleShot(false);
    subtimer->setInterval(1000);
    timer->setInterval(60000);
    connect(subtimer,&QTimer::timeout,this,&ChessServer::updatetime);
}
void ChessServer::READ(QByteArray data){
    QDataStream in(data);
    int type;
    in>>type;
    if(type==0){
        if(!white){
            Pos pos1,pos2;
            in>>pos1>>pos2;
            if(_move(pos1,pos2,false)){
                if(situation.pieces[pos2].type==4&&pos2.y==8){
                    targetUp=pos2;
                    QByteArray data;
                    QDataStream out(&data,QIODevice::ReadWrite);
                    out<<4;
                    packet->sendMsg(data);
                    waitingUp=true;
                }else{
                    _refrashsituation();
                    _switchside(true);
                }
            }
        }
    }else if(type==1){
        int id;
        in>>id;
        if(!waitingUp)return;
        _Up(id);
        _refrashsituation();
        _switchside(true);
    }else if(type==3){
        if(!white)_win(true);
    }
}
void ChessServer::begin(){
    isplaying=true;
    emit sig_begin();
    board->setStatus(1);
    board->setSide(true);
    QByteArray d1;
    QDataStream out1(&d1, QIODevice::ReadWrite);
    out1<<5;
    packet->sendMsg(d1);
    _refrashsituation();
    _switchside(true);
}
bool ChessServer::_move(Pos pos1,Pos pos2,bool sender){
    if(!isplaying)return false;
    if(white==sender){
        if(situation.pieces.contains(pos1)&&(situation.pieces[pos1].white==sender)){
            QVector<Walk>walks=utils::getFinalRange(situation,pos1);
            for(int i=0;i<walks.size();i++){
                if(walks[i].pos==pos2){
                    utils::move(situation,pos1,pos2);
                    return true;
                }
            }
        }
    }
    return false;
}
void ChessServer::move(Pos pos1,Pos pos2){
    if(_move(pos1,pos2,true)){
        if(situation.pieces.contains(pos2)&&situation.pieces[pos2].type==4&&pos2.y==1){
            UpDialog *up=new UpDialog(parent);
            up->setWindowModality(Qt::WindowModal);
            int id=up->getSetting();
            targetUp=pos2;
            waitingUp=true;
            _Up(id);
        }
        _refrashsituation();
        _switchside(false);
    }
}
void ChessServer::_refrashsituation(){
    if(!isplaying)return;
    board->setSituation(situation);
    QByteArray data;
    QDataStream out(&data,QIODevice::ReadWrite);
    out<<0<<situation;
    packet->sendMsg(data);
}
void ChessServer::_switchside(bool now){
    if(!isplaying)return;
    white=now;
    board->isnowwhite=white;
    QByteArray data;
    QDataStream out(&data,QIODevice::ReadWrite);
    out<<3<<white;
    packet->sendMsg(data);
    timer->start();
    subtimer->start();
    emit sig_switchside(white);
    _judge();
}
void ChessServer::_tie(){
    if(!isplaying)return;
    QByteArray data;
    QDataStream out(&data,QIODevice::ReadWrite);
    out<<2<<2;
    packet->sendMsg(data);
    emit sig_end(2);
}
void ChessServer::_win(bool b){
    if(!isplaying)return;
    QByteArray data;
    QDataStream out(&data,QIODevice::ReadWrite);
    int x=b?0:1;
    out<<2<<x;
    packet->sendMsg(data);
    emit sig_end(!b);
}
void ChessServer::_judge(){
     if(!isplaying)return;
     bool isv=utils::isKingVulnerable(situation,white);
     bool flag=true;
     foreach(Pos pos,situation.pieces.keys()){
         if(situation.pieces.value(pos).white==white&&!utils::getFinalRange(situation,pos).empty()){
             flag=false;
             break;
         }
     }
     if(flag){
         if(isv){
            _win(!white);
         }else{
            _tie();
         }
     }
}
void ChessServer::Reset(){
    waitingUp=false;
    timer->stop();
    subtimer->stop();
    isplaying=false;
    white=true;
    situation=utils::getDefaultSituation();
    board->setSituation(situation);
    board->setStatus(0);
}
void ChessServer::submit(){
    if(white)_win(false);
}
void ChessServer::timeout(){
    _win(!white);
}
void ChessServer::updatetime(){
    int t1,t2;
    if(white){
        t1=timer->remainingTime()/1000;
        t2=60;
    }else{
        t1=60;
        t2=timer->remainingTime()/1000;
    }
    _settime(t1,t2);
}
void ChessServer::_settime(int t1, int t2){
    if(!isplaying)return;
    emit sig_changetime(t1,t2);
    QByteArray data;
    QDataStream out(&data,QIODevice::ReadWrite);
    out<<1<<t1<<t2;
    packet->sendMsg(data);
}
void ChessServer::_Up(int id){
    if(!isplaying)return;
    if(!waitingUp)return;
    if(id<0||id>3)id=0;
    situation.pieces[targetUp].type=id;
    waitingUp=false;
}
void ChessServer::save(){
    if(isplaying)utils::save(parent,situation,white);
}
void ChessServer::load(){
    Situation tem;
    bool w;
    if(utils::load(parent,tem,w)){
        if(!isplaying){
            begin();
        }
        situation=tem;
        _refrashsituation();
        _switchside(w);
    }

}
