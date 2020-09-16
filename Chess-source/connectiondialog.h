#ifndef CONNECTIONDIALOG_H
#define CONNECTIONDIALOG_H

#include <QDialog>
#include <utils.h>

namespace Ui {
class ConnectionDialog;
}

class ConnectionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConnectionDialog(QWidget *parent);
    ~ConnectionDialog();
    void displayConnecting();
    void cancelConnecting();
signals:
    void serverdone(QTcpSocket*);
    void clientdone(QTcpSocket*);
private slots:
    void OK();
    void Cancel();
    void connected();
    void error();
    void newconnection();
private:
    Ui::ConnectionDialog *ui;
    QString ip="127.0.0.1";
    int port=8888;
    QWidget *parent;
    QTcpSocket *socket;
    QTcpServer *server;
};

#endif // CONNECTIONDIALOG_H
