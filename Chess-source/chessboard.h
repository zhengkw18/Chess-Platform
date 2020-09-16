#ifndef CHESSBOARD_H
#define CHESSBOARD_H

#include <QMainWindow>
#include <QObject>
#include <QWidget>
#include <utils.h>

class ChessBoard : public QWidget
{
    Q_OBJECT
public:
    explicit ChessBoard(QWidget *parent);
    void setSituation(Situation s);
    void setStatus(int id);
    void setSide(bool iw);
    bool isnowwhite;
protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent * event) override;
signals:
    void move(Pos pos,Pos pos2);
private:
    bool iswhite;
    Pos selectedpoint;
    QVector<Walk> validpoints;
    Situation situation=utils::getDefaultSituation();
    QPixmap *black[6],*white[6];//0~5 queen bishop rook knight pawn king
    int status;//0:empty 1:loaded 2:selecting
    Pos getLatticePos(Pos pos);
};

#endif // CHESSBOARD_H
