#include "updialog.h"
#include "ui_updialog.h"

UpDialog::UpDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UpDialog)
{
    ui->setupUi(this);
}

UpDialog::~UpDialog()
{
    delete ui;
}
int UpDialog::getSetting(){
    this->exec();
    return ui->comboBox->currentIndex();
}

void UpDialog::on_pushButton_clicked()
{
    close();
}
