#include "connectiondialog.h"
#include "ui_connectiondialog.h"

ConnectionDialog::ConnectionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConnectionDialog),parent(parent)
{
    ui->setupUi(this);
    QRegExp regExp("^((2(5[0-5]|[0-4]\\d))|[0-1]?\\d{1,2})(\\.((2(5[0-5]|[0-4]\\d))|[0-1]?\\d{1,2})){3}$");
    QRegExpValidator *pRegExpValidator = new QRegExpValidator(regExp,this);
    ui->lineEdit->setValidator(pRegExpValidator);
    ui->pushButton_2->setVisible(false);
    ui->label_display->setVisible(false);
    ui->label_ip->setVisible(false);
    ui->label_port->setVisible(false);
    connect(ui->pushButton,SIGNAL(clicked()),this,SLOT(OK()));
    connect(ui->pushButton_2,SIGNAL(clicked()),this,SLOT(Cancel()));
}

ConnectionDialog::~ConnectionDialog()
{
    delete ui;
}
void ConnectionDialog::displayConnecting(){
    ui->spinBox->setVisible(false);
    ui->lineEdit->setVisible(false);
    ui->pushButton->setVisible(false);
    ui->comboBox->setVisible(false);

    ui->label_ip->setVisible(true);
    ui->label_port->setVisible(true);
    ui->label_display->setVisible(true);
    ui->pushButton_2->setVisible(true);
}
void ConnectionDialog::cancelConnecting(){
    ui->spinBox->setVisible(true);
    ui->lineEdit->setVisible(true);
    ui->pushButton->setVisible(true);
    ui->comboBox->setVisible(true);

    ui->label_ip->setVisible(false);
    ui->label_port->setVisible(false);
    ui->label_display->setVisible(false);
    ui->pushButton_2->setVisible(false);
}
void ConnectionDialog::OK(){
    this->ip=ui->lineEdit->text();
    this->port=ui->spinBox->text().toInt();
    ui->label_port->setText(ui->spinBox->text());
    ui->label_ip->setText(ui->lineEdit->text());
    if(ui->comboBox->currentIndex()==0){
        ui->label_display->setText("Waiting for connection…");
        displayConnecting();
        server=new QTcpServer(parent);
        server->listen(QHostAddress(ip),port);
        connect(server,SIGNAL(newConnection()),this,SLOT(newconnection()));
    }else{
        ui->label_display->setText("Connecting…");
        socket=new QTcpSocket(parent);
        socket->connectToHost(QHostAddress(ip),port);
        displayConnecting();
        connect(socket,SIGNAL(connected()),this,SLOT(connected()));
        connect(socket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(error()));
    }
}
void ConnectionDialog::Cancel(){
    if(ui->comboBox->currentIndex()==0){
        server->close();
        cancelConnecting();
    }else{
        socket->disconnectFromHost();
        cancelConnecting();
    }
}
void ConnectionDialog::connected(){
    emit clientdone(socket);
    disconnect(socket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(error()));
    close();
}
void ConnectionDialog::error(){
    cancelConnecting();
    QMessageBox::critical(this, "Connecting Failed", "error");
}
void ConnectionDialog::newconnection(){
    socket=server->nextPendingConnection();
    server->close();
    emit serverdone(socket);
    close();
}
