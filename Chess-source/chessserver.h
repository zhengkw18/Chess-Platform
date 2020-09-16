#ifndef CHESSSERVER_H
#define CHESSSERVER_H

#include <QMainWindow>
#include <QObject>
#include <QWidget>
#include <utils.h>
#include <chessboard.h>
#include <packet.h>
class ChessServer : public QObject
{
    Q_OBJECT
public:
    explicit ChessServer(QWidget *parent,QTcpSocket *socket,ChessBoard *board);
    void Reset();
signals:
    void sig_begin();
    void sig_switchside(bool white);
    void sig_end(int id);
    void sig_changetime(int white,int black);
public slots:
    void READ(QByteArray data);
    void begin();
    void load();
    void move(Pos pos1,Pos pos2);
    void submit();
    void timeout();
    void updatetime();
    void save();
private:
    QTcpSocket *socket;
    ChessBoard *board;
    QWidget *parent;
    Packet *packet;
    bool isplaying=false;
    bool white=true;
    Situation situation=utils::getDefaultSituation();
    bool _move(Pos pos1,Pos pos2,bool sender);
    void _switchside(bool now);
    void _refrashsituation();
    void _judge();
    void _win(bool b);
    void _tie();
    QTimer *timer;
    QTimer *subtimer;
    void _settime(int t1,int t2);
    void _load();
    bool waitingUp;
    Pos targetUp;
    void _Up(int id);
};

#endif // CHESSSERVER_H
