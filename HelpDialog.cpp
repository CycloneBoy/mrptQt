#include "HelpDialog.hpp"
#include "../build-mrptQt-Desktop_Qt_5_10_0_GCC_64bit-Debug/ui_Helpdialog.h"

using namespace std;

HelpDialog::HelpDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HelpDialog)
{
    ui->setupUi(this);
    setColors();
    setText();
}


void HelpDialog::setColors(){

    QPalette pal = ui->textEdit->palette();
    pal.setColor(QPalette::Base, this->palette().color(this->backgroundRole()));
    ui->textEdit->setPalette(pal);

}

void HelpDialog::setText(){

    QFile file(":/new/prefix1/help.html");

    if(!file.open(QIODevice::ReadOnly)){
        QMessageBox::information(this, "Error", file.errorString());
    }

    QTextStream in(&file);
    QString text = "";

    while(!in.atEnd()){
        text.append(in.readAll());
    }

    ui->textEdit->setHtml(text);

}

HelpDialog::~HelpDialog()
{
    delete ui;
}
