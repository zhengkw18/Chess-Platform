#include "chessboard.h"

ChessBoard::ChessBoard(QWidget *parent) : QWidget(parent)
{
    this->black[0]=new QPixmap(":/image/black_queen.png");
    this->black[1]=new QPixmap(":/image/black_bishop.png");
    this->black[2]=new QPixmap(":/image/black_rook.png");
    this->black[3]=new QPixmap(":/image/black_knight.png");
    this->black[4]=new QPixmap(":/image/black_pawn.png");
    this->black[5]=new QPixmap(":/image/black_king.png");
    this->white[0]=new QPixmap(":/image/white_queen.png");
    this->white[1]=new QPixmap(":/image/white_bishop.png");
    this->white[2]=new QPixmap(":/image/white_rook.png");
    this->white[3]=new QPixmap(":/image/white_knight.png");
    this->white[4]=new QPixmap(":/image/white_pawn.png");
    this->white[5]=new QPixmap(":/image/white_king.png");
    status=0;
}
void ChessBoard::paintEvent(QPaintEvent *event){
    QPainter painter(this);
    QFont font;
    font.setPointSize(15);
    painter.setFont(font);
    for(int i=0;i<8;i++){
        for(int j=0;j<8;j++){
            painter.setPen(Qt::NoPen);
            if((i+j)%2){
                painter.setBrush(QColor(qRgb(181,135,99)));
            }else{
                painter.setBrush(QColor(qRgb(240,218,181)));
            }
            painter.drawRect(100*i,100*j,100,100);
            if((i+j)%2){
                painter.setPen(QColor(qRgb(240,218,181)));
            }else{
                painter.setPen(QColor(qRgb(181,135,99)));
            }
            if(i==0){
                painter.drawText(0,100*j+22,QString::number(8-j));
            }
            if(j==7){
                painter.drawText(100*i+85,797,QString('a'+i));
            }
        }
    }
    if(status!=0){
        for(auto it=situation.pieces.begin();it!=situation.pieces.end();it++){
            if(it.value().white){
                painter.drawPixmap(100*it.key().x-100,100*it.key().y-100,100,100,*white[it.value().type]);
            }else{
                painter.drawPixmap(100*it.key().x-100,100*it.key().y-100,100,100,*black[it.value().type]);
            }

        }
    }
    if(status==2){
        painter.setBrush(Qt::NoBrush);
        painter.setPen(QPen(Qt::yellow,3));
        painter.drawRect(100*selectedpoint.x-100,100*selectedpoint.y-100,100,100);
        for(int i=0;i<validpoints.size();i++){
            Walk walk=validpoints[i];
            if(walk.attack==0){
                painter.setPen(QPen(Qt::green,3));
            }else if(walk.attack==1){
                painter.setPen(QPen(Qt::red,3));
            }else{
                painter.setPen(QPen(Qt::blue,3));
            }
            painter.drawRect(100*walk.pos.x-100,100*walk.pos.y-100,100,100);
        }
    }
}
void ChessBoard::mousePressEvent(QMouseEvent *event){
    if(event->button()==Qt::LeftButton){
        if(status==1){
            if(iswhite!=isnowwhite)return;
            Pos pos=getLatticePos({event->x(),event->y()});
            if(situation.pieces.contains(pos)&&(situation.pieces[pos].white==iswhite)){
                status=2;
                selectedpoint=pos;
                validpoints=utils::getFinalRange(situation,pos);
                repaint();
            }
        }else if(status==2){
            Pos pos=getLatticePos({event->x(),event->y()});
            for(int i=0;i<validpoints.size();i++){
                if(validpoints[i].pos==pos){
                    emit move(selectedpoint,pos);
                    break;
                }
            }
            status=1;
            repaint();
        }
    }

}
void ChessBoard::setSituation(Situation s){
    situation=s;
    repaint();
}
void ChessBoard::setStatus(int id){
    status=id;
    repaint();
}
Pos ChessBoard::getLatticePos(Pos pos){
    return {1+pos.x/100,1+pos.y/100};
}
void ChessBoard::setSide(bool iw){
    iswhite=iw;
}
