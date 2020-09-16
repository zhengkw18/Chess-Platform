#ifndef UTILS_H
#define UTILS_H
#include <QTcpSocket>
#include <QTcpServer>
#include <QDataStream>
#include <QSet>
#include <QImage>
#include <QVector>
#include <QPixmap>
#include <QPaintEvent>
#include <QAction>
#include <QRegExp>
#include <QString>
#include <QMessageBox>
#include <QDebug>
#include <QMouseEvent>
#include <QPainter>
#include <QColor>
#include <updialog.h>
#include <QTimer>
#include <QFile>
#include <QTextStream>
#include <QFileDialog>
//server to client
//0:发送局面
//1:发送双方计时
//2:发送可走格点
//3:发送胜/负/和 0胜1负2和
//4:换为X边 0白1黑
//5:选择可以生变的类型 0~3 queen bishop rook knight
//6.开始 白方先走 只需调整变量 紧接着还要发换边
//7.请求兵升变

//client to server
//0:请求可走的格子
//1:走棋
//2:升变
//4:认输
struct Pos{
    int x,y;
    bool operator == (const Pos & value) const{
        return (value.x==x&&value.y==y);
    }
    bool operator < (const Pos & value) const{
        if(x<value.x)return true;
        if(x>value.x)return false;
        if(y<value.y)return true;
        if(y>value.y)return false;
        return false;
    }
    friend QDataStream& operator>>(QDataStream& in, Pos& s){
        in>>s.x>>s.y;
        return in;
    }
    friend QDataStream& operator<<(QDataStream& out,const Pos& s){
        out<<s.x<<s.y;
        return out;
    }
};
struct Piece{
    bool white;
    int type;
    friend QDataStream& operator>>(QDataStream& in, Piece& s){
        in>>s.white>>s.type;
        return in;
    }
    friend QDataStream& operator<<(QDataStream& out,const Piece& s){
        out<<s.white<<s.type;
        return out;
    }
};
struct Walk{
    Pos pos;
    int attack;//0:走 1:攻击 2:王车易位
};

struct Situation{
    Pos black_king;
    Pos white_king;
    QMap<Pos,Piece> pieces;
    friend QDataStream& operator>>(QDataStream& in, Situation& s){
        in>>s.black_king;
        in>>s.white_king;
        int size;
        in>>size;
        for(int i=0;i<size;i++){
            Pos pos;
            Piece p;
            in>>pos>>p;
            s.pieces.insert(pos,p);
        }
        return in;
    }
    friend QDataStream& operator<<(QDataStream& out,const Situation& s){
        out<<s.black_king;
        out<<s.white_king;
        out<<s.pieces.size();
        for(auto it=s.pieces.begin();it!=s.pieces.end();it++){
            out<<it.key()<<it.value();
        }
        return out;
    }
};

