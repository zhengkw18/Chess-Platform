#ifndef CHESSCLIENT_H
#define CHESSCLIENT_H

#include <QMainWindow>
#include <QObject>
#include <QWidget>
#include <utils.h>
#include <chessboard.h>
#include <packet.h>
class ChessClient : public QObject
{
    Q_OBJECT
public:
    explicit ChessClient(QWidget *parent,QTcpSocket *socket,ChessBoard *board);
    void Reset();
signals:
    void sig_begin();
    void sig_switchside(bool white);
    void sig_end(int id);
    void sig_changetime(int white,int black);
public slots:
    void READ(QByteArray data);
    void move(Pos pos1,Pos pos2);
    void submit();
    void save();
private:
    QTcpSocket *socket;
    ChessBoard *board;
    QWidget *parent;
    Packet *packet;
    bool white=true;
    bool isplaying=false;
    Situation situation=utils::getDefaultSituation();
};

#endif // CHESSCLIENT_H
