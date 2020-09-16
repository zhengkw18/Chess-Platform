#ifndef UPDIALOG_H
#define UPDIALOG_H

#include <QDialog>

namespace Ui {
class UpDialog;
}

class UpDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UpDialog(QWidget *parent);
    ~UpDialog();
    int getSetting();
private slots:
    void on_pushButton_clicked();

private:
    Ui::UpDialog *ui;
};

#endif // UPDIALOG_H