inline uint qHash(const Pos key){
    return key.x + key.y;
}
namespace utils {
inline Situation getDefaultSituation(){
    Situation situation;
    situation.black_king={5,1};
    situation.white_king={5,8};
    situation.pieces.insert({4,1},{false,0});
    situation.pieces.insert({4,8},{true,0});
    situation.pieces.insert({3,1},{false,1});
    situation.pieces.insert({6,1},{false,1});
    situation.pieces.insert({3,8},{true,1});
    situation.pieces.insert({6,8},{true,1});
    situation.pieces.insert({1,1},{false,2});
    situation.pieces.insert({8,1},{false,2});
    situation.pieces.insert({1,8},{true,2});
    situation.pieces.insert({8,8},{true,2});
    situation.pieces.insert({2,1},{false,3});
    situation.pieces.insert({7,1},{false,3});
    situation.pieces.insert({2,8},{true,3});
    situation.pieces.insert({7,8},{true,3});
    situation.pieces.insert({5,1},{false,5});
    situation.pieces.insert({5,8},{true,5});
    for(int i=1;i<=8;i++){
        situation.pieces.insert({i,2},{false,4});
        situation.pieces.insert({i,7},{true,4});
    }
    return situation;
}
inline void move(Situation& s,Pos pos1,Pos pos2){
        if(s.pieces.contains(pos2)&&s.pieces[pos1].white==s.pieces[pos2].white){
            Piece king=s.pieces[pos1];
            Piece rook=s.pieces[pos2];
            if(s.pieces[pos1].white){
                if(pos1.x==5&&pos1.y==8){
                    if(pos2.x==1&&pos2.y==8){
                        s.pieces.insert({3,8},king);
                        s.pieces.insert({4,8},rook);
                        s.pieces.erase(s.pieces.find(pos1));
                        s.pieces.erase(s.pieces.find(pos2));
                        s.white_king={3,8};
                    }else if(pos2.x==8&&pos2.y==8){
                        s.pieces.erase(s.pieces.find(pos1));
                        s.pieces.erase(s.pieces.find(pos2));
                        s.pieces.insert({7,8},king);
                        s.pieces.insert({6,8},rook);
                        s.white_king={7,8};
                    }
                }
            }else{
                if(pos1.x==5&&pos1.y==1){
                    if(pos2.x==1&&pos2.y==1){
                        s.pieces.erase(s.pieces.find(pos1));
                        s.pieces.erase(s.pieces.find(pos2));
                        s.pieces.insert({3,1},king);
                        s.pieces.insert({4,1},rook);
                        s.black_king={3,1};
                    }else if(pos2.x==8&&pos2.y==1){
                        s.pieces.erase(s.pieces.find(pos1));
                        s.pieces.erase(s.pieces.find(pos2));
                        s.pieces.insert({7,1},king);
                        s.pieces.insert({6,1},rook);
                        s.black_king={7,1};
                    }
                }
            }
        }else{
            if(s.pieces[pos1].type==5){
                if(s.pieces[pos1].white){
                    s.white_king=pos2;
                }else{
                    s.black_king=pos2;
                }
            }
            Piece piece=s.pieces[pos1];
            s.pieces.erase(s.pieces.find(pos1));
            s.pieces.insert(pos2,piece);
        }
    }
//最浅的一步
inline QVector<Walk> getWalkRange(Situation& s,Pos pos){
    QVector<Walk> walks;
    if(!s.pieces.contains(pos))return walks;
    if(s.pieces[pos].type<=2){
        int start,end;
        int step[8][2]={{1,0},{-1,0},{0,1},{0,-1},{1,1},{1,-1},{-1,1},{-1,-1}};
        if(s.pieces[pos].type==0){
            start=0;
            end=8;
        }else if(s.pieces[pos].type==1){
            start=4;
            end=8;
        }else if(s.pieces[pos].type==2){
            start=0;
            end=4;
        }else{
            start=0;
            end=0;
        }
        for(int i=start;i<end;i++){
            int x=pos.x;
            int y=pos.y;
            while(true){
                x+=step[i][0];
                y+=step[i][1];
                if(x>=1&&x<=8&&y>=1&&y<=8){
                    if(s.pieces.contains({x,y})){
                        if(s.pieces[{x,y}].white==s.pieces[pos].white){
                            break;
                        }else{
                            Walk walk;
                            walk.pos={x,y};
                            walk.attack=true;
                            walks.push_back(walk);
                            break;
                        }
                    }else{
                        Walk walk;
                        walk.pos={x,y};
                        walk.attack=false;
                        walks.push_back(walk);
                    }
                }else{
                    break;
                }
            }
        }
    }else if(s.pieces[pos].type==3){
        int step[8][2]={{2,1},{2,-1},{-2,1},{-2,-1},{1,2},{1,-2},{-1,2},{-1,-2}};
        for(int i=0;i<8;i++){
            int x=pos.x+step[i][0];
            int y=pos.y+step[i][1];
            if(x>=1&&x<=8&&y>=1&&y<=8){
                if(s.pieces.contains({x,y})){
                    if(s.pieces[{x,y}].white==s.pieces[pos].white){
                        continue;
                    }else{
                        Walk walk;
                        walk.pos={x,y};
                        walk.attack=true;
                        walks.push_back(walk);
                        continue;
                    }
                }else{
                    Walk walk;
                    walk.pos={x,y};
                    walk.attack=false;
                    walks.push_back(walk);
                }
            }
        }
    }else if(s.pieces[pos].type==4){
        if(s.pieces[pos].white){
            if(pos.y>1){
                if(!s.pieces.contains({pos.x,pos.y-1})){
                    Walk walk;
                    walk.pos={pos.x,pos.y-1};
                    walk.attack=false;
                    walks.push_back(walk);
                    if(pos.y==7){
                        if(!s.pieces.contains({pos.x,pos.y-2})){
                            Walk walk;
                            walk.pos={pos.x,pos.y-2};
                            walk.attack=false;
                            walks.push_back(walk);
                        }
                    }
                }
            }
        }else{
            if(pos.y<8){
                if(!s.pieces.contains({pos.x,pos.y+1})){
                    Walk walk;
                    walk.pos={pos.x,pos.y+1};
                    walk.attack=false;
                    walks.push_back(walk);
                    if(pos.y==2){
                        if(!s.pieces.contains({pos.x,pos.y+2})){
                            Walk walk;
                            walk.pos={pos.x,pos.y+2};
                            walk.attack=false;
                            walks.push_back(walk);
                        }
                    }
                }
            }
        }
        int step[4][2]={{1,1},{-1,1},{1,-1},{-1,-1}};
        int start,end;
        if(s.pieces[pos].white){
            start=2;
            end=4;
        }else{
            start=0;
            end=2;
        }
        for(int i=start;i<end;i++){
            int x=pos.x+step[i][0];
            int y=pos.y+step[i][1];
            if(x>=1&&x<=8&&y>=1&&y<=8&&s.pieces.contains({x,y})&&s.pieces[{x,y}].white!=s.pieces[pos].white){
                Walk walk;
                walk.pos={x,y};
                walk.attack=true;
                walks.push_back(walk);
            }
        }
    }else if(s.pieces[pos].type==5){
        int step[8][2]={{1,0},{-1,0},{0,1},{0,-1},{1,1},{1,-1},{-1,1},{-1,-1}};
        for(int i=0;i<8;i++){
            int x=pos.x+step[i][0];
            int y=pos.y+step[i][1];
            if(x>=1&&x<=8&&y>=1&&y<=8){
                if(s.pieces.contains({x,y})){
                    if(s.pieces[{x,y}].white==s.pieces[pos].white){
                        continue;
                    }else{
                        Walk walk;
                        walk.pos={x,y};
                        walk.attack=true;
                        walks.push_back(walk);
                        continue;
                    }
                }else{
                    Walk walk;
                    walk.pos={x,y};
                    walk.attack=false;
                    walks.push_back(walk);
                }
            }
        }
    }
    return walks;
}

inline bool isKingVulnerable(Situation& s,bool white){
    int x,y;
    if(white){
        x=s.white_king.x;
        y=s.white_king.y;
    }else{
        x=s.black_king.x;
        y=s.black_king.y;
    }
    foreach(Pos pos,s.pieces.keys()){
        if(s.pieces.value(pos).white==white)continue;
        QVector<Walk> walks=getWalkRange(s,pos);
        foreach(Walk walk,walks){
            if(walk.pos.x==x&&walk.pos.y==y)return true;
        }
    }
    return false;
}
inline QVector<Walk> getFinalRange(Situation& s,Pos pos){
    QVector<Walk> raw=getWalkRange(s,pos);
    QVector<Walk> finalwalks;
    for(int i=0;i<raw.size();i++){
        Situation news=s;
        Walk walk=raw[i];
        move(news,pos,walk.pos);
        if(!isKingVulnerable(news,s.pieces[pos].white)){
            finalwalks.push_back(walk);
        }
    }
    //最后判断王车易位可行性
    if(s.pieces[pos].type==5){
        if(s.pieces[pos].white){
            if(pos.x==5&&pos.y==8){
                //王：{5,8}-{3,8}
                //车：{1,8}-{4,8}
                if(s.pieces.contains({1,8})&&s.pieces[{1,8}].type==2){
                    bool flag=true;
                    for(int i=2;i<=4;i++){
                        if(s.pieces.contains({i,8}))flag=false;
                    }
                    for(int i=3;i<=5;i++){
                        Situation news=s;
                        news.white_king={i,8};
                        if(isKingVulnerable(news,true))flag=false;
                    }
                    if(flag){
                        Walk walk;
                        walk.pos={1,8};
                        walk.attack=2;
                        finalwalks.push_back(walk);
                    }
                }
                //王：{5,8}-{7,8}
                //车：{8,8}-{6,8}
                if(s.pieces.contains({8,8})&&s.pieces[{8,8}].type==2){
                    bool flag=true;
                    for(int i=6;i<=7;i++){
                        if(s.pieces.contains({i,8}))flag=false;
                    }
                    for(int i=5;i<=7;i++){
                        Situation news=s;
                        news.white_king={i,8};
                        if(isKingVulnerable(news,true))flag=false;
                    }
                    if(flag){
                        Walk walk;
                        walk.pos={8,8};
                        walk.attack=2;
                        finalwalks.push_back(walk);
                    }
                }
            }
        }else{
            if(pos.x==5&&pos.y==1){
                //王：{5,1}-{3,1}
                //车：{1,1}-{4,1}
                if(s.pieces.contains({1,1})&&s.pieces[{1,1}].type==2){
                    bool flag=true;
                    for(int i=2;i<=4;i++){
                        if(s.pieces.contains({i,1}))flag=false;
                    }
                    for(int i=3;i<=5;i++){
                        Situation news=s;
                        news.black_king={i,1};
                        if(isKingVulnerable(news,false))flag=false;
                    }
                    if(flag){
                        Walk walk;
                        walk.pos={1,1};
                        walk.attack=2;
                        finalwalks.push_back(walk);
                    }
                }
                //王：{5,1}-{7,1}
                //车：{8,1}-{6,1}
                if(s.pieces.contains({8,1})&&s.pieces[{8,1}].type==2){
                    bool flag=true;
                    for(int i=6;i<=7;i++){
                        if(s.pieces.contains({i,1}))flag=false;
                    }
                    for(int i=5;i<=7;i++){
                        Situation news=s;
                        news.black_king={i,1};
                        if(isKingVulnerable(news,false))flag=false;
                    }
                    if(flag){
                        Walk walk;
                        walk.pos={8,1};
                        walk.attack=2;
                        finalwalks.push_back(walk);
                    }
                }
            }
        }
    }
    return finalwalks;
}
inline void save(QWidget *parent,Situation s,bool white){
    QString strFile = QFileDialog::getSaveFileName(parent,"Choose a txt file to save","C:","text files(*txt)");
    if(strFile.isEmpty()||strFile.isNull())return;
    if(!strFile.endsWith(".txt",Qt::CaseInsensitive))strFile+=".txt";
    QFile f(strFile);
    if (!f.open(QIODevice::WriteOnly)){
        QMessageBox::critical(parent, "Saving Failed", "Could not open file.");
        return;
    }
    QString prefix[2]={"white","black"};
    QString ob[6]={"queen","bishop","rook","knight","pawn","king"};
    QVector<Pos> poss[2][6];
    foreach(Pos pos,s.pieces.keys()){
        Piece p=s.pieces[pos];
        poss[!p.white][p.type].push_back(pos);
    }
    QTextStream out(&f);
    out<<prefix[!white]<<"\n";
    for(int i=0;i<6;i++){
        if(!poss[!white][i].empty()){
            out<<ob[i]<<" "<<poss[!white][i].size();
            foreach(Pos pos,poss[!white][i]){
                out<<" "<<QString('a'+pos.x-1)<<QString::number(9-pos.y);
            }
            out<<"\n";
        }

    }
    out<<prefix[white]<<"\n";
    for(int i=0;i<6;i++){
        if(!poss[white][i].empty()){
            out<<ob[i]<<" "<<poss[white][i].size();
            foreach(Pos pos,poss[white][i]){
                out<<" "<<QString('a'+pos.x-1)<<QString::number(9-pos.y);
            }
            out<<"\n";
        }

    }
    f.close();
}
inline bool load(QWidget *parent,Situation &tem,bool &white){
    QString strFile = QFileDialog::getOpenFileName(parent,"Choose a txt file to load","C:","text files(*txt)");
    if(strFile.isEmpty()||strFile.isNull())return false;
    QFile f(strFile);
    if (!f.open(QIODevice::ReadOnly)){
        QMessageBox::critical(parent, "Loading Failed", "Could not open file.");
        return false;
    }
    QTextStream in(&f);
    QMap<QString,int> s2type;
    s2type.insert("queen",0);
    s2type.insert("bishop",1);
    s2type.insert("rook",2);
    s2type.insert("knight",3);
    s2type.insert("pawn",4);
    s2type.insert("king",5);
    bool w=true;
    while (!in.atEnd()) {
        QString s=in.readLine();
        s.trimmed();
        if(s=="white"){
            w=true;
            continue;
        }
        if(s=="black"){
            w=false;
            continue;
        }
        QStringList sl=s.split(" ",QString::SkipEmptyParts);
        if(sl.empty())continue;
        if(s2type.contains(sl.at(0))){
            int type=s2type[sl.at(0)];
            for(int i=2;i<sl.size();i++){
                QString posstr=sl.at(i);
                int x=posstr.at(0).toLatin1()-'a'+1;
                int y='9'-posstr.at(1).toLatin1();
                if(type==5){
                    if(w){
                        tem.white_king={x,y};
                    }else{
                        tem.black_king={x,y};
                    }
                }
                Piece piece;
                piece.type=type;
                piece.white=w;
                tem.pieces.insert({x,y},piece);
            }
        }
    }
    white=!w;
}
}
#endif // UTILS_H
