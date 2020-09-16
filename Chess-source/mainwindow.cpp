#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->bg=new QPixmap(":/image/bg.png");
    QPalette palette;
    palette.setBrush(QPalette::Background, QBrush(*bg));
    this->setPalette(palette);
    connect(ui->action,&QAction::triggered,this,[=](){
        dlg=new ConnectionDialog(this);
        dlg->setAttribute(Qt::WA_DeleteOnClose);//设置对话框关闭后，自动销毁
        dlg->setWindowModality(Qt::WindowModal);
        connect(dlg,&ConnectionDialog::serverdone,this,&MainWindow::initiateServer);
        connect(dlg,&ConnectionDialog::clientdone,this,&MainWindow::initiateClient);
        dlg->exec();
    });
    board=new ChessBoard(this);
    board->setGeometry(0,30,800,800);
    this->setFixedSize(1000,830);
    ui->pushButton->setDisabled(true);
    ui->pushButton_2->setDisabled(true);
    ui->pushButton_4->setDisabled(true);
    ui->pushButton_5->setDisabled(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::initiateClient(QTcpSocket *c){
    ui->action->setDisabled(true);
    ChessClient *chessclient=new ChessClient(this,c,board);
    isserver=false;
    client=chessclient;
    connect(ui->pushButton_2,&QPushButton::clicked,chessclient,&ChessClient::submit);
    connect(ui->pushButton_5,&QPushButton::clicked,chessclient,&ChessClient::save);
    connect(chessclient,&ChessClient::sig_begin,this,&MainWindow::begin);
    connect(chessclient,&ChessClient::sig_end,this,&MainWindow::end);
    connect(chessclient,&ChessClient::sig_switchside,this,&MainWindow::switchside);
    connect(chessclient,&ChessClient::sig_changetime,this,&MainWindow::changetime);
}
void MainWindow::initiateServer(QTcpSocket *c){
    ui->action->setDisabled(true);
    ui->pushButton->setDisabled(false);
    ui->pushButton_4->setDisabled(false);
    ChessServer *chessserver=new ChessServer(this,c,board);
    isserver=true;
    server=chessserver;
    connect(ui->pushButton,&QPushButton::clicked,chessserver,&ChessServer::begin);
    connect(ui->pushButton_2,&QPushButton::clicked,chessserver,&ChessServer::submit);
    connect(ui->pushButton_4,&QPushButton::clicked,chessserver,&ChessServer::load);
    connect(ui->pushButton_5,&QPushButton::clicked,chessserver,&ChessServer::save);
    connect(chessserver,&ChessServer::sig_begin,this,&MainWindow::begin);
    connect(chessserver,&ChessServer::sig_end,this,&MainWindow::end);
    connect(chessserver,&ChessServer::sig_switchside,this,&MainWindow::switchside);
    connect(chessserver,&ChessServer::sig_changetime,this,&MainWindow::changetime);
}
void MainWindow::begin(){
    ui->pushButton->setDisabled(true);
    ui->pushButton_5->setDisabled(false);
}
void MainWindow::end(int id){
    if(isserver){
        server->Reset();
        ui->lcdNumber->display(0);
        ui->lcdNumber_2->display(0);
        ui->pushButton->setDisabled(false);
        ui->pushButton_2->setDisabled(true);
        ui->pushButton_4->setDisabled(false);
        ui->pushButton_5->setDisabled(true);
    }else{
        client->Reset();
        ui->lcdNumber->display(0);
        ui->lcdNumber_2->display(0);
        ui->pushButton->setDisabled(true);
        ui->pushButton_2->setDisabled(true);
        ui->pushButton_4->setDisabled(true);
        ui->pushButton_5->setDisabled(true);
    }
    if(id==0){
        QMessageBox::information(this, "Game over", "白方赢了");
    }else if(id==1){
        QMessageBox::information(this, "Game over", "黑方赢了");
    }else if(id==2){
        QMessageBox::information(this, "Game over", "双方和棋");
    }
}
void MainWindow::switchside(bool white){
    if(isserver){
        if(white){
            ui->pushButton_2->setDisabled(false);
            ui->pushButton_4->setDisabled(false);
        }else{
            ui->pushButton_2->setDisabled(true);
            ui->pushButton_4->setDisabled(true);
        }
    }else{
        if(white){
            ui->pushButton_2->setDisabled(true);
            ui->pushButton_4->setDisabled(true);
        }else{
            ui->pushButton_2->setDisabled(false);
            ui->pushButton_4->setDisabled(true);
        }
    }
}
void MainWindow::changetime(int white,int black){
    ui->lcdNumber->display(black);
    ui->lcdNumber_2->display(white);
}
