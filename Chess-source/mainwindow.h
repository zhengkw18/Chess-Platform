#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <utils.h>
#include <connectiondialog.h>
#include <chessboard.h>
#include <chessclient.h>
#include <chessserver.h>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void initiateServer(QTcpSocket*);
    void initiateClient(QTcpSocket*);
private slots:
//proxy到mainwindow的通信
//0:开始(c/s)
//1:换某边(c/s)
//2:结束(c/s)
//3:计时器(c/s)

//mainwindow到proxy的通信
//0:开始(s) 未开始
//1:认输(c/s) 此方
//2:王车易位(c/s) 此方 可易位
//3:载入残局(s) 此方
//4:保存残局(c/s) 此方

//board到proxy的通信
//0:移动
void begin();
void switchside(bool white);
void end(int id);
void changetime(int white,int black);
private:
    Ui::MainWindow *ui;

    QPixmap *bg;
    bool isserver;
    ChessServer *server;
    ChessClient *client;
    ConnectionDialog *dlg;
    ChessBoard *board;
};

#endif // MAINWINDOW_H
